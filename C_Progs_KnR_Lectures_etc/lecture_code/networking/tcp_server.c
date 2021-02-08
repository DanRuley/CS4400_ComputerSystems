#include "csapp.h"

int main(int argc, char **argv) {
  char *portno;
  int ls, s;
  struct addrinfo hints, *addrs;

  if (argc != 2)
    app_error("need <port>");

  portno = argv[1];

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;        /* Request IPv4 */
  hints.ai_socktype = SOCK_STREAM;  /* Accept TCP connections */
  hints.ai_flags = AI_PASSIVE;      /* ... on any IP address */
  Getaddrinfo(NULL, portno, &hints, &addrs);

  ls = Socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
  Bind(ls, addrs->ai_addr, addrs->ai_addrlen);
  Freeaddrinfo(addrs);

  Listen(ls, 5);

  while (1) {
    struct sockaddr_in addr;
    unsigned int len = sizeof(addr);
    char buffer[MAXBUF];
    size_t amt;

    s = Accept(ls, (struct sockaddr *)&addr, &len);

    amt = Read(s, buffer, MAXBUF);
    write(1, buffer, amt);
    write(1, "\n", 1);
    write(s, &amt, sizeof(amt));

    Close(s);
  }

  return 0;
}
