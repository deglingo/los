/* lnone.c -
 */

#include "los/lnone.h"
#include "los/lnone.inl"



/* [fixme] not mt-safe */
LNone *__l_none_singleton = NULL;



/* _l_none_init:
 */
void _l_none_init ( void )
{
  __l_none_singleton = L_NONE(l_object_new(L_CLASS_NONE, NULL));
}
