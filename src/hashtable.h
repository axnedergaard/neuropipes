#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct hashtable hashtable;
struct hashtable;

hashtable* hashtable_create();
int hashtable_destroy(hashtable*);
int hashtable_insert(hashtable*, char*, void*);  //key, entry
int hashtable_remove(hashtable*, char*);
void* hashtable_lookup(hashtable*, char*);
int hashtable_size(hashtable*);

#endif
