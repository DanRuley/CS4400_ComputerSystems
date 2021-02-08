#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

struct student{
  char name[50];
  int uid;
  float gpa;
};

typedef struct student student;

void incr_gpa(student a, student *b){
  a.gpa += 0.1;
  b->gpa += 0.1;
}



typedef struct node {
    short s;
    int i;
    double d;
    struct node *next;    
} node;


int main(){

  /* student s = {"Bob",1234567, 3.9}; */
  /* student j = {"Jane",7654321, 3.6}; */
  /* incr_gpa(s, &j); */
  /* printf("%.1f\n", s.gpa); */
  /* printf("%.1f\n", j.gpa); */



  node n;
  printf("Total size: %ld\nshort is %ld bytes away, int is %ld bytes away, double is %ld bytes away, node* is %ld bytes away\n", sizeof(node), offsetof(n, s), offsetof(n, i), offsetof(n, d), offsetof(n, next));
  
}
