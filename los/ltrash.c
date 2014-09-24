/* ltrash.c -
 */

#include "los/private.h"
#include "los/ltrash.h"

typedef struct _LTrash LTrash;
typedef struct _LTrashNode LTrashNode;



/* LTrash:
 */
struct _LTrash
{
  LTrash *next_trash;
  LTrashNode *first_node;
};



/* LTrashNode:
 */
struct _LTrashNode
{
  LTrashNode *next;
  gpointer item;
  LTrashFunc func;
};



/* globals */
static LTrash *current_trash = NULL;
static LTrash *free_trash = NULL;
static LTrashNode *free_node = NULL;



/* l_trash_alloc:
 */
static LTrash *l_trash_alloc ( void )
{
  LTrash *trash;
  if (G_LIKELY(free_trash))
    {
      trash = free_trash;
      free_trash = trash->next_trash;
    }
  else
    {
      trash = g_new(LTrash, 1);
    }
  return trash; 
}



/* l_trash_free:
 */
static void l_trash_free ( LTrash *trash )
{
  trash->next_trash = free_trash;
  free_trash = trash;
}



/* l_trash_node_alloc:
 */
static LTrashNode *l_trash_node_alloc ( void )
{
  LTrashNode *node;
  if (G_LIKELY(free_node))
    {
      node = free_node;
      free_node = node->next;
    }
  else
    {
      node = g_new(LTrashNode, 1);
    }
  return node;
}



/* l_trash_node_free:
 */
static void l_trash_node_free ( LTrashNode *node )
{
  node->next = free_node;
  free_node = node;
}



/* l_trash_push:
 */
void l_trash_push ( void )
{
  LTrash *t = l_trash_alloc();
  t->next_trash = current_trash;
  t->first_node = NULL;
  current_trash = t;
}



/* l_trash_pop:
 */
void l_trash_pop ( void )
{
  LTrash *trash = current_trash;
  LTrashNode *node;
  ASSERT(trash);
  while ((node = trash->first_node))
    {
      node->func(node->item);
      trash->first_node = node->next;
      l_trash_node_free(node);
    }
  current_trash = trash->next_trash;
  l_trash_free(trash);
}



/* l_trash_add:
 */
gpointer l_trash_add ( gpointer item,
                       LTrashFunc func )
{
  LTrashNode *node;
  ASSERT(current_trash);
  node = l_trash_node_alloc();
  node->next = current_trash->first_node;
  node->item = item;
  node->func = func;
  current_trash->first_node = node;
  return item;
}
