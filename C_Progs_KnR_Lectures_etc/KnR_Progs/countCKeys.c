#include "string_util.h"
//volatile volatile volatile volatile

#define MAXWORD 100

typedef struct key{
  char *word;
  int count;
} key;

#define NKEYS (sizeof(tab) / sizeof(key))

static int binsearch(char *, int);
static key *binsearchp(char *, int);

static key tab[] = {
  {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0},
  {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0},
  {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0},
  {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0}
};

static int binsearch(char *word, int n){

  int cond;
  int low = 0, mid, high = n - 1;

  while (low <= high) {
    mid = (low + high) / 2;
    if((cond = strcmp(word, tab[mid].word)) < 0)
      high = mid - 1;
    else if (cond > 0)
      low = mid + 1;
    else
      return mid;
  }

  return -1;
}


static key *binsearchp(char *word, int n){
  int cond;
  key *low = &tab[0];
  key *hi = &tab[n-1];
  key *mid;

  while(low < hi) {
    mid = low + (hi - low) / 2;
    if((cond = strcmp(word, mid->word)) > 0)
      low = mid + 1;
    else if (cond < 0)
      hi = mid;
    else
      return mid;
  }
  return NULL;
}

int main() {
  printf("%ld %ld\n", sizeof(key), sizeof(tab));
  key *p;
  char word[MAXWORD];
  printf("%d\n", NKEYS);

  while(getword(word, MAXWORD) != EOF){
    if(isalpha(word[0]))
      if((p = binsearchp(word, NKEYS)) != NULL)
	p->count++;
    //printf("%s\n", word);
  }

  for(p = &tab[0]; p <= &tab[NKEYS - 1]; p++)
    printf("Count: %4d for keyword: %s\n", p->count, p->word);

  return 0;
}

/* static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* };static key tab[] = { */
/*   {"auto", 0}, {"break", 0}, {"case", 0}, {"char", 0}, {"const", 0}, {"continue", 0}, {"default", 0}, {"do", 0}, */
/*   {"double", 0}, {"else", 0}, {"enum", 0}, {"extern", 0}, {"float", 0}, {"for", 0}, {"goto", 0}, {"if", 0}, */
/*   {"int", 0}, {"long", 0}, {"register", 0}, {"return", 0}, {"short", 0}, {"signed", 0}, {"sizeof", 0}, {"static", 0}, */
/*   {"struct", 0}, {"switch", 0}, {"typedef", 0}, {"union", 0}, {"unsigned", 0}, {"void", 0}, {"volatile", 0}, {"while", 0} */
/* }; */
