#include "fm.h"

char **init_htable(void){
  char** htable = NULL;
  if((htable = malloc(sizeof(char*)*512)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  memset(htable, 0 , sizeof(char*)* 512);
  return htable;
}

unsigned int hash(char * s){
  unsigned int hashval;
  for(hashval = 0; *s != '\0'; s++){
    hashval = *s + 31 * hashval;
  }
  return hashval % 512;
}

void htableInsert(char * s, char**htable){
  if(htableLookup(s, htable)){
    return;
  }
  htable[hash(s)] = strdup(s);
}

void htableClear(char** htable){
  int i;
  for(i = 0; i< 512; i++){
    if(htable[i]){
      free(htable[i]);
      htable[i] = NULL;
    }
  }
}

int htableLookup(char *s, char**htable){
  int hval = hash(s);
  if(htable[hval] != NULL){
    if(!strcmp(s, htable[hval])){
      return 1;
    }

  }
  return 0;
}

void debugHtable(char** ht){
  for(int i = 0; i< 512; i++){
    if(ht[i]){
      printf("%d,-%s-\n", i, ht[i]);
    }
  }
}
