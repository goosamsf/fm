#define DEFAULT_SRC_LEN 2048
#define DEFAULT_DST_LEN 512

typedef struct marked_t{
  char command;
  int num;
  char* src;
  char* dst;
} marked_t;


marked_t *init_marked(void);
void debugMarking(marked_t *mark);
void updateMarking(marked_t *mark, char newcommand, char *cwd, char** htable);
void readySrc(marked_t *mark, char **htable);
void executeCommand(marked_t *marking, char** htable);
