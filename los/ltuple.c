/* ltuple.c -
 */

#include "los/private.h"
#include "los/ltuple.h"
#include "los/ltuple.inl"



static void _dispose ( LObject *object );



/* l_tuple_class_init:
 */
static void l_tuple_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* l_tuple_new:
 */
LTuple *l_tuple_new ( guint size )
{
  LTuple *t = L_TUPLE(l_object_new(L_CLASS_TUPLE, NULL));
  t->_size = size;
  t->_items = g_new0(LObject *, size);
  return t;
}



/* l_tuple_newl_give:
 */
LTuple *l_tuple_newl_give ( guint size,
                            ... )
{
  LTuple *t;
  va_list args;
  va_start(args, size);
  t = l_tuple_newv_give(size, args);
  va_end(args);
  return t;
}



/* l_tuple_newv_give:
 */
LTuple *l_tuple_newv_give ( guint size,
                            va_list args )
{
  LTuple *t = l_tuple_new(size);
  guint i;
  LObject *item;
  for (i = 0; i < size; i++) {
    item = va_arg(args, LObject *);
    ASSERT(item);
    t->_items[i] = item;
  }
  return t;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  if (L_TUPLE(object)->_items)
    {
      guint i;
      for (i = 0; i < L_TUPLE_SIZE(object); i++)
        L_OBJECT_CLEAR(L_TUPLE(object)->_items[i]);
      g_free(L_TUPLE(object)->_items);
      L_TUPLE(object)->_items = NULL;
      L_TUPLE(object)->_size = 0;
    }
  /* [FIXME] */
  ((LObjectClass *) parent_class)->dispose(object);
}



/* l_tuple_give_item:
 */
void l_tuple_give_item ( LTuple *tuple,
                         guint index,
                         LObject *item )
{
  ASSERT(index < tuple->_size);
  ASSERT(tuple->_items[index] == NULL);
  tuple->_items[index] = item;
}
