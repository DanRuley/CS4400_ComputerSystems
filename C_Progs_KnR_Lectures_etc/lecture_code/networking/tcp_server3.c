#include "csapp.h"

int main(int argc, char **argv) {
  char *portno;
  int ls, s;

  if (argc != 2)
    app_error("need <port>");

  portno = argv[1];

  ls = Open_listenfd(portno);

  while (1) {
    struct sockaddr_in addr;
    unsigned int len = sizeof(addr);
    size_t total_amt, amt;
    char *buffer;

    s = Accept(ls, (struct sockaddr *)&addr, &len);

    amt = Rio_readn(s, &total_amt, sizeof(total_amt));
    if (amt != sizeof(total_amt))
      app_error("amount truncated");

    buffer = malloc(total_amt);
    amt = Rio_readn(s, buffer, total_amt);

    Rio_writen(1, buffer, amt);
    Rio_writen(1, "\n", 1);
    
    free(buffer);

    write(s, &amt, sizeof(amt));
    
    Close(s);
  }

  return 0;
}
