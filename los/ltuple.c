/* ltuple.c -
 */

#include "los/private.h"
#include "los/ltuple.h"
#include "los/ltuple.inl"



/* l_tuple_new:
 */
LTuple *l_tuple_new ( guint size )
{
  LTuple *t = L_TUPLE(l_object_new(L_CLASS_TUPLE, NULL));
  t->_size = size;
  t->_items = g_new0(LObject *, size);
  return t;
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
