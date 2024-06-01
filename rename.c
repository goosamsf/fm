#include "fm.h"

void renameHandler(char *cwd){
  /*
   * This function handles in response to the 'y' key pressed
wrefresh(win);

    // Enable the cursor
    curs_set(1);
   */
  WINDOW *prompt;
  pid_t pid;
  echo();
  char *env[4];
  char tempstr[64];
  int sub_h = LINES/3 - 7;
  int sub_w = COLS/3;
  int sub_y = LINES/4;
  int sub_x = COLS/3;
  if((prompt= newwin(sub_h, sub_w, sub_y, sub_x)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  box(prompt, 0,0);
  mvwprintw(prompt, 1, 1, " Type new name : ");
  wrefresh(prompt);
  curs_set(1); 
  mvwgetnstr(prompt, 1,18, tempstr, sizeof(tempstr)-1);
  delwin(prompt);
  noecho();
  env[0] = "mv";
  env[1] = cwd;
  env[2] = tempstr; 
  env[3] = NULL;

  pid = fork();
  if(pid == -1){
    
  }else if (pid == 0){
    execvp("mv",env);

  }else{
    wait(NULL);
  }

}
