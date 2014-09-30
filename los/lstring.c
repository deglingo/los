/* lstring.c -
 */

#include "los/private.h"
#include "los/lstring.h"
#include "los/lstring.inl"

#include <string.h>



static void _dispose ( LObject *object );
static gchar *_to_string ( LObject *object );
static guint _hash ( LObject *object );
static gboolean _eq ( LObject *a,
                      LObject *b );



/* l_string_class_init:
 */
static void l_string_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
  cls->to_string = _to_string;
  cls->hash = _hash;
  cls->eq = _eq;
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
  return g_strdup_printf("\"%s\"", L_STRING(object)->str);
}



/* _hash:
 */
static guint _hash ( LObject *object )
{
  /* copied from GLib */
  const gchar *p, *end;
  guint32 h = 5381;
  end = L_STRING(object)->str + L_STRING(object)->len;
  for (p = L_STRING(object)->str; p != end; p++)
    h = (h << 5) + h + *p;
  return h;
}



/* _eq:
 */
static gboolean _eq ( LObject *a,
                      LObject *b )
{
  return L_IS_STRING(b) && 
    (L_STRING(a)->len == L_STRING(b)->len) &&
    (!strncmp(L_STRING(a)->str, L_STRING(b)->str, L_STRING(a)->len));
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
