#include "hashtable.h"

#include <uthash.h>
#include <stdio.h>

#define KEY_LEN 64

//WRAPPER FOR UTHASH... unnecesseary?

typedef struct hash hash;  //hash used for uthash
struct hash {
  char key[KEY_LEN];
  void* entry;
  UT_hash_handle hh;
};

struct hashtable {
  hash *table;
};

hashtable* hashtable_create()  {  //TODO mem
  hashtable *ht = (hashtable*)malloc(sizeof(hashtable));
  ht->table = NULL;
  return ht;
}
int hashtable_destroy(hashtable* ht)  {
  free(ht);
  return 0;
}

int hashtable_insert(hashtable* ht, char* key, void* entry)  {
  hash *h;
  HASH_FIND_STR(ht->table, key, h);
  if (h == NULL)  {
    h = (hash*)malloc(sizeof(hash));
    strncpy(h->key, key, KEY_LEN);
    h->entry = entry;
    HASH_ADD_STR(ht->table, key, h);
    return 1;
  }
  else  {  
    fprintf(stderr, "hashtable_insert: key %s already in table\n", key);
    return 0;
  }
}

int hashtable_remove(hashtable *ht, char *key)  {
  hash *h;
  HASH_FIND_STR(ht->table, key, h);
  if (h == NULL)  {
    return -1;
  }
  HASH_DEL(ht->table, h);
  free(h);
  return 1;
}

void* hashtable_lookup(hashtable *ht, char *key)  {
  hash *h;
  HASH_FIND_STR(ht->table, key, h);
  if (h == NULL)  {
    return NULL;
  } 
  else  {
    return h->entry;
  }
}

int hashtable_size(hashtable *ht)  {
  //?
  return -1;
}
