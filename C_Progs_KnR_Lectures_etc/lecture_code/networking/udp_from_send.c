#include "csapp.h"

int main(int argc, char **argv)
{
  char *myportno, *hostname, *portno;
  struct addrinfo hints, *addrs, *my_addrs;
  char host[256], serv[32];
  int s;
  int copies;
  size_t amt = 0;

  if ((argc != 5) && (argc != 6))
    app_error("need <port> <hostname> <port> <message> [<repeat>]");

  myportno = argv[1];
  hostname = argv[2];
  portno = argv[3];

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;      /* Request IPv4 */
  hints.ai_socktype = SOCK_DGRAM; /* UDP connection */
  Getaddrinfo(hostname, portno, &hints, &addrs);

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = addrs->ai_family;
  hints.ai_socktype = addrs->ai_socktype;
  hints.ai_protocol = addrs->ai_protocol;
  hints.ai_flags = AI_PASSIVE;    /* ... from any IP address */
  Getaddrinfo(NULL, myportno, &hints, &my_addrs);

  Getnameinfo(addrs->ai_addr, addrs->ai_addrlen,
              host, sizeof(host),
              serv, sizeof(serv),
              NI_NUMERICHOST | NI_NUMERICSERV);
  printf("sending to %s:%s\n", host, serv);
      
  s = Socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
  Bind(s, my_addrs->ai_addr, my_addrs->ai_addrlen);
  Freeaddrinfo(my_addrs);

  if (argc == 6)
    copies = atoi(argv[5]);
  else
    copies = 1;

  while (copies--)
    amt = Sendto(s, argv[4], strlen(argv[4]), 0,
                 addrs->ai_addr, addrs->ai_addrlen);
  
  Freeaddrinfo(addrs);
  
  return (amt != strlen(argv[3]));
}
