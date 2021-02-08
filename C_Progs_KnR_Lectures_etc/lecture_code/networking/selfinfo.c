#include "csapp.h"
#include <ifaddrs.h>

/* Prints all the IPv4 address for the current host
   by using getifaddrs() */

int main(int argc, char **argv, char **envp) {
  char host[256];
  struct ifaddrs *addrs, *addr;
  
  if (getifaddrs(&addrs) != 0)
    unix_error("getifaddrs failed");

  for (addr = addrs; addr != NULL; addr = addr->ifa_next) {
    if (addr->ifa_addr->sa_family == PF_INET) {
      Getnameinfo(addr->ifa_addr, sizeof(struct sockaddr_in),
		  host, sizeof(host),
		  NULL, 0,
		  NI_NUMERICHOST);
      printf("%s\n", host);
    }
  }
  
  freeifaddrs(addrs);
  
  return 0;
}
