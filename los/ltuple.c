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
  return t;
}
