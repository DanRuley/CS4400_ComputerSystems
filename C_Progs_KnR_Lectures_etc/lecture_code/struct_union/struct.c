#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct point {
  int x;
  int y;

}point;


typedef struct rect {
  point bottom_l;
  point top_l;
  
  point bottom_r;
  point top_r;
}rect;



struct rect initialize_rect(point *p1, point *p2){

  if(p1->x == p2->x || p1->y == p2->y){
    printf("Error, initialization points must be opposite corners.\n");
    return;
  }
  
  int x_diff = p1->x < p2->x ? p2->x - p1->x : p1->x - p2->x;
  int y_diff = p1->y < p2->y ? p2->y - p1->y : p1->y - p2->y;

  //p1 is bottom-l, p2 is top-r
  if(p1->x < p2->x && p1-> y < p2->y){
    rect ret = {*p1, {p1->x, p1->y + y_diff}, {p2->x, p2->y - y_diff}, *p2};
    return ret;
  }

  //p1 is upper-left, p2 is bottom-right
  else if(p1->x < p2->x && p1->y > p2->y){
    rect ret = {{p1->x, p1->y - y_diff}, *p1, *p2, {p2->x, p2->y + y_diff}};
    return ret;
  }
  
    //p1 is bottom-r, p2 is top-l
  else if(p1->x > p2->x && p1->y < p2->y){
    rect ret = {{p2->x, p2->y - y_diff}, *p2, *p1, {p1->x, p1->y + y_diff}};
    return ret;
  }
  //p1 is top-r, p2 is bottom-l
  else{
    rect ret = {*p2, {p2->x, p2->y + y_diff}, {p1->x, p1->y - y_diff}, *p1};
    return ret;
  }

}


void print_line(int size, char edge, char inner){
  char c[size + 1];
  c[0] = edge;
  int i;
  for(i = 1; i < size - 1; i++)
    c[i] = inner;
  c[size-1] = edge;
  c[size] = '\0';
  printf("%s\n", c);
}

void print_rect(rect *r){
  point *p1 = &r->bottom_l;
  point *p2 = &r->top_r;

  int x_diff = p1->x < p2->x ? p2->x - p1->x : p1->x - p2->x;
  int y_diff = p1->y < p2->y ? p2->y - p1->y : p1->y - p2->y;
  int i, j;

  print_line(x_diff, '+', '-');
  for(i = 0; i < y_diff - 2; i++)
    print_line(x_diff, '|', ' ');
  print_line(x_diff, '+', '-');
}

int main(int argc, char *argv){

  point p2 = {1, 13};
  point p = {5, 17};

  printf("p is located at address: %x\nx is %d bytes away\ny is %d bytes away\n", &p, offsetof(point, x), offsetof(point, y));
  point *pp = &p;

  rect r = initialize_rect(&p, &p2);

  print_rect(&r);

  return 0;
}



















/* typedef struct element { */
/*   char name[17]; */
/*   char symbol[5]; */
/*   int atom_no; */
/*   double mass; */

/* } element; */


/* void add_neutrons(element *e){ */
/*   e->mass++; */
/* } */

/* void report(element *e) { */
/*   printf("%s is atomic number %d\nIts symbol is: %s, and its atomic mass is: %f\n", e->name, e->atom_no, e->symbol, e->mass); */
/* } */
