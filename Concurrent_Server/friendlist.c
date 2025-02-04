/*
 * friendlist.c - a web-based friend-graph manager.
 *
 * Based on:
 *  tiny.c - A simple, iterative HTTP/1.0 Web server that uses the 
 *      GET method to serve static and dynamic content.
 *   Tiny Web server
 *   Dave O'Hallaron
 *   Carnegie Mellon University
 *
 * Implementation completed by Dan Ruley u0956834
 */
#include "csapp.h"
#include "dictionary.h"
#include "more_string.h"

//Global friends_graph dictionary shared by all threads
static dictionary_t *friends_graph;

//The port this server runs on
static int this_port;

//Semaphore used to synchronize friends_graph access between threads
static sem_t sem;

static void *doit(void *fdp);
static dictionary_t *read_requesthdrs(rio_t *rp);
static void read_postquery(rio_t *rp, dictionary_t *headers, dictionary_t *d);
static void clienterror(int fd, char *cause, char *errnum, 
                        char *shortmsg, char *longmsg);

static void print_webgraphviz(dictionary_t *d);
static void print_stringdictionary(dictionary_t *d);
static void seed_dictionary();

static void serve_header(size_t len, int fd);
static void serve_friends(int fd, dictionary_t *query);
static void serve_befriend(int fd, dictionary_t *query);
static void serve_unfriend(int fd, dictionary_t *query);
static void serve_introduce(int fd, dictionary_t *query);


static void free_strs(char **strs);
static void add_friends(char *user, char **friends, char *friend);
static void remove_friends(char *user, char **friends);
static char* get_friends_response(char *user);




int main(int argc, char **argv) {
  int listenfd, connfd, *connfd_p;
  pthread_t th;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  friends_graph = make_dictionary(COMPARE_CASE_SENS, (void(*)(void*))free_dictionary);
  
  //initialize the semaphore
  Sem_init(&sem, 0, 1);

  listenfd = Open_listenfd(argv[1]);
  this_port = atoi(argv[1]);

  /* Don't kill the server if there's an error, because
     we want to survive errors due to a client. But we
     do want to report errors. */
  exit_on_error(0);

  /* Also, don't stop on broken connections: */
  Signal(SIGPIPE, SIG_IGN);

  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    if (connfd >= 0) {

      Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                  port, MAXLINE, 0);
      //printf("Accepted connection from (%s, %s)\n", hostname, port);
      
      //handle the work of serving each client on separate threads
      connfd_p = malloc(sizeof(int));
      *connfd_p = connfd;
      Pthread_create(&th, NULL, doit, connfd_p);
      Pthread_detach(th);
    }
  }
}

/*
 * doit - handle one HTTP request/response transaction
 */
void *doit(void *fdp) {
  int fd = *(int *)fdp;
  free(fdp);

  char buf[MAXLINE], *method, *uri, *version;
  rio_t rio;
  dictionary_t *headers, *query;

  /* Read request line and headers */
  Rio_readinitb(&rio, fd);
  if (Rio_readlineb(&rio, buf, MAXLINE) <= 0)
    return NULL;
  //printf("%s", buf);
  
  if (!parse_request_line(buf, &method, &uri, &version)) {
    clienterror(fd, method, "400", "Bad Request",
                "Friendlist did not recognize the request");
  } else {
    if (strcasecmp(version, "HTTP/1.0")
        && strcasecmp(version, "HTTP/1.1")) {
      clienterror(fd, version, "501", "Not Implemented",
                  "Friendlist does not implement that version");
    } else if (strcasecmp(method, "GET")
               && strcasecmp(method, "POST")) {
      clienterror(fd, method, "501", "Not Implemented",
                  "Friendlist does not implement that method");
    } else {
      headers = read_requesthdrs(&rio);

      /* Parse all query arguments into a dictionary */
      query = make_dictionary(COMPARE_CASE_SENS, free);
      parse_uriquery(uri, query);
      if (!strcasecmp(method, "POST"))
        read_postquery(&rio, headers, query);

      /* Serve the specified query, since these are so littered with friends_graph dictionary accesses,
       it seemed easiest just to lock this entire block*/
      P(&sem);
      if(starts_with("/friends", uri))
	serve_friends(fd, query);
      else if(starts_with("/befriend", uri))
	serve_befriend(fd, query);
      else if(starts_with("/unfriend", uri))
	serve_unfriend(fd, query);
      else if(starts_with("/introduce", uri))
	serve_introduce(fd, query);
      V(&sem);

      /* Clean up */
      free_dictionary(query);
      free_dictionary(headers);
    }

    /* Clean up status line */
    free(method);
    free(uri);
    free(version);
  }

  Close(fd);
  return NULL;
}

/*
 * read_requesthdrs - read HTTP request headers
 */
dictionary_t *read_requesthdrs(rio_t *rp) {
  char buf[MAXLINE];
  dictionary_t *d = make_dictionary(COMPARE_CASE_INSENS, free);

  Rio_readlineb(rp, buf, MAXLINE);
  //printf("%s", buf);
  while(strcmp(buf, "\r\n")) {
    Rio_readlineb(rp, buf, MAXLINE);
    //printf("%s", buf);
    parse_header_line(buf, d);
  }
  
  return d;
}

/*Read the POST header*/
void read_postquery(rio_t *rp, dictionary_t *headers, dictionary_t *dest) {
  char *len_str, *type, *buffer;
  int len;
  
  len_str = dictionary_get(headers, "Content-Length");
  len = (len_str ? atoi(len_str) : 0);

  type = dictionary_get(headers, "Content-Type");
  
  buffer = malloc(len+1);
  Rio_readnb(rp, buffer, len);
  buffer[len] = 0;

  if (!strcasecmp(type, "application/x-www-form-urlencoded")) {
    parse_query(buffer, dest);
  }

  free(buffer);
}

/*Make OK response header indicating a future write of len bytes*/
static char *ok_header(size_t len, const char *content_type) {
  char *len_str, *header;
  
  header = append_strings("HTTP/1.0 200 OK\r\n",
                          "Server: Friendlist Web Server\r\n",
                          "Connection: close\r\n",
                          "Content-length: ", len_str = to_string(len), "\r\n",
                          "Content-type: ", content_type, "\r\n\r\n",
                          NULL);
  free(len_str);

  return header;
}

/*Send the generic OK header and size to the client*/
static void serve_header(size_t len, int fd){
  char *header;

  /* Send response headers to client */
  header = ok_header(len, "text/html; charset=utf-8");
  Rio_writen(fd, header, strlen(header));
  //printf("Response headers:\n");
  //printf("%s", header);  
  
  free(header);
}

/*Serves the friends request - simply responds with a newline separated list containing the user's friends*/
static void serve_friends(int fd, dictionary_t *query) {
  size_t len;
  char *body, *user;

  user = dictionary_get(query, "user");

  body = get_friends_response(user);
  len = body == NULL ? 0 : strlen(body);

  serve_header(len, fd);
  Rio_writen(fd, body, len);
  
  if(body)
    free(body);
}

/*returns a newline separated string containing all of the user's friends - this must be freed by the caller*/
static char* get_friends_response(char *user){
  char *body;

  if(!dictionary_get(friends_graph, user))
    return NULL;

  const char **friends = dictionary_keys((dictionary_t *)dictionary_get(friends_graph, user));

  body = join_strings((const char * const *)friends, '\n');
  free(friends);
  
  return body;
}

static void free_strs(char **strs){
  int i = 0;

  while(strs[i] != NULL)
      free(strs[i++]);
    
  free(strs);
}

/*Serves the befriend request - adds friendship pairs between user and the friends in the query.
  Responds with a list of user's friends after the friends are added*/
static void serve_befriend(int fd, dictionary_t *query) {
  size_t len;
  char *body, *user;
  char **friends;
  
  user = dictionary_get(query, "user");

  friends = split_string(dictionary_get(query, "friends"), '\n');  
  add_friends(user, friends, NULL);
  
  body = get_friends_response(user);
  len = body == NULL ? 0 : strlen(body);
  
  serve_header(len, fd);  
  Rio_writen(fd, body, len);  

  free_strs(friends);
  if(body)
    free(body);
}

/*Adds friendship pairs to the graph.
Calling formats: (user, friends, NULL) -> add friendships between user and all friends
                 (user, NULL, friend)  -> add singleton friendship between user and friend */
static void add_friends(char *user, char **friends, char *friend) {
  int i = 0;
  
  if(!dictionary_get(friends_graph, user))
    dictionary_set(friends_graph, user, make_dictionary(COMPARE_CASE_SENS, free));
  
  //singleton
  if(friend){
    if(!dictionary_get(friends_graph, friend))
      dictionary_set(friends_graph, friend, make_dictionary(COMPARE_CASE_SENS, NULL));
    
    if(strcmp(friend, user)){
      dictionary_set(dictionary_get(friends_graph, user), friend, NULL);
      dictionary_set(dictionary_get(friends_graph, friend), user, NULL);
    }
    return;
  }

  //multiple friends
  while(friends[i] != NULL) {
    if(!strcmp(friends[i], user)) {
	i++;
	continue;
      }
      if(!dictionary_get(friends_graph, friends[i]))
	dictionary_set(friends_graph, friends[i], make_dictionary(COMPARE_CASE_SENS, NULL));
      
      dictionary_set(dictionary_get(friends_graph, user), friends[i], NULL);
      dictionary_set(dictionary_get(friends_graph, friends[i]), user, NULL);
      i++;
  }
}

/*Serve the unfriend request - removes the specified friend pairs from the graph
  and returns the remaining friends for <user>*/
static void serve_unfriend(int fd, dictionary_t *query) {
  size_t len;
  char *body, *user;
  char **friends;
  
  user = dictionary_get(query, "user");

  friends = split_string(dictionary_get(query, "friends"), '\n');
  
  remove_friends(user, friends);
      
  body = get_friends_response(user);
  len = body == NULL ? 0 : strlen(body);

  serve_header(len, fd);

  Rio_writen(fd, body, len);

  free_strs(friends);
  
  if(body)
    free(body);
}

/*Remove friend pairs between user and each string in friends, if they exist*/
static void remove_friends(char *user, char **friends){
  
  int i = 0;

  if(!dictionary_get(friends_graph, user))
    return;

  while(friends[i] != NULL){
    if(!dictionary_get(friends_graph, friends[i])) {
      i++;
      continue;
    }
    
    dictionary_remove(dictionary_get(friends_graph, user), friends[i]);
    dictionary_remove(dictionary_get(friends_graph, friends[i]), user);
    i++;
  }
}

/*Serve the introduce query.  If the port is different from this server, then connect with a socket
and use the GET /friends? request to retrieve the friends.  Otherwise, just handle the introduction
internally*/
static void serve_introduce(int fd, dictionary_t *query) {
  char *user, *hostname, *portno, *friend, *request;
  char buf[MAXLINE];
  int server_fd;
  size_t len;
  rio_t rio;

  user = dictionary_get(query, "user");
  hostname = dictionary_get(query, "host");
  portno = dictionary_get(query, "port");
  friend = dictionary_get(query, "friend");

  //It's a different server
  if(this_port != atoi(portno)) {
    request = append_strings("GET /friends?user=", friend, " HTTP/1.1\r\n\r\n", NULL);
    len = strlen(request);
    server_fd = Open_clientfd(hostname, portno);
  
    Rio_writen(server_fd, request, len);
    Rio_readinitb(&rio, server_fd);

    //read the initial html line to get response status
    Rio_readlineb(&rio, buf, MAXLINE);
        
    char **html = split_string(buf, ' ');

    //stop if not OK
    if(atoi(html[1]) != 200){
      free_strs(html);
      return;
    }

    while(strcmp(buf, "\r\n"))
      Rio_readlineb(&rio, buf, MAXLINE);
  
    while(Rio_readlineb(&rio, buf, MAXLINE)) {
      //trim off the newline
      buf[strlen(buf) - 1] = '\0';
      add_friends(user, NULL, buf);
    }

    free_strs(html);  
    free(request);
  
    Close(server_fd);
  }

  //Same server
  else {
    if(dictionary_get(friends_graph, friend)) {
      const char **friends = dictionary_keys(dictionary_get(friends_graph, friend));
      add_friends(user, (char **)friends, NULL);
      add_friends(user, NULL, friend);
      free(friends);
    }
  }

  char *friends = get_friends_response(user);
  char *body = append_strings("Introductions complete.  Current friends for user ", user, ":\n", friends, NULL);
  len = strlen(body);
  serve_header(len, fd);

  //printf("%s\n", body);
  Rio_writen(fd, body, len);

  free(friends);
  free(body); 
}


/*
 * clienterror - returns an error message to the client
 */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) {
  size_t len;
  char *header, *body, *len_str;

  body = append_strings("<html><title>Friendlist Error</title>",
                        "<body bgcolor=""ffffff"">\r\n",
                        errnum, " ", shortmsg,
                        "<p>", longmsg, ": ", cause,
                        "<hr><em>Friendlist Server</em>\r\n",
                        NULL);
  len = strlen(body);

  /* Print the HTTP response */
  header = append_strings("HTTP/1.0 ", errnum, " ", shortmsg, "\r\n",
                          "Content-type: text/html; charset=utf-8\r\n",
                          "Content-length: ", len_str = to_string(len), "\r\n\r\n",
                          NULL);
  free(len_str);
  
  Rio_writen(fd, header, strlen(header));
  Rio_writen(fd, body, len);

  free(header);
  free(body);
}

/*Meh debugging print fn*/
static void print_stringdictionary(dictionary_t *d) {
  int i, count;
  printf("Printing string dictionary\n");
  count = dictionary_count(d);
  for (i = 0; i < count; i++) {
    printf("%s=%s\n",
           dictionary_key(d, i),
           (const char *)dictionary_value(d, i));
  }
  printf("\n");
}

/*Nice debugging print fn
Print a representation of the friends graph that can be used at the webgraphviz website*/
static void print_webgraphviz(dictionary_t *d){
  int i = 0, j;

  dictionary_t *seen = make_dictionary(COMPARE_CASE_INSENS, NULL);

  const char **keys = dictionary_keys(d);
  
  printf("graph Friends {\n");
  while(keys[i] != NULL){
    const char **subkeys = dictionary_keys(dictionary_get(d, keys[i]));
    j = 0;
    while(subkeys[j] != NULL){
      char *s1 = append_strings(keys[i], subkeys[j], NULL);
      char *s2 = append_strings(subkeys[j], keys[i], NULL);
      if(!dictionary_get(seen, s1)){
	printf("%s -- %s\n", keys[i], subkeys[j]);
	dictionary_set(seen, s1, s2);
	dictionary_set(seen, s2, s1);
      }
      free(s1);
      free(s2);
      j++;
    }
    free(subkeys);
    i++;
  }

  printf("}\n");
  free_dictionary(seen);
  free(keys);
}


/*Seed the graph with some values for debugging*/
static void seed_dictionary(){
  char *names[7];

  names[0] = "Alice";
  names[1] = "Jim";
  names[2] = "Bob";
  names[3] = "Carol";
  names[4] = "Sue";
  names[5] = "Jimbob";
  names[6] = "SunnyJim";

  int i;
  for(i = 0; i < 7; i++)
    dictionary_set(friends_graph, names[i], make_dictionary(COMPARE_CASE_SENS, free));
  
  add_friends(names[0], NULL, names[2]);
  add_friends(names[0], NULL, names[3]);
  add_friends(names[0], NULL, names[4]);
  add_friends(names[0], NULL, names[6]);

  add_friends(names[1], NULL, names[2]);
  add_friends(names[1], NULL, names[5]);

  add_friends(names[2], NULL, names[0]);
  add_friends(names[2], NULL, names[1]);
  add_friends(names[2], NULL, names[4]);
  add_friends(names[2], NULL, names[6]);

  add_friends(names[3], NULL, names[0]);
  add_friends(names[3], NULL, names[4]);

  add_friends(names[4], NULL, names[0]);
  add_friends(names[4], NULL, names[2]);
  add_friends(names[4], NULL, names[3]);

  add_friends(names[5], NULL, names[1]);
  add_friends(names[5], NULL, names[6]);

  add_friends(names[6], NULL, names[0]);
  add_friends(names[6], NULL, names[2]);
  add_friends(names[6], NULL, names[5]);
}
