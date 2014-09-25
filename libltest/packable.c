/* packable.c -
 */

#include "libltest/packable.h"
#include "los/ltuple.h"
#include "los/lint.h"
#include "los/lstring.h"
#include "libltest/packable.inl"



void _finalize ( LObject *object );
LObject *_get_state ( LObject *object );
LObject *_from_state ( LObjectClass *cls,
                       LObject *state );



/* packable_class_init:
 */
static void packable_class_init ( LObjectClass *cls )
{
  cls->finalize = _finalize;
  cls->get_state = _get_state;
  cls->from_state = _from_state;
}



/* packable_new:
 */
Packable *packable_new ( gint a,
                         const gchar *b )
{
  Packable *p;
  p = PACKABLE(l_object_new(CLASS_PACKABLE, NULL));
  p->a = a;
  p->b = g_strdup(b);
  return p;
}



/* _finalize:
 */
void _finalize ( LObject *object )
{
  g_free(PACKABLE(object)->b);
  PACKABLE(object)->b = NULL;
}



/* _get_state:
 */
LObject *_get_state ( LObject *object )
{
  Packable *p = PACKABLE(object);
  LTuple *t = l_tuple_newl_give(2,
                                l_int_new(p->a),
                                l_string_new(p->b, -1),
                                NULL);
  return L_OBJECT(t);
}



/* _from_state:
 */
LObject *_from_state ( LObjectClass *cls,
                       LObject *state )
{
  LObject *obj;
  if (!(L_IS_TUPLE(state) &&
        L_TUPLE_SIZE(state) == 2 &&
        L_IS_INT(L_TUPLE_ITEM(state, 0)) &&
        L_IS_STRING(L_TUPLE_ITEM(state, 1))))
    {
      fprintf(stderr, "ERROR: invalid state: %s\n", l_object_to_string(state));
      abort();
    }
  obj = l_object_new(cls, NULL);
  PACKABLE(obj)->a = L_INT_VALUE(L_TUPLE_ITEM(state, 0));
  PACKABLE(obj)->b = g_strdup(L_STRING(L_TUPLE_ITEM(state, 1))->str);
  return obj;
}
