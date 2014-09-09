/* lstring.h -
 */

#ifndef _LSTRING_H_
#define _LSTRING_H_

#include "los/lobject.h"
#include "los/lstring-def.h"



/* LString:
 */
struct _LString
{
  L_STRING_INSTANCE_HEADER;

  guint len;
  gchar *str;
};



/* LStringClass:
 */
struct _LStringClass
{
  L_STRING_CLASS_HEADER;
};



LString *l_string_new ( const gchar *str );



#endif
