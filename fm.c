#include "fm.h"

/***********
 * globals *
 * *********/
curr_node* curr_level;
char ** child_level;
char ** parent_level;
int c_menumax;

int main(void){
  int menuitem = 0; 
  int key;
  update_curr_level();
  initscr();
  draw_curr_level(menuitem);
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
      default:
        break;
    }
    draw_curr_level(menuitem);
  }while(key != 'q');
  echo();
  endwin();
  
  return 0;
}
void update_curr_level(void){
  int m_num_files = get_num_files(".");
  c_menumax = m_num_files;
  if(curr_level){
    free(curr_level);
  }
  if((curr_level = malloc(sizeof(curr_node) * m_num_files)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  } 
  con_files(".");
  qsort(curr_level, m_num_files, sizeof(curr_node), compare_node);
}


void draw_curr_level(int item){
  int i;
  //print_level(curr_level, m_num_files);
  clear();
  addstr("Printing Current level's files");
  for (i = 0; i < c_menumax; i++) {
    if(i == item) {
      attron(A_REVERSE);
    }
    mvaddstr(3 + (i*2) , 20 , curr_level[i].name);
    attroff(A_REVERSE);
  }
  mvaddstr(17,25, "Use jk keys to move; Enter to select");
  refresh();

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

void con_files(char *path){
  /* construct file list */
  int i = 0;
  DIR *dir;
  struct dirent *entry;
  curr_node* curr = curr_level;
  if((dir = opendir(path)) == NULL){
    fprintf(stderr, "Failed to get opendinr in get_num_files\n");
    return;
  }
  while((entry =readdir(dir)) != NULL){
    if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
      curr[i].name = strdup(entry->d_name);
      if(is_dir(entry->d_name)){
        //If this directory get child directory
        curr[i].child = con_ch_files(entry->d_name);
      }
      i++;
    }
  }
  closedir(dir);
}

char **con_ch_files(char *filename){
  DIR *dir;
  struct dirent *entry;
  int i = 0;
  if((dir = opendir(filename)) == NULL){
    fprintf(stderr, "Failed to get opendinr in get_num_files\n");
    exit(EXIT_FAILURE);
  }
  char **ch_files= NULL;
  int num_files = get_num_files(filename);
  if((ch_files = malloc(sizeof(char*) * num_files)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  } 
  while((entry = readdir(dir)) != NULL){
    if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
      ch_files[i++] = strdup(entry->d_name);
    }
  }
  qsort(ch_files, num_files, sizeof(char*), compare_string);
  //
  for (i = 0; i < num_files; i++){
    printf("%s\n", ch_files[i]);
  }
  closedir(dir);
  return ch_files;
}

int get_num_files(char * path){
  /* read directory specified by path and return number of files
   * without counting . and .. */
  int count = 0;
  DIR *dir;
  struct dirent *entry;
  if((dir = opendir(path)) == NULL){
    fprintf(stderr, "Failed to get opendinr in get_num_files\n");
    return 1;
  }
  while((entry =readdir(dir)) != NULL){
    if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
      count++;
    }
  }
  closedir(dir);
  return count;
}



