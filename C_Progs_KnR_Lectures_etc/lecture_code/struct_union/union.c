#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/*A number *and* a string*/
//|size|    |  color   |
//0    4    8          15
//     ^bit locations^
struct t_shirt {
  int size;
  char *color;
};

/*A number *or* a string */
//A union is an or of field values.
//Note it must have memory to accomodate the larger of the two data types.
//[grade][...]
//0     3    7
//[excuse]
//0      7  
union homework_result {
  int grade;
  char *excuse;
};

struct homework_record {
  int graded;
  union homework_result r;
};

  
void got_doctor_note(struct homework_record *h){
  h->graded = 0;
  h->r.excuse = "illness";
}


int main() {

  struct homework_record h;
  h.graded = 1;
  h.r.grade = 0;

  got_doctor_note(&h);

  if(h.graded)
    printf("%d\n", h.r.grade);
  else
    printf("%s\n", h.r.excuse);
  
}
