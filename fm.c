#include "fm.h"

/***********
 * globals *
 * *********/
curr_node* curr_level;
char ** child_level;
char ** parent_level;
int c_menumax;
int p_menumax;
int ch_menumax;
unsigned char color_map[256] = {0};


WINDOW *curr_win;
WINDOW *paren_win;
WINDOW *child_win;

WINDOW *cp_button;
int main(int argc, char* argv[]){
  int cdflag;
  int cwdlen, hashval;
  char cwd[MAXPATHLEN];
  char **env;
  char local[MAXPATHLEN]= {0};
  char **htable;
	int menuitem,p_index, key, ret;
  marked_t *marking; 
	menuitem = 0;
	p_index = 0;
	cdflag = 0;
  if(argc > 1 && !(strcmp(argv[1], "cd"))){
    cdflag = 1;
  }
  if(getcwd(cwd, sizeof(cwd)) == NULL){
    fprintf(stderr, "Failed to get current working directory:87\n");
    exit(EXIT_FAILURE);
  }
  marking = init_marked();
  htable = init_htable();
  color_map['y'] = 1;
  color_map['d'] = 2;
  
  /*Initialize before getting into main loop */
  update_curr_level(&p_index);
  initscr();
  start_color();
  use_default_colors();

  init_pair(1, COLOR_WHITE, COLOR_RED); /*Initialize the color pair for mark */
  init_pair(2, COLOR_WHITE, COLOR_BLUE); /*Initialize the color pair for mark */
  
  curs_set(0); /* Disable cursor, hide it */

  if((curr_win = newwin(LINES-5,COLS/9*2+5, 2, COLS/9*2+5)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  if((paren_win = newwin(LINES-5,COLS/9*2, 2, 1)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  if((child_win = newwin(LINES-5,COLS/9*5, 2,COLS/9*4 +10)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  if((cp_button = newwin(1,5, LINES-2, 1)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  
  mvwaddstr(cp_button, 0, 0, "COPY");


  refresh();
  scrollok(curr_win, TRUE);
  displayCurrPath();
  draw_paren_level(&p_index);
  draw_child_level('i', menuitem, cwd, htable, marking);
  draw_curr_level('i', menuitem,htable, marking); 
  wrefresh(cp_button);
  wrefresh(curr_win);
  wrefresh(child_win);
  keypad(stdscr,TRUE);
  noecho();
  do {
    key = getch();
    switch(key){
      case 'j':
        if(menuitem == c_menumax-1){
          break;
        }
        menuitem++;
        break;
      case 'k':
        if(menuitem == 0){
          break;
        }
        menuitem--;
        break;
      case 'h':
        chdir("..");
        displayCurrPath();
        menuitem = p_index;
        update_curr_level(&p_index);
        break;
      case 'l':
        if(curr_level[menuitem].fileflag){
          endwin();
          openTextEditor(curr_level[menuitem].name);
          initscr();
        }else{
          chdir(curr_level[menuitem].name);
          displayCurrPath();
          menuitem = 0;
          update_curr_level(&p_index);
        }
        break;
      case 'd':
      case 'y':
        cwdlen = strlen(cwd);
        strcat(local, cwd);
        local[cwdlen] = '/';
        cwdlen++;
        strcat(local+cwdlen,curr_level[menuitem].name);
        if(!htableLookup(local, htable) || key != marking->command ) {
          updateMarking(marking,key, local, htable);
        }else{
          hashval = hash(local);
          free(htable[hashval]);
          htable[hashval] = NULL;
          marking->num--;
        }
        memset(local, 0, MAXPATHLEN);
        break;
      case 'D':
        ht2marking(marking ,htable);
        if((ret = deletePrompt(marking)) == 0){
          deleteAll(marking, htable);
          update_curr_level(&p_index);
        }
        freeResources(marking, htable);
        menuitem = 0;
        break;
      case 'p':
        env = ready2fire(marking, htable);
        executeCommand(marking, htable, env);
        update_curr_level(&p_index);
        freeResources(marking, htable);
        break;
      case 'r':
        renameHandler(curr_level[menuitem].name);
        update_curr_level(&p_index);
        break;
    }
    if(getcwd(cwd, sizeof(cwd)) == NULL){
      fprintf(stderr, "Failed to get current working directory:87\n");
      exit(EXIT_FAILURE);
    }
    draw_curr_level(key ,menuitem, htable, marking);
    draw_child_level(key,menuitem,cwd, htable, marking);
    draw_paren_level(&p_index);
    wrefresh(curr_win);
    wrefresh(child_win);
  }while(key != 'q');
  echo();
  endwin();
  if(cdflag){
    cdHandler(cwd, argv[2]);
  }
  printf("%s\n",cwd);
  return 0;
}

void cdHandler(char* cwd, char* path){
  FILE *fp;
  fp = fopen(path, "w");
  fprintf(fp, "%s",cwd);
  fclose(fp);
}

int deletePrompt(marked_t *marking){
  WINDOW *prompt;
  int key;
  int i = 0;
  int j;
  int sub_h = LINES/3 + marking->num;
  int sub_w = COLS/3;
  int sub_y = LINES/4;
  int sub_x = COLS/3;
   
  if((prompt= newwin(sub_h, sub_w, sub_y, sub_x)) == NULL){
    perror("newwin");
    exit(EXIT_FAILURE);
  }
  box(prompt, 0,0);
  mvwprintw(prompt, 1, 1, "Deleting Following %d Files?", marking->num);
  
  for(j = 0; j < marking->num; j++){
    mvwprintw(prompt, j + 3, 2 , "%s",marking->src[j]);
  }
  
  wattron(prompt, A_STANDOUT);
  mvwprintw(prompt, sub_h -2, sub_w/2 -8, "YES");
  wattroff(prompt, A_STANDOUT);
  mvwprintw(prompt, sub_h -2, sub_w/2 +8, "NO");
  wrefresh(prompt);
  do {
    key = getch();
    switch (key){
      case 'h' :
        i = 0;
        break;
      case 'l' :
        i = 1;
        break;
    }
    wrefresh(prompt);
    draw_menu_item(i, prompt, sub_h,sub_w, sub_y, sub_x);
  } while(key != '\n');
  delwin(prompt);
  return i;
}

void ht2marking(marked_t *marking, char**htable){
  int i = 0; 
  int j = 0;
  while(j < 512){
    if(htable[j]){
      marking->src[i++] = htable[j];
    }
    j++;
  }

}


void draw_menu_item(int i, WINDOW *prompt, int sub_h, int sub_w, int sub_y, int sub_x){
  mvwprintw(prompt, sub_h-2, sub_w/2 -8, "   ");
  mvwprintw(prompt, sub_h-2, sub_w/2 +8, "  ");
  
  if(i == 0){
    wattron(prompt, A_STANDOUT);
    mvwprintw(prompt, sub_h-2, sub_w/2 -8, "YES");
    wattroff(prompt, A_STANDOUT);
    mvwprintw(prompt, sub_h-2, sub_w/2 +8, "NO");
  }else{
    wattron(prompt, A_STANDOUT);
    mvwprintw(prompt, sub_h-2, sub_w/2 +8, "NO");
    wattroff(prompt, A_STANDOUT);
    mvwprintw(prompt, sub_h-2, sub_w/2 -8, "YES");
  }
  wrefresh(prompt);
}


void openTextEditor(char *filename){
  char cwd[MAXPATHLEN];
  pid_t pid;
  int i = 0;
  if(getcwd(cwd,sizeof(cwd)) == NULL){
    fprintf(stderr, "Failed to getcwd in openTextEditor\n");
    return;
  }
  while(cwd[i] != '\0'){
    i++;
  }
  cwd[i] = '/';
  pid = fork();
  if(pid == -1){
    fprintf(stderr, "Failed to fork : openTextEditor\n");
    return;
  }else if(pid == 0){
    strcat(cwd, filename);
    execlp("vim", "vim", cwd, NULL);
  }else{
    wait(NULL);
  }
}


void update_curr_level(int *p_index){
  /*
   * update current level 
   * get the number of files in current directory
   *
   */
  int num_files;
  num_files_t nfiles;
  get_num_files(".", &nfiles);
  num_files = nfiles.nf;
  c_menumax = num_files;

  if(curr_level){
    free(curr_level);
  }
  if(num_files == 0){
    if((curr_level = malloc(sizeof(curr_node))) == NULL){
      perror("malloc");
      exit(EXIT_FAILURE);
    } 
    curr_level[0].parent = con_pa_files("..", p_index);
    return;
  }
  if((curr_level = malloc(sizeof(curr_node) * c_menumax)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  } 

  con_files(".");
  qsort(curr_level, c_menumax, sizeof(curr_node), compare_node);
  curr_level[0].parent = con_pa_files("..", p_index);
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
  while((entry =readdir(dir)) != NULL){
    filename = entry->d_name;
    if(filename[0] == '.'){

    }else{
      curr[i].name = strdup(filename);
      curr[i].child = NULL;
      if(is_dir(filename)){
        curr[i].child = con_ch_files(filename, i);
        curr[i].fileflag = 0;
      }else{
        curr[i].fileflag = 1;
      }
      i++;
    }
  }

  closedir(dir);
}

char **con_ch_files(char *path, int index){
  DIR *dir;
  struct dirent *entry;
  int i = 0;
  int num_files;
  char **ch_files= NULL;
  char *filename = NULL;
  num_files_t nfiles;
  get_num_files(path, &nfiles);
  num_files = nfiles.nf;
  if(num_files == 0){
    return NULL;
  }
  ch_menumax = num_files;

  if((ch_files = malloc(sizeof(char*) * num_files + 1)) == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  } 

  if((dir = opendir(path)) == NULL){
		if(errno == EACCES){
			curr_level[index].pm = 0;
			return NULL;
		}
    fprintf(stderr, "Failed to get opendir in con_ch_files\n");
    exit(EXIT_FAILURE);
  }
	curr_level[index].pm = 1;
  
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

char **con_pa_files(char * path, int *p_index){
  DIR *dir;
  struct dirent *entry;
  num_files_t nfiles;
  int i;
  int j;
	int num_files;
  char cwd[MAXPATHLEN];
  char *parentdir = NULL;
  char *filename;
	char **pa_files;
	i = 0;
	j = 0;

  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd");
    exit(EXIT_FAILURE);
  }

  if((parentdir= strrchr(cwd, '/')) == NULL){
    perror("strrchr");
    exit(EXIT_FAILURE);
  }
  parentdir++;  /* skip '/' */

  
  if((dir = opendir(path)) == NULL){
    fprintf(stderr, "Failed to get opendir in con_pa_files\n");
    exit(EXIT_FAILURE);
  }
	
  pa_files = NULL;
  get_num_files(path, &nfiles);
  num_files = nfiles.nf;

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

  for(j = 0; j < i; j++){
    /* Due to qsort, we have to recompute the parent index */
    if(!strcmp(parentdir,pa_files[j])){
      *p_index = j; 
    }
  }

  closedir(dir);
  return pa_files;

}

void draw_child_level(char c, int item, char *cwd, char **htable, marked_t *marking){
  int i,chmenu;
  char **localchild = curr_level[item].child;
  int cwdlen;
  char newstr[MAXPATHLEN] = {0};
	i = 0;
	chmenu = 0;
  werase(child_win);
  strcat(newstr, cwd);
  cwdlen = strlen(newstr);
  newstr[cwdlen] = '/';
  strcat(newstr, curr_level[item].name);
  cwdlen = strlen(newstr);
  newstr[cwdlen] = '/';
  cwdlen++;
   
  if(c_menumax == 0){
    werase(child_win);
    wrefresh(child_win);
    return;
  }else if (curr_level[item].fileflag){
    showFileContents(curr_level[item].name);
    return;
  }
  
	if(curr_level[item].pm == 0){
    mvwaddstr(child_win, 0, 0, "Permissio Require");
    wrefresh(child_win);
    return;
	}
  if(!localchild){
    mvwaddstr(child_win, 0, 0, "Empty");
    wrefresh(child_win);
    return;
  }
  
  while(localchild[chmenu] != NULL){
    chmenu++;
  }
  werase(child_win);
  wrefresh(child_win);
  
  for(i = 0; i< chmenu; i++){
    strcat(newstr,localchild[i]);
  
    if(i == 0){
     wattron(child_win, A_REVERSE); 
    }
    mvwaddstr(child_win, i, 2, localchild[i]);
    wattroff(child_win,A_REVERSE);
    if(htableLookup(newstr, htable)){
     wattrset(child_win,COLOR_PAIR(WHATPAIR(c, marking))| A_BOLD);
     mvwaddstr(child_win, i, 0 ,"-");
     wrefresh(child_win);
     wattroff(child_win,COLOR_PAIR(WHATPAIR(c, marking))| A_BOLD);
    }
    memset(newstr+cwdlen,0, MAXPATHLEN-cwdlen);
  }
  wrefresh(child_win);
}

void draw_curr_level(char c, int item,  char** htable, marked_t *marking){
  int i=0;
  int cwdlen;
  char cwd[MAXPATHLEN];
  char *newstr;
  if(getcwd(cwd, sizeof(cwd)) == NULL){
    fprintf(stderr, "Failed to get current working directory:87\n");
    exit(EXIT_FAILURE);
  }
  cwd[strlen(cwd)] = '/';
  cwdlen = strlen(cwd);
   
  werase(curr_win);
  wrefresh(curr_win);
  if(marking->command != c){
    
  }

  for (i = 0; i < c_menumax; i++) {
    newstr = strcat(cwd,curr_level[i].name);
    if(i == item) {
      wattron(curr_win, A_STANDOUT);
    }
    mvwaddnstr(curr_win, i, 2, curr_level[i].name,COLS/9*2);
    wattroff(curr_win, A_STANDOUT);
    if(htableLookup(newstr, htable)){
     wattrset(curr_win,COLOR_PAIR(WHATPAIR(c, marking))| A_BOLD);
     mvwaddstr(curr_win, i, 0 ,"-");
     wrefresh(curr_win);
     wattroff(curr_win,COLOR_PAIR(WHATPAIR(c, marking))| A_BOLD);
    }
    memset(cwd+cwdlen,0, strlen(curr_level[i].name));
  }


  if(c_menumax == 0){
    wattron(curr_win, A_STANDOUT);
    mvwaddstr(curr_win, i, 0, "Empty ");
  }
}

void draw_paren_level(int *p_ind){
  int i;
  char cwd[MAXPATHLEN];
  if(getcwd(cwd, sizeof(cwd)) == NULL){
    fprintf(stderr, "Failed to getcwd in 292\n");
    exit(EXIT_FAILURE);
  }
  werase(paren_win);
  wrefresh(paren_win);
  if(!(strcmp(cwd, "/"))){
    return;  
  }
  for (i = 0; i < p_menumax; i++ ){
    /*
    if(!strcmp(curr_level[0].parent[i], parent)){
      wattron(paren_win, A_REVERSE);
      *p_ind = i;
    }
    */
    if(*p_ind == i){
      wattron(paren_win, A_REVERSE);
    }
    mvwaddstr(paren_win, i, 1, curr_level[0].parent[i]);
    wattroff(paren_win, A_REVERSE);
  }
  wrefresh(paren_win);
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
		if(errno == EACCES){
			return;
		}
    fprintf(stderr, "Failed to get opendinr in get_num_files\n");
		exit(EXIT_FAILURE);
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


void showFileContents(char *filename){
  int fd;
  char buffer[2048];
	ssize_t readbytes;
  if((fd = open(filename , O_RDONLY)) == -1){
    if(errno == EACCES){
      mvwaddstr(child_win,0,0,"Permission Denied\n");
      wattron(child_win, A_REVERSE); 
      wrefresh(child_win);
      return;
    }
    fprintf(stderr, "Failed to open file\n ");
    exit(EXIT_FAILURE);
  }
  readbytes = read(fd, buffer, sizeof(buffer) - 1);
  if(readbytes == -1){
    fprintf(stderr, "Failed to read from file\n");
    exit(EXIT_FAILURE);
  }
    
  werase(child_win);
  mvwaddstr(child_win, 0, 0,buffer);
  wrefresh(child_win);
  close(fd);
}

void displayCurrPath(void){
  char cwd[MAXPATHLEN];
  if(getcwd(cwd, sizeof(cwd)) == NULL){
    fprintf(stderr, "Failed to getcwd\n");
    return;
  }
  mvaddstr(0,0, "                                                     ");
  mvaddstr(0,0, cwd);
}

