#include "csapp.h"
#include "dictionary.h"
#include "more_string.h"

/*
 *Given a dictionary of dictionaries where the keys of D1 are strings mapped to dictionaries 
 *and each level 2 dictionary contains string keys mapped to NULL:
 *Print the resulting graph in webgraphviz format*/

static void print_webgraphviz(dictionary_t *d){
  int i = 0, j;

  dictionary_t *seen = make_dictionary(COMPARE_CASE_INSENS, NULL);


  const char **keys = dictionary_keys(d);
  
  printf("graph Friends {\n");
  while(keys[i] != NULL){
    const char **subkeys = dictionary_keys(dictionary_get(d, keys[i]));
    j = 0;
    while(subkeys[j] != NULL){
      char *s1 = append_strings(keys[i], subkeys[j], NULL);
      char *s2 = append_strings(subkeys[j], keys[i], NULL);
      if(!dictionary_get(seen, s1)){
	printf("%s -- %s\n", keys[i], subkeys[j]);
	dictionary_set(seen, s1, s2);
	dictionary_set(seen, s2, s1);
      }
      free(s1);
      free(s2);
      j++;
    }
    free(subkeys);
    i++;
  }

  printf("}\n");
  free_dictionary(seen);
  free(keys);
}


static void print_stringdictionary(dictionary_t *d) {
  int i, count;

  count = dictionary_count(d);
  for (i = 0; i < count; i++) {
    printf("%s=%s\n",
           dictionary_key(d, i),
           (const char *)dictionary_value(d, i));
  }
  printf("\n");
}

int main(){
  int i;  
  int c = 5;
  dictionary_t *friends = make_dictionary(COMPARE_CASE_INSENS, (void(*)(void *))free_dictionary);
  char *names[5];

  names[0] = "Alice";
  names[1] = "Jim";
  names[2] = "Bob";
  names[3] = "Carol";
  names[4] = "Sue";

  for(i = 0; i < 5; i++)
    dictionary_set(friends, names[i], make_dictionary(COMPARE_CASE_SENS, NULL));

  
  dictionary_set(dictionary_get(friends, names[0]), names[2], NULL);
  dictionary_set(dictionary_get(friends, names[0]), names[3], NULL);
  dictionary_set(dictionary_get(friends, names[0]), names[4], NULL);

  dictionary_set(dictionary_get(friends, names[1]), names[2], NULL);

  dictionary_set(dictionary_get(friends, names[2]), names[0], NULL);
  dictionary_set(dictionary_get(friends, names[2]), names[1], NULL);
  dictionary_set(dictionary_get(friends, names[2]), names[4], NULL);
 
  dictionary_set(dictionary_get(friends, names[3]), names[0], NULL);
  dictionary_set(dictionary_get(friends, names[3]), names[4], NULL);

  dictionary_set(dictionary_get(friends, names[4]), names[0], NULL);
  dictionary_set(dictionary_get(friends, names[4]), names[2], NULL);
  dictionary_set(dictionary_get(friends, names[4]), names[3], NULL);

  print_webgraphviz(friends);



  free_dictionary(friends);
  
}
