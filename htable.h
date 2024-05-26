#define HTABLE_SIZE 512

char **init_htable(void);
unsigned int hash(char * s);
void htableInsert(char * s, char**htable);
void htableClear(char** htable);
int htableLookup(char *s, char**htable);
void debugHtable(char** ht);
