#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct linkedlist_node linkedlist_node;
struct linkedlist_node  {
  void* data;
  linkedlist_node* next;
};

struct linkedlist  {
  linkedlist_node *head;
  linkedlist_node *tail;
  int size;
  linkedlist_node *iterater;
  int iterater_init;
};

linkedlist *linkedlist_create()  {
  linkedlist* l = (linkedlist*)malloc(sizeof(linkedlist));
  if (l == NULL)  {
    printf("linkedlist_create: failed to alloc mem\n");
    return NULL;
  }
  l->size = 0;
  l->iterater_init = 0;
  l->iterater = NULL;
  l->head = NULL;
  l->tail = NULL;
  return l;
}

int linkedlist_destroy(linkedlist* l)  {  //TODO destroy nodes?
  free(l->iterater);
  free(l);
  return 1;
}

int linkedlist_clear(linkedlist *l)  {
  linkedlist_node *cur = l->head;
  while (l->size > 0)  {
    linkedlist_node *next = cur->next;
    free(cur);
    cur = next;
    l->size--;
  }
  l->head = NULL;
  l->tail = NULL;
  return 1;
}

int linkedlist_insert(linkedlist *l, void *d)  {
  linkedlist_node* prev = NULL;
  linkedlist_node* cur = l->head;
  while (cur != NULL)  {
    prev = cur;
    cur = cur->next;
  }
  linkedlist_node* inserted = (linkedlist_node*)malloc(sizeof(linkedlist_node));
  if (inserted == NULL)  {
    printf("linkedlist_insert: failed to alloc mem\n");
    return 0;
  }
  inserted->data = d; 
  inserted->next = NULL;
  if (prev != NULL)  {
    prev->next = inserted;
  }
  else  {
    l->head = inserted;
  }
  l->tail = inserted;
  l->size++;
  return 1;
}

int linkedlist_remove(linkedlist* l, void* d)  {
  linkedlist_node* prev = NULL;
  linkedlist_node* cur = l->head;
  while ((cur != NULL) && (cur->data != d))   {
    prev = cur;
    cur = cur->next;
  }
  if (cur == NULL)  {  //not found
    return -1;
  }
  if (prev != NULL)  {  //update predecessor if exists
     prev->next = cur->next;
  }
  if (l->tail == cur)  {  //update tail if removed node was tail
    l->tail = prev;
  }
  free(cur); //deallocate memory 
  l->size--;
  return 1; //success
}

int linkedlist_size(linkedlist* l)  {
  return l->size;
}

void *linkedlist_head(linkedlist *l)  {
  return l->head->data;
}

void *linkedlist_pop(linkedlist *l)  {
  linkedlist_node *head = l->head;
  void *data = head->data;
  l->head = head->next;
  free(head);
  l->size--;
  return data;
}

void *linkedlist_tail(linkedlist *l)  {
  return l->tail->data;
}

void *linkedlist_iterate(linkedlist *l)  {
  if (l->iterater_init == 0)  {
    l->iterater_init = 1;
    l->iterater = l->head;
  }
  if (l->iterater != NULL)  {
    void* tmp = l->iterater->data;
    l->iterater = l->iterater->next;
    return tmp;
  }
  else  {
    l->iterater_init = 0;
  }
  return NULL;
}

int linkedlist_reset_iterater(linkedlist* l)  {  //reset iterater, return 1 if iterater was already init else 0
  int init = l->iterater_init; 
  l->iterater_init = 0;
  return init;
}

