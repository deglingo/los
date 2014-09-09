/* lstring.c -
 */

#include "los/lstring.h"
#include "los/lstring.inl"

#include <string.h>



/* l_string_new:
 */
LString *l_string_new ( const gchar *str )
{
  LString *lstr = L_STRING(l_object_new(L_CLASS_STRING, NULL));
  lstr->len = strlen(str);
  lstr->str = strdup(str);
  return lstr;
}
