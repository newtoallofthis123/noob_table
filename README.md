# NoobTable

A simple implementation of a hash table in C,
A slightly modified version of the tutorial by [James Routley](https://github.com/jamesroutley/write-a-hash-table/)

## Documentation:

```c
static item *new_item(const char *k, const char *v);
table *new_table_with_size(int base_size);
table *new_table();
void set_item(table *t, const char *key, const char *value);
char *get_item(table *t, char *key);
void delete_item(table *t, char *key);
void print_items(table *t);
```

Here, the table and item are defined as follows:

```c
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
```

The item is for internal representation only and it is not needed for operation

## TODO

See [todo.txt](todo.txt) for more details.

## License

This project is under the MIT License, the details of which can be found in the [LICENSE](LICENSE) file.
