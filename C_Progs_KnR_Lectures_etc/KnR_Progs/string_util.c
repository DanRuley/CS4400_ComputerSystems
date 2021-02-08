#include "string_util.h"

char buf[BUFSIZE];
int bufp = 0;
unsigned char in_quote;
unsigned char in_block;
unsigned char in_diamond;

int getch(void) {
  return bufp > 0 ? buf[--bufp] : getchar();
}

void ungetch(int c) {
  if (bufp >= BUFSIZE)
    printf("ungetch: too many characters\n");
  else
    buf[bufp++] = c;
}

int getword(char *word, int lim, int *lc)
{
  int c;
  char *w = word;
  printf("%s\n",w);
  while(isspace(c = getch()))
    ;
  
  if(c == '"') {
    while((c = getch()) != EOF) {
      if (c == '"')
	break;
    }
  }

  if(c == '\n')
    (*lc)++;

  if(c == '/'){

    if((c = getch()) == '*') {
      while((c = getch()) != EOF)
	if(c == '*')
	  if((c = getch()) == '/')
	    break;
    }

    else if (c == '/') {
      while((c = getch()) != EOF)
	if(c == '\n')
	  break;  
    }
  }

  if(c == '<')
    while((c = getch()) != EOF)
      if(c == '>')
	break;

  if(c != EOF)
    *w++ = c;
  if(!isalpha(c)) {
    *w = '\0';
    return c;
  }
  for( ; --lim > 0; w++)
    if(!isalnum(*w = getch()) && *w != '_') {
      ungetch(*w);
      break;
    }
  *w = '\0';
  return word[0];
}
