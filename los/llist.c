/* llist.c -
 */

#include "los/llist.h"
#include "los/llist.inl"



/* l_list_new:
 */
LList *l_list_new ( void )
{
  LList *l = L_LIST(l_object_new(L_CLASS_LIST, NULL));
  return l;
}



/* l_list_append:
 */
void l_list_append ( LList *list,
                     LObject *item )
{
}
