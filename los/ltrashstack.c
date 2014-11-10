/* ltrashstack.c -
 */

#include "los/private.h"
#include "los/ltrashstack.h"



/* LTrashItem:
 */
typedef struct _LTrashItem LTrashItem;
struct _LTrashItem
{
  LTrashItem *next;
};



/* LTrashStack:
 */
struct _LTrashStack
{
  gsize item_size;
  gssize max_height;
  gsize height;
  GMutex lock;
  LTrashItem *free_item;
};



/* l_trash_stack_new:
 */
LTrashStack *l_trash_stack_new ( gsize item_size,
                                 gssize max_height )
{
  LTrashStack *stack;
  ASSERT(item_size >= sizeof(LTrashItem));
  stack = g_slice_alloc0(sizeof(LTrashStack));
  stack->item_size = item_size;
  stack->max_height = max_height;
  g_mutex_init(&stack->lock);
  return stack;
}



/* l_trash_stack_destroy:
 */
void l_trash_stack_destroy ( LTrashStack *stack )
{
  /* [fixme] lock ? */
  while (stack->free_item)
    {
      LTrashItem *item = stack->free_item;
      stack->free_item = item->next;
      g_slice_free1(stack->item_size, item);
    }
  g_mutex_clear(&stack->lock);
  g_slice_free1(sizeof(LTrashStack), stack);
}



/* l_trash_stack_alloc:
 */
gpointer l_trash_stack_alloc ( LTrashStack *stack )
{
  LTrashItem *item;
  g_mutex_lock(&stack->lock);
  if (stack->free_item)
    {
      item = stack->free_item;
      stack->free_item = item->next;
      g_mutex_unlock(&stack->lock);
    }
  else
    {
      g_mutex_unlock(&stack->lock);
      item = g_slice_alloc(stack->item_size);
    }
  return item;
}



/* l_trash_stack_free:
 */
void l_trash_stack_free ( LTrashStack *stack,
                               gpointer item )
{
  g_mutex_lock(&stack->lock);
  if (stack->max_height < 0 || stack->height < stack->max_height)
    {
      stack->height++;
      ((LTrashItem *) item)->next = stack->free_item;
      stack->free_item = item;
      g_mutex_unlock(&stack->lock);
    }
  else
    {
      g_mutex_unlock(&stack->lock);
      g_slice_free1(stack->item_size, item);
    }
}



/* l_trash_stack_alloc0:
 */
gpointer l_trash_stack_alloc0 ( LTrashStack *stack )
{
  gpointer item = l_trash_stack_alloc(stack);
  memset(item, 0, stack->item_size);
  return item;
}
