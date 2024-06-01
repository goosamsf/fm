#define DEFAULT_SRC_LEN 2048
#define DEFAULT_DST_LEN 512
#define DEFAULT_NUM_SRC 64
typedef struct marked_t{
  char command;
  int num;
  char* src[DEFAULT_NUM_SRC];
  char* dst;
} marked_t;


marked_t *init_marked(void);
void debugMarking(marked_t *mark);
void updateMarking(marked_t *mark, char newcommand, char *cwd, char** htable);
char** ready2fire(marked_t *mark, char **htable);
void executeCommand(marked_t *marking, char** htable, char**env);

void deleteAll(marked_t *marking, char** htable);

void freeResources(marked_t *marking, char** htable);
