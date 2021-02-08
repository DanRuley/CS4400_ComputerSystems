#include "csapp.h"

int main(int argc, char **argv) {
  char *portno;
  struct addrinfo hints, *addrs;
  int s;

  if (argc != 2)
    app_error("need <port>");

  portno = argv[1];

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;        /* Request IPv4 */
  hints.ai_socktype = SOCK_DGRAM;   /* Accept UDP connections */
  hints.ai_flags = AI_PASSIVE;      /* ... on any IP address */
  Getaddrinfo(NULL, portno, &hints, &addrs);

  s = Socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
  
  Bind(s, addrs->ai_addr, addrs->ai_addrlen);
  Freeaddrinfo(addrs);

  while (1) {
    char buffer[MAXBUF];
    size_t amt;
    amt = Recv(s, buffer, MAXBUF, 0);
    Write(1, buffer, amt);
    Write(1, "\n", 1);
  }

  return 0;
}
