#ifndef FM_H
#define FM_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <ncurses.h>
#include <fcntl.h>
#include <errno.h> // Include errno.h header file

#include "marking.h"
#include "htable.h"
#include "rename.h"

#define WHATPAIR(c,marking) \
  ((c) == 'y' ? 1 : ((c) == 'd' ? 2 : color_map[marking->command]))  
typedef struct curr_node {
  int nf;
  int nh;
  int fileflag;
  char *name;
  char **hiddens;
  char **child;
  char **parent;
} curr_node;


typedef struct num_files_t{
  /*pair value of number of files */
  int nf;
  int nh;
} num_files_t;

void get_num_files(char* path, num_files_t *nf);
//int get_num_files(char * path);
void con_files(char *path);
int compare_node(const void *a, const void *b);
int compare_string(const void *a, const void *b);
//void draw_curr_level(char c, int item, char **htable);
void draw_curr_level(char c, int item,  char** htable, marked_t *marking);
//void draw_child_level(int item, char * cwd, char **htable);
void draw_child_level(char c, int item, char *cwd, char **htable, marked_t *marking);
void print_level(char** level, int num);
void update_curr_level(int *p_index);

char** con_ch_files(char *filename);
char** con_pa_files(char *filename, int* p_index);

int is_dir(char *filename);
void draw_paren_level(int *p_index);
void openTextEditor(char *filename);
void displayCurrPath(void);
void showFileContents(char *filename);

//void draw_menu_item(int i, WINDOW *w);
void draw_menu_item(int i, WINDOW *prompt, int sub_h, int sub_w, int sub_y, int sub_x);
int deletePrompt(marked_t *marking);
void ht2marking(marked_t *marking, char**htable);
void cdHandler(char* cwd, char* path);
#endif
