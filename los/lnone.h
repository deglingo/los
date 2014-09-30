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



LNone *l_none_ref ( void );



#endif
