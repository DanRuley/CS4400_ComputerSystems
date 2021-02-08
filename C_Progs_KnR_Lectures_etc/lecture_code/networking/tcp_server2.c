#include "csapp.h"

int main(int argc, char **argv) {
  char *portno;
  int ls, s;

  if (argc != 2)
    app_error("need <port>");

  portno = argv[1];

  ls = Open_listenfd(portno);

  Listen(ls, 5);

  while (1) {
    struct sockaddr_in addr;
    unsigned int len = sizeof(addr);
    size_t total_amt = 0;

    s = Accept(ls, (struct sockaddr *)&addr, &len);

    while (1) {
      char buffer[MAXBUF];
      size_t amt;
      
      amt = Rio_readn(s, buffer, MAXBUF);
      if (amt == 0) {
        printf("client is done\n");
        Rio_writen(s, &total_amt, sizeof(total_amt));
        break;
      } else {
        Rio_writen(1, buffer, amt);
        Rio_writen(1, "\n", 1);
        total_amt += amt;
      }
    }

    Close(s);
  }

  return 0;
}
