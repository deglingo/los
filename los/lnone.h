/* lnone.h -
 */

#ifndef _LNONE_H_
#define _LNONE_H_

#include "los/lobject.h"
#include "los/lnone-def.h"



/* LNone:
 */
struct _LNone
{
  L_NONE_INSTANCE_HEADER;
};



/* LNoneClass:
 */
struct _LNoneClass
{
  L_NONE_CLASS_HEADER;
};



/* private */
extern LNone *__l_none_singleton;

#define l_none_get() (__l_none_singleton)
#define l_none_ref() (l_object_ref(l_none_get()))

void _l_none_init ( void );



#endif
