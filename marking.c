#include "fm.h"

marked_t *init_marked(void){
  marked_t * mark = NULL;
  if((mark = malloc(sizeof(marked_t))) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  /*
  if((mark->src = malloc(sizeof(char*) * DEFAULT_NUM_SRC)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  */

  if((mark->dst = malloc(sizeof(char) * DEFAULT_DST_LEN)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  memset(mark->src, 0, sizeof(char*)*DEFAULT_NUM_SRC);
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

char **ready2fire(marked_t *mark, char **htable){
  uint16_t i,j =0;
  uint8_t k = 2 + (mark->command == 'y' || mark->command == 'x');
  char **ret = NULL;

  if((ret = malloc(sizeof(char*)*mark->num+k)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  /* ENV must be NULL terminated */
  ret[mark->num+k-1] = NULL;
  switch(mark->command){
    case 'y':
      ret[0] = "cp";
      break;
    case 'x':
      ret[0] = "mv";
      break;
    case 'r':
      ret[0] = "mv";
    case 'd':
      ret[0] = "rm";
    default:
      //print error message:
  }
  for(i = 0; i < HTABLE_SIZE; i++){
    if(htable[i]){
      ret[j++] = htable[i];
    }
  }
  return ret;
}

void debugMarking(marked_t *mark){
  printf("Command : %c\n-", mark->command);
  printf("Number of src : %d\n", mark->num);
  printf("Src : %s\n", mark->src);
  printf("Dst : %s\n", mark->dst);
}

void executeCommand(marked_t *marking, char** htable, char**env){
  char cwd[MAXPATHLEN];
  if(getcwd(cwd, sizeof(cwd)) == NULL){
    fprintf(stderr, "Failed to get current working directory:87\n");
    exit(EXIT_FAILURE);
  }
  /* Access marked_t structure         */
  /* - see what the holding command is  m-> mv, D-> rm, y->cp, 
   * - set the destination
   */
  pid_t pid;
  char *command;
  switch(marking->command){
    case 'y': 
      command = "cp";
      break; 
    default:
      break;
  }
  pid = fork();
  if(pid == -1){
    fprintf(stderr, "Failed to fork : openTextEditor\n");
    return;
  }else if(pid == 0){
    int i,j = 0;
    char *env[marking->num+3];   
    char *dst = strndup(cwd, strlen(cwd));
    
    env[0] = "cp";
    for(i = 1; i < marking->num+1; i++){
      for(; j < 512;j++){
        if(htable[j]){
          env[i] = htable[j++];
          break;
        }
      }
    }
    env[marking->num+3-2] = dst;
    env[marking->num+3-1] = NULL;

    execvp(command, env);
  }else{
    wait(NULL);
  }
  

}


