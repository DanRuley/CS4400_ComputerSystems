#include "csapp.h"

int main(int argc, char **argv) {
  int s, copies;
  size_t amt, got, len;
  char *hostname, *portno;

  if ((argc != 4) && (argc != 5))
    app_error("need <hostname> <port> <message> [<copies>]");

  hostname = argv[1];
  portno = argv[2];

  s = Open_clientfd(hostname, portno);

  if (argc == 5)
    copies = atoi(argv[4]);
  else
    copies = 1;

  len = strlen(argv[3]);

  amt = copies * len;
  Rio_writen(s, &amt, sizeof(amt));
  
  while (copies--)
    Rio_writen(s, argv[3], len);
  
  got = 0;
  amt = Rio_readn(s, &got, sizeof(got));
  if (amt != sizeof(got))
    app_error("response truncated");
  
  printf("server got %ld\n", got);
  
  Close(s);
  
  return 0;
}
