#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFSIZE 100


int getch(void);
void ungetch(int);
int getword(char *, int, int *);
