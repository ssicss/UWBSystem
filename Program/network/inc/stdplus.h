#ifndef __STD_PLUS_H
#define __STD_PLUS_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>


/**********************common***********************/

#undef USE_ASSERT


#ifdef USE_ASSERT
	#define SP_ASSERT(x) assert(x)
#else
	#define SP_ASSERT(x)  
#endif






/*********************queue**************************/ 
struct QUEUE_INFO_INDENTIFICATION{
    
	size_t size;		
	size_t front;	  
	size_t rear;   
};

struct QUEUE  
{  
	struct QUEUE_INFO_INDENTIFICATION indentification;
    size_t *pbuf; 
};  


struct QUEUE *QueueCreate(const size_t size);
bool QueuePush(struct QUEUE *q, const size_t it);
bool QueuePop(struct QUEUE *q, size_t *item);
bool QueueIsFull(const struct QUEUE *q);
bool QueueIsEmpty(const struct QUEUE *q);
#endif


/*********************linklist**************************/ 






/* listnodes must always contain data to be valid. Adding an empty node
 * to a list is invalid
 */
struct listnode 
{
  struct listnode *next;
  struct listnode *prev;
  
  /* private member, use getdata() to retrieve, do not access directly */
  void *data;
};
 
struct list 
{
  struct listnode *head;
  struct listnode *tail;
 
  /* invariant: count is the number of listnodes in the list */
  unsigned int count;
 
  /*
   * Returns -1 if val1 < val2, 0 if equal?, 1 if val1 > val2.
   * Used as definition of sorted for listnode_add_sort
   */
  int (*cmp) (void *val1, void *val2);
 
  /* callback to free user-owned data when listnode is deleted. supplying
   * this callback is very much encouraged!
   */
  void (*del) (void *val);
};
 
#define listnextnode(X) ((X) ? ((X)->next) : NULL)
#define listhead(X) ((X) ? ((X)->head) : NULL)
#define listtail(X) ((X) ? ((X)->tail) : NULL)
#define listcount(X) ((X)->count)
#define list_isempty(X) ((X)->head == NULL && (X)->tail == NULL)
#define listgetdata(X) ( (X)->data)
 
/* Prototypes. */
extern struct list *list_new(void); /* encouraged: set list.del callback on new lists */
extern void list_free (struct list *);
 
extern void listnode_add (struct list *, void *);
extern void listnode_add_sort (struct list *, void *);
extern void listnode_add_after (struct list *, struct listnode *, void *);
extern void listnode_delete (struct list *, void *);
extern struct listnode *listnode_lookup (struct list *, void *);
extern void *listnode_head (struct list *);
 
extern void list_delete (struct list *);
extern void list_delete_all_node (struct list *);
 
extern void list_delete_node (struct list *, struct listnode *);
extern void list_add_node_prev (struct list *, struct listnode *, void *);
extern void list_add_node_next (struct list *, struct listnode *, void *);
extern void list_add_list (struct list *, struct list *);
 
/* List iteration macro. 
 * Usage: for (ALL_LIST_ELEMENTS (...) { ... }
 * It is safe to delete the listnode using this macro.
 */
#define ALL_LIST_ELEMENTS(list,node,nextnode,data) \
  (node) = listhead(list), ((data) = NULL); \
  (node) != NULL && \
    ((data) = listgetdata(node),(nextnode) = node->next, 1); \
  (node) = (nextnode), ((data) = NULL)
 
/* read-only list iteration macro.
 * Usage: as per ALL_LIST_ELEMENTS, but not safe to delete the listnode Only
 * use this macro when it is *immediately obvious* the listnode is not
 * deleted in the body of the loop. Does not have forward-reference overhead
 * of previous macro.
 */
#define ALL_LIST_ELEMENTS_RO(list,node,data) \
  (node) = listhead(list), ((data) = NULL);\
  (node) != NULL && ((data) = listgetdata(node), 1); \
  (node) = listnextnode(node), ((data) = NULL)
 
/* these *do not* cleanup list nodes and referenced data, as the functions
 * do - these macros simply {de,at}tach a listnode from/to a list.
 */
 
/* List node attach macro.  */
#define LISTNODE_ATTACH(L,N) \
  do { \
    (N)->prev = (L)->tail; \
    if ((L)->head == NULL) \
      (L)->head = (N); \
    else \
      (L)->tail->next = (N); \
    (L)->tail = (N); \
    (L)->count++; \
  } while (0)
 
/* List node detach macro.  */
#define LISTNODE_DETACH(L,N) \
  do { \
    if ((N)->prev) \
      (N)->prev->next = (N)->next; \
    else \
      (L)->head = (N)->next; \
    if ((N)->next) \
      (N)->next->prev = (N)->prev; \
    else \
      (L)->tail = (N)->prev; \
    (L)->count--; \
  } while (0)
 


