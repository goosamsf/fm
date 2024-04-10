#include <stdio.h>
#include <dirent.h>

#include <string.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <ncurses.h>

int get_num_files(char * path);
void con_files(char *path);
int compare_node(const void *a, const void *b);
int compare_string(const void *a, const void *b);
void draw_curr_level(int item);
void print_level(char** level, int num);
void update_curr_level(void);

char** con_ch_files(char *filename);
char** con_pa_files(char *filename);
int is_dir(char *filename);

void draw_paren_level(char *parent);
typedef struct curr_node {
  char *name;
  char **child;
  char **parent;
} curr_node;
