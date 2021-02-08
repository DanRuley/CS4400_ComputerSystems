#include "csapp.h"

int main(int argc, char **argv) {
  int s, copies;
  size_t amt, got, len;
  char *hostname, *portno;
  struct addrinfo hints, *addrs;

  if ((argc != 4) && (argc != 5))
    app_error("need <hostname> <port> <message> [<copies>]");

  hostname = argv[1];
  portno = argv[2];

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;        /* Request IPv4 */
  hints.ai_socktype = SOCK_STREAM;  /* TCP connection */
  Getaddrinfo(hostname, portno, &hints, &addrs);
      
  s = Socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);

  Connect(s, addrs->ai_addr, addrs->ai_addrlen);
  Freeaddrinfo(addrs);

  if (argc == 5)
    copies = atoi(argv[4]);
  else
    copies = 1;

  len = strlen(argv[3]);

  while (copies--) {
    amt = Write(s, argv[3], len);
    if (amt != len)
      app_error("incomplete write");
  }
  
  got = 0;
  amt = Read(s, &got, sizeof(got));
  printf("server got %ld\n", got);
  
  Close(s);
  
  return 0;
}
