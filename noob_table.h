typedef struct {
  char *key;
  char *value;
} item;

typedef struct {
  int size;
  int count;
  int base_size;
  item **items;
} table;
