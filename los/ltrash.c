/* ltrash.c -
 */

#include "los/private.h"
#include "los/ltrash.h"
#include "los/lprivate.h"

typedef struct _LTrashPool LTrashPool;
typedef struct _LTrash LTrash;
typedef struct _LTrashNode LTrashNode;



/* LTrashPool:
 */
struct _LTrashPool
{
  LTrash *current_trash;
  LTrash *free_trash;
  LTrashNode *free_node;
};



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



/* l_trash_pool_get:
 */
static LTrashPool *l_trash_pool_get ( void )
{
  LPrivate *priv = l_private_get();
  if (G_UNLIKELY(!priv->trash_pool))
    {
      priv->trash_pool = g_new0(LTrashPool, 1);
    }
  return priv->trash_pool;
}



/* l_trash_pool_free:
 */
void l_trash_pool_free ( gpointer pool_ )
{
  LTrashPool *pool = pool_;
  LTrash *trash;
  LTrashNode *node;
  ASSERT(!pool->current_trash);
  CL_TRACE("%p", pool);
  while ((trash = pool->free_trash))
    {
      pool->free_trash = trash->next_trash;
      g_free(trash);
    }
  while ((node = pool->free_node))
    {
      pool->free_node = node->next;
      g_free(node);
    }
  g_free(pool);
}



/* l_trash_alloc:
 */
static LTrash *l_trash_alloc ( LTrashPool *pool )
{
  LTrash *trash;
  if (G_LIKELY(pool->free_trash))
    {
      trash = pool->free_trash;
      pool->free_trash = trash->next_trash;
    }
  else
    {
      trash = g_new(LTrash, 1);
    }
  return trash; 
}



/* l_trash_free:
 */
static void l_trash_free ( LTrashPool *pool,
                           LTrash *trash )
{
  trash->next_trash = pool->free_trash;
  pool->free_trash = trash;
}



/* l_trash_node_alloc:
 */
static LTrashNode *l_trash_node_alloc ( LTrashPool *pool )
{
  LTrashNode *node;
  if (G_LIKELY(pool->free_node))
    {
      node = pool->free_node;
      pool->free_node = node->next;
    }
  else
    {
      node = g_new(LTrashNode, 1);
    }
  return node;
}



/* l_trash_node_free:
 */
static void l_trash_node_free ( LTrashPool *pool,
                                LTrashNode *node )
{
  node->next = pool->free_node;
  pool->free_node = node;
}



/* l_trash_push:
 */
void l_trash_push ( void )
{
  LTrashPool *pool = l_trash_pool_get();
  LTrash *t = l_trash_alloc(pool);
  t->next_trash = pool->current_trash;
  t->first_node = NULL;
  pool->current_trash = t;
}



/* l_trash_pop:
 */
void l_trash_pop ( void )
{
  LTrashPool *pool = l_trash_pool_get();
  LTrash *trash = pool->current_trash;
  LTrashNode *node;
  /* [FIXME] can be made more efficient by freeing the whole node chain at once */
  ASSERT(trash);
  while ((node = trash->first_node))
    {
      node->func(node->item);
      trash->first_node = node->next;
      l_trash_node_free(pool, node);
    }
  pool->current_trash = trash->next_trash;
  l_trash_free(pool, trash);
}



/* l_trash_add:
 */
gpointer l_trash_add ( gpointer item,
                       LTrashFunc func )
{
  LTrashPool *pool = l_trash_pool_get();
  LTrashNode *node;
  ASSERT(pool->current_trash);
  node = l_trash_node_alloc(pool);
  node->next = pool->current_trash->first_node;
  node->item = item;
  node->func = func;
  pool->current_trash->first_node = node;
  return item;
}
