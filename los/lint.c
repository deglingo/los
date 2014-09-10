/* lint.c -
 */

#include "los/lint.h"
#include "los/lint.inl"



/* l_int_new:
 */
LInt *l_int_new ( gint value )
{
  LInt *i = L_INT(l_object_new(L_CLASS_INT, NULL));
  i->value = value;
  return i;
}
