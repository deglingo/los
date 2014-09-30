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
};



/* LListClass:
 */
struct _LListClass
{
  L_LIST_CLASS_HEADER;
};



LList *l_list_new ( void );



#endif
