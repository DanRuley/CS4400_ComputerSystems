#include "csapp.h"

int main(int argc, char **argv) {
  char *hostname;
  struct addrinfo hints, *addrs, *addr;
  char host[256];

  if (argc != 2)
    app_error("need <hostname>");

  hostname = argv[1];

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;       /* Request IPv4 */
  hints.ai_socktype = SOCK_STREAM; /* TCP connection */
  Getaddrinfo(hostname, NULL, &hints, &addrs);

  for (addr = addrs; addr != NULL; addr = addr->ai_next) {
    Getnameinfo(addr->ai_addr, addr->ai_addrlen,
                host, sizeof(host),
                NULL, 0,
                NI_NUMERICHOST);
    printf("%s\n", host);
  }
  
  Freeaddrinfo(addrs);
  
  return 0;
}
