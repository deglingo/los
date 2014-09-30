/* lnone.c -
 */

#include "los/lnone.h"
#include "los/lnone.inl"



/* [fixme] not mt-safe */
static LNone *l_none = NULL;



/* l_none_ref:
 */
LNone *l_none_ref ( void )
{
  if (!l_none)
    l_none = L_NONE(l_object_new(L_CLASS_NONE, NULL));
  return l_object_ref(l_none);
}
