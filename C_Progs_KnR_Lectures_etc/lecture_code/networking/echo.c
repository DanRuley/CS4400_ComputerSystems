#include "csapp.h"

void *echo(void *);

static size_t counter;
sem_t count_sem;

int main(int argc, char **argv) {
  pthread_t th;
  int listenfd, connfd, *connfd_p;
  char client_hostname[MAXLINE], client_port[MAXLINE];
  struct sockaddr_storage clientaddr; /* Enough room for any addr */
  
  if (argc != 2)
    app_error("need <port>");
  
  listenfd = Open_listenfd(argv[1]);
  while (1) {
    socklen_t clientlen = sizeof(struct sockaddr_storage);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    
    Getnameinfo((SA *) &clientaddr, clientlen,
                client_hostname, MAXLINE, client_port, MAXLINE, 0);
    printf("Connected to (%s, %s)\n", client_hostname, client_port);
    
    connfd_p = malloc(sizeof(int));
    *connfd_p = connfd;
    
    Pthread_create(&th, NULL, echo, connfd_p);
    Pthread_detach(th);
  }
  
  return 0;
}

void *echo(void *connfd_p) {
  int connfd = *(int *)connfd_p;
  size_t n;
  char buf[MAXLINE];
  rio_t rio;

  free(connfd_p);

  Sem_init(&count_sem, 0, 1);
  
  Rio_readinitb(&rio, connfd);
  while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    //printf("server received %ld bytes\n", n);
    P(&count_sem);
    counter += n;
    V(&count_sem);
    Rio_writen(connfd, buf, n);
  }

  printf("total so far: %ld\n", counter);

  Close(connfd);

  return NULL;
}
