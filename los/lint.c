/* lint.c -
 */

#include "los/lint.h"
#include "los/lint.inl"



static gchar *_to_string ( LObject *object );
static guint _hash ( LObject *object );
static gboolean _eq ( LObject *a,
                      LObject *b );



/* l_int_class_init:
 */
static void l_int_class_init ( LObjectClass *cls )
{
  cls->to_string = _to_string;
  cls->hash = _hash;
  cls->eq = _eq;
}



/* l_int_new:
 */
LInt *l_int_new ( gint value )
{
  LInt *i = L_INT(l_object_new(L_CLASS_INT, NULL));
  i->value = value;
  return i;
}



/* _to_string:
 */
static gchar *_to_string ( LObject *object )
{
  return g_strdup_printf("<%s %d>",
                         l_object_class_name(L_OBJECT_GET_CLASS(object)),
                         L_INT_VALUE(object));
}



/* _hash:
 */
static guint _hash ( LObject *object )
{
  return (guint) (L_INT(object)->value);
}



/* _eq:
 */
static gboolean _eq ( LObject *a,
                      LObject *b )
{
  return L_IS_INT(b) && (L_INT_VALUE(a) == L_INT_VALUE(b));
}
