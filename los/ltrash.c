/* ltrash.c -
 */

#include "los/private.h"
#include "los/ltrash.h"



/* l_trash_init:
 */
LTrash *l_trash_init ( LTrash *trash,
                       guint size )
{
  trash->size = size;
  trash->n_items = 0;
  trash->items = ((gpointer) trash) + sizeof(LTrash);
  return trash;
}



/* l_trash_cleanup:
 */
void l_trash_cleanup ( LTrash *trash )
{
  LTrashItem *item;
  for (item = &trash->items[trash->n_items-1]; item >= trash->items; item--)
    item->func(item->ptr);
}



/* l_trash_add:
 */
gpointer l_trash_add ( LTrash *trash,
                       gpointer ptr,
                       LTrashFunc func )
{
  LTrashItem *item;
  ASSERT(trash->n_items < trash->size);
  item = &trash->items[trash->n_items++];
  item->ptr = ptr;
  item->func = func;
  return ptr;
}
