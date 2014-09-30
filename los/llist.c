/* llist.c -
 */

#include "los/llist.h"
#include "los/llist.inl"



#define START_SIZE 32

static void _dispose ( LObject *object );



static inline void _grow ( LList *l,
                           gssize size )
{
  if (size > l->_buf_size) {
    while (size > l->_buf_size)
      l->_buf_size = (l->_buf_size ? (l->_buf_size * 2) : START_SIZE);
    l->_items = g_renew(LObject *, l->_items, l->_buf_size);
  }
}



/* l_list_class_init:
 */
static void l_list_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* l_list_new:
 */
LList *l_list_new ( void )
{
  LList *l = L_LIST(l_object_new(L_CLASS_LIST, NULL));
  return l;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  LList *l = L_LIST(object);
  gsize i;
  for (i = 0; i < l->_size; i++)
    L_OBJECT_CLEAR(l->_items[i]);
  g_free(l->_items);
  l->_items = NULL;
  l->_size = 0;
  l->_buf_size = 0;
  ((LObjectClass *) parent_class)->dispose(object);
}



/* l_list_append:
 */
void l_list_append ( LList *list,
                     LObject *item )
{
  _grow(list, list->_size + 1);
  list->_items[list->_size++] = l_object_ref(item);
}



/* l_list_get_item:
 */
LObject *l_list_get_item ( LList *list,
                           gsize index )
{
  if (index < list->_size)
    return list->_items[index];
  else
    return NULL;
}
