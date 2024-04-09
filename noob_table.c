#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "noob_table.h"

#define HT_PRIME_1 109423049273943239
#define HT_PRIME_2 21084079493897983
#define BASE_SIZE 53

static void resize_up(table *t);
static void resize_down(table *t);

// is the given number a prime
int is_prime(const int x) {
  if (x < 2) {
    return -1;
  }
  if (x < 4) {
    return 1;
  }
  if ((x % 2) == 0) {
    return 0;
  }
  for (int i = 3; i <= floor(sqrt((double)x)); i += 2) {
    if ((x % i) == 0) {
      return 0;
    }
  }
  return 1;
}

// Simple function for calculating the next prime
int next_prime(int x) {
  while (is_prime(x) != 1) {
    x++;
  }
  return x;
}

// Makes a new item
static item *new_item(const char *k, const char *v) {
  item *i = malloc(sizeof(item));
  i->key = strdup(k);
  i->value = strdup(v);
  return i;
}

/* new_table_with_size
 *
 * Initializes a new table with a given size
 * */
table *new_table_with_size(int base_size) {
  table *ht = malloc(sizeof(table));

  ht->base_size = base_size;
  ht->size = next_prime(base_size);
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(item *));
  return ht;
}

// Creates a new Table and returns a pointer to the table
table *new_table() { return new_table_with_size(BASE_SIZE); }

// Frees an Item
static void del_item(item *i) {
  free(i->key);
  free(i->value);
  free(i);
}

// Frees the table
void del_table(table *ht) {
  for (int i = 0; i < ht->size; i++) {
    item *item = ht->items[i];
    if (item != NULL) {
      del_item(item);
    }
  }
  free(ht->items);
  free(ht);
}

/* cal_hash
 *
 * Algorithm to calculate the hash
 * */
static int cal_hash(const char *s, const long a, const int m) {
  long hash = 0;
  const int len_s = strlen(s);
  for (int i = 0; i < len_s; i++) {
    hash += (long)pow(a, len_s - (i + 1)) * s[i];
    hash = hash % m;
  }
  return (int)hash;
}

/* get_hash
 *
 * Accepts an index along with the key in order to
 * implement a double hashing algorithm
 * */
static int get_hash(const char *s, const int num_buckets, const int attempt) {
  const int hash_a = cal_hash(s, HT_PRIME_1, num_buckets);
  const int hash_b = cal_hash(s, HT_PRIME_2, num_buckets);
  return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

// Representes a deleted item.
// We cannot actuall delete an item completely, so this is to get around to that
// these are usually flushed out on a resize
static item DELETED_ITEM = {NULL, NULL};

/* set_item
 *
 * Sets the item / inserts an item to the table
 * */
void set_item(table *t, const char *key, const char *value) {
  const int load = t->count * 100 / t->size;
  if (load > 70) {
    resize_up(t);
  }

  item *curr_item;
  item *item = new_item(key, value);
  int index = get_hash(item->key, t->size, 0);
  curr_item = t->items[index];
  int i = 1;
  while (curr_item != NULL && curr_item != &DELETED_ITEM) {
    if (strcmp(curr_item->key, key) == 0) {
      del_item(curr_item);
      t->items[index] = item;
      return;
    }
    index = get_hash(item->key, t->size, i);
    curr_item = t->items[index];
    i++;
  }
  t->items[index] = item;
  t->count++;
}

/* resize_table
 *
 * Resizes the table to another size
 * */
static void resize_table(table *t, const int size) {
  if (size < BASE_SIZE) {
    return;
  }
  table *new_t = new_table_with_size(size);
  for (int i = 0; i < t->size; i++) {
    item *curr_item = t->items[i];
    if (curr_item != NULL && curr_item != &DELETED_ITEM) {
      set_item(new_t, curr_item->key, curr_item->value);
    }
  }

  t->base_size = new_t->base_size;
  t->count = new_t->count;

  const int tmp_size = t->size;
  t->size = new_t->size;
  new_t->size = tmp_size;

  item **tmp_items = t->items;
  t->items = new_t->items;
  new_t->items = tmp_items;

  del_table(new_t);
}

/* resize_up
 *
 * Increases the size of the table
 * */
static void resize_up(table *t) {
  const int new_size = t->base_size * 2;
  resize_table(t, new_size);
}

/* resize_down
 *
 * Reduces the size of the table
 * */
static void resize_down(table *t) {
  const int new_size = t->base_size / 2;
  resize_table(t, new_size);
}

/* get_item
 *
 * Returns the value upon being given the key
 * */
char *get_item(table *t, char *key) {
  item *curr_item;
  int index = get_hash(key, t->size, 0);
  curr_item = t->items[index];
  int i = 1;
  while (curr_item != NULL) {
    if (strcmp(curr_item->key, key) == 0 && curr_item != &DELETED_ITEM) {
      return curr_item->value;
    }

    index = get_hash(key, t->size, i);
    curr_item = t->items[index];
    i++;
  }

  return NULL;
}

/* delete_item
 *
 * Deletes an Item from the table.
 * Relies essentially on the index, so O(1) time
 * */
void delete_item(table *t, char *key) {
  const int load = t->count * 100 / t->size;
  if (load < 10) {
    resize_down(t);
  }
  int index = get_hash(key, t->size, 0);
  item *curr_item = t->items[index];
  int i = 1;
  while (curr_item != NULL) {
    if (curr_item != &DELETED_ITEM) {
      if (strcmp(curr_item->key, key) == 0) {
        del_item(curr_item);
        t->items[index] = &DELETED_ITEM;
      }
    }
    index = get_hash(key, t->size, i);
    curr_item = t->items[index];
    i++;
  }
  t->count--;
}

/* print_items
 *
 * Print out all the items present in the table
 * */
void print_items(table *t) {
  item *curr_item;
  for (int i = 0; i < t->size; i++) {
    curr_item = t->items[i];
    if (curr_item != NULL && curr_item != &DELETED_ITEM) {
      printf("%d -> %s: %s\n", i, curr_item->key, curr_item->value);
    }
  }
}
