#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct int_list {
  int v;
  struct int_list *next;

} int_list;


typedef union uni{
  struct s1 *h;
  struct s2 *i;
  char j;
} uni;

typedef struct s1{
  char a[3];
  union uni b;
  int c;
} s1;

typedef struct s2{
  struct s1 *d;
  char e;
  int f[4];
  struct s2 *g;
} s2;

int proc(struct s1 *x){
  return x->b.i>f[3];
  return 0;
}


int main(){
  
  
  

  char c[3] = {'a','b','c'};
  int n[4] = {1,2,3,4};
  //struct s1 ex1 = {c, NULL, 5};
  //struct s2 ex2 = {NULL, 'a', n, NULL};

  s1 ex1;
  s2 ex2;
  uni un = {&ex2};
  ex1.a[0] = 'a';
  ex1.a[1] = 'b';
  ex1.a[2] = 'c';
  ex1.b = un;
  ex1.c = 5;


  ex2.d = &ex1;
  ex2.e = 'z';
  int i;
  for(i = 0; i < 4; i++)
    ex2.f[i] = 17;
  ex2.g = &ex2;

  s1 *e1p = &ex1;

  printf("%d\n",e1p->b.i->f[3]);

}
