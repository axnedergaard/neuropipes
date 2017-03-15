#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct linkedlist linkedlist;
struct linkedlist;

linkedlist *linkedlist_create();
int linkedlist_destroy(linkedlist*);
int linkedlist_insert(linkedlist*, void*);
int linkedlist_remove(linkedlist*, void*);
int linkedlist_clear(linkedlist*);
int linkedlist_size(linkedlist*);
void *linkedlist_head(linkedlist*);
void *linkedlist_pop(linkedlist*);
void *linkedlist_tail(linkedlist*);
void *linkedlist_iterate(linkedlist*);
int linkedlist_reset_iterater(linkedlist*);

#endif
