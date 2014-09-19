/* lstring.c -
 */

#include "los/lstring.h"
#include "los/lstring.inl"

#include <string.h>



static void _dispose ( LObject *object );



/* l_string_class_init:
 */
static void l_string_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  g_free(L_STRING(object)->str);
  L_STRING(object)->str = NULL;
  L_STRING(object)->len = 0;
}



/* l_string_new:
 */
LString *l_string_new ( const gchar *str )
{
  LString *lstr = L_STRING(l_object_new(L_CLASS_STRING, NULL));
  lstr->len = strlen(str);
  lstr->str = g_strdup(str);
  return lstr;
}
