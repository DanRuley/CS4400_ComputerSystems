#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct node{
  int val;
  int num;
  struct node* next;
  
}node;

int main(){

  node n1 = {207, 1, NULL}; 
  node n2 = {90, 2, NULL};
  node n3 = {431, 3, NULL};
  node n4 = {642, 4, NULL};
  node n5 = {349, 5, NULL};
  node n6 = {608, 6, NULL};
  n1.next = &n2;
  n2.next = &n3;
  n3.next = &n4;
  n4.next = &n5;
  n5.next = &n6;
  int i;
  node *temp = &n1;
  /* for(i = 0; i < 6; i++) { */
  /*     printf("%d ", temp->val); */
  /*     temp = temp->next; */
  /*   } */
  printf("Up until >98 it basically makes sure all of my numbers - 1 are not greater than 5.\nThen it just sets up variables before the loop at >111.\n");
  int nums[6] = {1,2,3,4,5,6};
  int *rsi, *rax;
  int ecx, edx, esi;
  node *edxp;

  rax = (int*)nums;
  rsi = (int*)nums + 6;
  ecx = 7;
  printf("The loop from >111 to >124 applies the following transformation to my numbers:\n");
  for(; rax < rsi; rax++) {
    printf("%d --> ", *rax);
    edx = ecx;
    edx -= *rax;
    *rax = edx;
    printf("%d\n", *rax);
  } 

  printf("\n>126:\n");
  esi = 0;
  printf("jump >165\n");
  ecx = nums[0];
  if ( ecx <= 11)
    goto l146;

 l146:
  edxp = &n1;
}
