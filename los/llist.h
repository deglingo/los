/* llist.h -
 */

#ifndef _LLIST_H_
#define _LLIST_H_

#include "los/lobject.h"
#include "los/llist-def.h"



/* LList:
 */
struct _LList
{
  L_LIST_INSTANCE_HEADER;

  LObject **_items;
  gssize _size;
  gssize _buf_size;
};



/* LListClass:
 */
struct _LListClass
{
  L_LIST_CLASS_HEADER;
};



LList *l_list_new ( void );
void l_list_append ( LList *list,
                     LObject *item );
LObject *l_list_get_item ( LList *list,
                           gsize index );



#endif
