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

void freeResources(marked_t *marking, char** htable){
  memset(marking, 0, sizeof(marked_t));
  htableClear(htable);
}

void updateMarking(marked_t *mark, char newcommand, char *cwd, char **htable){
  if(mark->command != newcommand){
    mark->command = newcommand;
    htableClear(htable);
    mark->num = 0;
  } 
  htableInsert(cwd, htable);
  mark->num++;  
}

char **ready2fire(marked_t *mark, char **htable){
  uint16_t i,j =0;
  uint8_t k = 2; 
  char **ret = NULL;
  if(mark->command == 'y'){
    k +=2;
  }else {
    k++;
  }

  if((ret = malloc(sizeof(char*)*mark->num+k)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE); 
  }
  /* ENV must be NULL terminated */
  ret[mark->num+k-1] = NULL;
  switch(mark->command){
    case 'y':
      ret[j++] = "cp";
      ret[j++] = "-r";
      break;
    case 'x':
      ret[j++] = "mv";
      break;
    case 'r':
      ret[j++] = "mv";
			break;
    case 'd':
      ret[j++] = "mv";
			break;
    default:
			break;
  }

  for(i = 0; i < HTABLE_SIZE; i++){
    if(htable[i]){
      ret[j++] = htable[i];
    }
  }
  return ret;
}

void debugMarking(marked_t *mark){
	/*
  printf("Command : %c\n-", mark->command);
  printf("Number of src : %d\n", mark->num);
  printf("Src : %s\n", mark->src);
  printf("Dst : %s\n", mark->dst);
	*/
}

void deleteAll(marked_t *marking, char** htable){
  pid_t pid;
  int i;
  int j = 0;
  char *env[64];
  env[0] = "rm";
  env[1] = "-rf";
  for(i =2; i < 64 && marking->src[j] != NULL; i++){
    env[i] = marking->src[j++];
  }
  env[i] = NULL;
  pid = fork(); 
  if(pid == -1){

  }else if(pid == 0){
    execvp("rm",env);
  }else{
    wait(NULL);
  }

}



void executeCommand(marked_t *marking, char** htable, char**env){
  char cwd[MAXPATHLEN];
  uint8_t k = 2;
  pid_t pid;
  char command[8];
  if(getcwd(cwd, sizeof(cwd)) == NULL){
    fprintf(stderr, "Failed to get current working directory:87\n");
    exit(EXIT_FAILURE);
  }
  if(marking->command == 'y'){
    k +=2;
  }else {
    k++;
  }
  /* Access marked_t structure         */
  /* - see what the holding command is  m-> mv, D-> rm, y->cp, 
   * - set the destination
   */
  strcpy(command, env[0]); 
  env[marking->num+k-2] = cwd;
  env[marking->num+k-1] = NULL;

  pid = fork();
  if(pid == -1){
    fprintf(stderr, "Failed to fork : openTextEditor\n");
    return;
  }else if(pid == 0){
    execvp(command, env);
  }else{
    wait(NULL);
  }
  

}


