/* ldict.c -
 */

#include "los/ldict.h"
#include "los/ldict.inl"



/* l_dict_new:
 */
LDict *l_dict_new ( void )
{
  return L_DICT(l_object_new(L_CLASS_DICT, NULL));
}
