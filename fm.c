#include "fm.h"
#include <menu.h>
/***********
 * globals *
 * *********/
curr_node* curr_level;
char ** child_level;
char ** parent_level;
char *hi_parent;
int c_menumax;
int p_menumax;
int ch_menumax;
WINDOW *curr_win;
WINDOW *paren_win;
WINDOW *child_win;

int main(void){
  int menuitem = 0; 
  int key;

  update_curr_level();

  initscr();
  if((curr_win = newwin(LINES,COLS, 2, COLS/3)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  if((paren_win = newwin(LINES,COLS, 2, 1)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  if((child_win = newwin(LINES,COLS, 2,COLS - COLS/3)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  scrollok(curr_win, TRUE);


  printw("window size is %d row, %d,", LINES, COLS);
  refresh();
  draw_paren_level(hi_parent);
  draw_curr_level(menuitem);
  draw_child_level(0);
  //draw_child_level();
  keypad(stdscr,TRUE);
  noecho();
  do {
    key = getch();
    switch(key){
      case 'j':
        menuitem++;
        if(menuitem > c_menumax-1) {
          menuitem = 0;
        }
        break;
      case 'k':
        menuitem--;
        if(menuitem < 0) {
          menuitem = c_menumax-1;
        }
        break;
      case 'h':
        chdir("..");
        update_curr_level();
        menuitem = 0;
        draw_paren_level(hi_parent);
        break;

      case 'l':
        chdir(curr_level[menuitem].name);
        update_curr_level();
        menuitem = 0;
        draw_paren_level(hi_parent);
        break;
      default:
        break;
    }
    draw_curr_level(menuitem);
    draw_child_level(menuitem);
  }while(key != 'q');
  echo();
  endwin();
  
  return 0;
}
void update_curr_level(void){
  /*
   * update current level 
   * get the number of files in current directory
   *
   */
  num_files_t nfiles;
  get_num_files(".", &nfiles);
  int num_files = nfiles.nf;
  int num_hiddens = nfiles.nh;
  
  c_menumax = num_files;
  if(curr_level){
    free(curr_level);
  }
  if((curr_level = malloc(sizeof(curr_node) * c_menumax)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  } 

  con_files(".");
  qsort(curr_level, c_menumax, sizeof(curr_node), compare_node);
  curr_level[0].parent = con_pa_files("..");
}

void con_files(char *path){
  /* construct file list */
  int i = 0;
  DIR *dir;
  struct dirent *entry;
  char *filename;
  curr_node* curr = curr_level;
  if((dir = opendir(path)) == NULL){
    fprintf(stderr, "Failed to get opendinr in get_num_files\n");
    return;
  }
  //curr[0].parent = con_pa_files("..");
  while((entry =readdir(dir)) != NULL){
    filename = entry->d_name;
    if(filename[0] == '.'){
    //if filename begins with dot

    }else{
      //regular file 
      curr[i].name = strdup(filename);
      curr[i].child = NULL;
      if(is_dir(filename)){
        //If this directory get child directory
        curr[i].child = con_ch_files(filename);
      }
      i++;
    }
  }
  closedir(dir);
}

char **con_ch_files(char *path){
  DIR *dir;
  struct dirent *entry;
  int i = 0;
  int num_files;
  char **ch_files= NULL;
  char *filename = NULL;
  num_files_t nfiles;
  get_num_files(path, &nfiles);
  num_files = nfiles.nf;
  ch_menumax = num_files;

  if((ch_files = malloc(sizeof(char*) * num_files + 1)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  } 

  if((dir = opendir(path)) == NULL){
    printf("tryiing to open filename : %s\n",path);
    fprintf(stderr, "Failed to get opendir in con_ch_files\n");
    return NULL;
    exit(EXIT_FAILURE);
  }
  
  //int num_files = get_num_files(filename);
  while((entry = readdir(dir)) != NULL){
    filename = entry->d_name;
    if(filename[0] != '.'){
      ch_files[i++] = strdup(filename);
    }
  }
  qsort(ch_files, num_files, sizeof(char*), compare_string);
  ch_files[i] = NULL;
  closedir(dir);
  return ch_files;
}

char **con_pa_files(char * path){
  DIR *dir;
  struct dirent *entry;
  num_files_t nfiles;
  int i = 0;
  char cwd[MAXPATHLEN];
  char *parentdir = NULL;
  size_t slen;
  char *filename = NULL;
  
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    //printf("Current directory is: %s\n", cwd);
    perror("getcwd");
    exit(EXIT_FAILURE);
  }

  if((parentdir= strrchr(cwd, '/')) == NULL){
    perror("strrchr");
    exit(EXIT_FAILURE);
  }
  parentdir++;  /* skip '/' */
  slen = strlen(parentdir);

  hi_parent = strndup(parentdir,slen ); /* get highlighted value in parent */
  
  if((dir = opendir(path)) == NULL){
    fprintf(stderr, "Failed to get opendir in con_pa_files\n");
    exit(EXIT_FAILURE);
  }
  char **pa_files = NULL;
  //int num_files = get_num_files(filename);
  get_num_files(path, &nfiles);
  int num_files = nfiles.nf;

  p_menumax = num_files;
  if((pa_files = malloc(sizeof(char*)*num_files)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  while((entry = readdir(dir)) != NULL){
    filename = entry->d_name; 
    pa_files[i] = NULL;
    if(filename[0] != '.'){
      pa_files[i++] = strdup(filename);
    }
  }
  qsort(pa_files, num_files, sizeof(char*), compare_string);
  /*
  printf("parent files : \n");
  for (i = 0; i < num_files; i++){
    printf("%s\n", pa_files[i]);
  }
  */
  closedir(dir);
  return pa_files;

}

void draw_child_level(int item){
  int i, chmenu = 0;
  char **localchild = curr_level[item].child;
  werase(child_win);
  wrefresh(child_win);
  
  if(!localchild){
    return;
  }
  
  while(localchild[chmenu] != NULL){
    chmenu++;
  }
  
  for(i = 0; i< chmenu; i++){
    if(i == 0){
     wattron(child_win, A_REVERSE); 
    }
    mvwaddstr(child_win, i, 0, localchild[i]);
    wattroff(child_win,A_REVERSE);
  }
  wrefresh(child_win);
  //refresh();
}

void draw_curr_level(int item){
  int i;
  werase(curr_win);
  wrefresh(curr_win);
  //print_level(curr_level, m_num_files);
  //clear();
  //addstr("Printing Current level's files");
  //mvprintw(2, COLS/COLS, "%d", 0);
  for (i = 0; i < c_menumax; i++) {
    if(i == item) {
      wattron(curr_win, A_REVERSE);
    }
    mvwaddstr(curr_win, i, 0, curr_level[i].name);
    wattroff(curr_win, A_REVERSE);
  }
  //mvwaddstr(curr_win,17,25, "Use jk keys to move; Enter to select");
  wrefresh(curr_win);
  refresh();
}

void draw_paren_level(char *parent){
  int i;
  werase(paren_win);
  wrefresh(paren_win);
  for ( i = 0; i < p_menumax; i++ ){
    if(!strcmp(curr_level[0].parent[i], parent)){
      wattron(paren_win, A_REVERSE);
    }
    mvwaddstr(paren_win, i, 0, curr_level[0].parent[i]);
    wattroff(paren_win, A_REVERSE);
  }
  wrefresh(paren_win);
  //refresh();
}

void print_level(char** level, int num){
  int i;
  for(i = 0; i<num; i++ ){
    printf("%s\n", level[i]);
  }
}

int compare_string(const void *a, const void *b){
  const char *str_a = *(const char **)a;
  const char *str_b = *(const char **)b;
  return strcmp(str_a, str_b);
}

int compare_node(const void *a, const void *b){
  /* compare logic for sort */
  const curr_node *nodeA = (const curr_node *)a;
  const curr_node *nodeB = (const curr_node *)b;
  return strcmp(nodeA->name, nodeB->name);
}

int is_dir(char *filename){
  struct stat fileStat;
  if(stat(filename, &fileStat) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  } 
  return (S_ISDIR(fileStat.st_mode));
}




void get_num_files(char * path, num_files_t *numfiles){
  /* read directory specified by path and return number of files
   * without counting . and .. */
  int count = 0;
  int hidden = 0;
  DIR *dir;
  struct dirent *entry;
  char *filename;
  if((dir = opendir(path)) == NULL){
    fprintf(stderr, "Failed to get opendinr in get_num_files\n");
    return;
  }
  while((entry =readdir(dir)) != NULL){
    filename = entry->d_name;
    if(filename[0]== '.'){
      hidden++;
    }else{
      count++;
    }
  }
  closedir(dir);
  numfiles->nf = count;
  numfiles->nh = hidden;
}



