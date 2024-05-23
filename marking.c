#include "fm.h"

marked_t *init_marked(void){
  marked_t * mark = NULL;
  if((mark = malloc(sizeof(marked_t))) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  if((mark->src = malloc(sizeof(char) * DEFAULT_SRC_LEN)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  if((mark->dst = malloc(sizeof(char) * DEFAULT_DST_LEN)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  memset(mark->src, 0, DEFAULT_SRC_LEN);
  memset(mark->dst, 0, DEFAULT_DST_LEN);
  mark->command = 'i';
  mark->num = 0;
  return mark;
}

void updateMarking(marked_t *mark, char newcommand, char *cwd, char **htable){
  if(mark->command != newcommand){
    mark->command = newcommand;
    htableClear(htable);
  } 
  htableInsert(cwd, htable);
  mark->num++;  
}

void readySrc(marked_t *mark, char **htable){
  int i,j =0;
  int len; 
  for(i = 0; i < 512; i++){
    if(htable[i]){
      len = strlen(htable[i]);
      strcat(mark->src + j, htable[i]);
      j += len;
      mark->src[j++] = ' ';
    }
  }
}

void debugMarking(marked_t *mark){
  printf("Command : %c\n", mark->command);
  printf("Number of src : %d\n", mark->num);
  printf("Src : %s\n", mark->src);
  printf("Dst : %s\n", mark->dst);
}


