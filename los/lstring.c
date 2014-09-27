/* lstring.c -
 */

#include "los/private.h"
#include "los/lstring.h"
#include "los/lstring.inl"

#include <string.h>



static void _dispose ( LObject *object );
static gchar *_to_string ( LObject *object );



/* l_string_class_init:
 */
static void l_string_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
  cls->to_string = _to_string;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  g_free(L_STRING(object)->str);
  L_STRING(object)->str = NULL;
  L_STRING(object)->len = 0;
  /* [fixme] */
  ((LObjectClass *) parent_class)->dispose(object);
}



/* _to_string:
 */
static gchar *_to_string ( LObject *object )
{
  /* [FIXME] handle embedded nul bytes */
  return g_strdup_printf("<%s \"%s\">",
                         l_object_class_name(L_OBJECT_GET_CLASS(object)),
                         L_STRING(object)->str);
}



/* l_string_new:
 */
LString *l_string_new ( const gchar *str )
{
  return l_string_new_len(str, -1);
}



/* l_string_new_len:
 */
LString *l_string_new_len ( const gchar *str,
                            gssize len )
{
  LString *lstr = L_STRING(l_object_new(L_CLASS_STRING, NULL));
  if (len < 0)
    lstr->len = strlen(str);
  else
    lstr->len = len;
  lstr->str = g_malloc(lstr->len+1);
  memcpy(lstr->str, str, lstr->len);
  lstr->str[lstr->len] = 0;
  return lstr;
}
