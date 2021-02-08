#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "string_util.h"

#define MAXWORD 100

typedef struct tnode {
  char *word;
  int count;
  struct tnode *left;
  struct tnode *right;
} tnode;

tnode *talloc(void);
char *sdup(char *w);
tnode *addtree(tnode *p, char *w);
void treeprint(tnode *r);
void make_tnodearr(tnode *root, tnode **nodes);
void sort_by_count(tnode **nodes, int n);
int treecount;
int i;

int main() {
  struct tnode *root;
  char word[MAXWORD];
  root = NULL;
  treecount = 0;
  int lc = 0;
  int *lcp = &lc;

  while(getword(word, MAXWORD, lcp) != EOF) {
    if (isalpha(word[0]))
      root = addtree(root, word);
    printf("%d\n", lc);
  }
  
  
  treeprint(root);
  
  tnode **nodes = malloc(treecount * sizeof (tnode *) + 1);
  nodes[treecount] = NULL;
  make_tnodearr(root, nodes);
  printf("tree size: %d\n", treecount);

  

  sort_by_count(nodes, treecount);
  i = 0;
  while(nodes[i] != NULL) {
    printf("Count: %4d, Word %s\n", nodes[i]->count, nodes[i]->word);
    i++;
  }

  tnode *count_tree;
  
  //make_count_tree(root, count_tree);
}

void sort_by_count(tnode **nodes, int n) {
  int gap, i, j;
  tnode *temp;

  for(gap = n/2; gap > 0; gap /= 2)
    for(i = gap; i < n; i++)
      for(j = i - gap; j >= 0 && nodes[j]->count < nodes[j + gap]->count; j -= gap) {
	temp = nodes[j];
	nodes[j] = nodes[j+gap];
	nodes[j+gap] = temp;
      }
}


/* tnode *make_count_tree(tnode *root) { */
/*   if(root != NULL) { */
/*     ma */
/*   } */
/* } */

void make_tnodearr(tnode *root, tnode **nodes) {
  if(root != NULL) {
    make_tnodearr(root->left, nodes);
    nodes[i++] = root;
    make_tnodearr(root->right, nodes);
  }
}

tnode *talloc() {
  return (tnode *) malloc(sizeof(tnode));
}

char *sdup(char *w) {
  char *p;
  p = (char *) malloc(sizeof(char) * (strlen(w) + 1));
  if (p != NULL)
    strcpy(p, w);
  return p;
}

void treeprint(tnode *p) {
  if (p != NULL) {
    treecount++;
    treeprint(p->left);
    printf("Count: %4d    Word:    %s\n", p->count, p->word);
    treeprint(p->right);
  }
}


tnode *addtree(tnode *p, char *w) {
  int cond;

  if (p == NULL) {
    p = talloc();
    p->count = 1;
    p->word = strdup(w);
    p->left = p->right = NULL;
  }
  else if((cond = strcmp(w, p->word)) < 0)
    p->left = addtree(p->left, w);
  else if (cond > 0)
    p->right = addtree(p->right, w);
  else
    p->count++;

  return p;
}
