/* ltestpackable.c -
 */

#include "libltest/ltestpackable.h"
#include "los/ltuple.h"
#include "los/lint.h"
#include "los/lstring.h"
#include "libltest/ltestpackable.inl"



void _finalize ( LObject *object );
LObject *_get_state ( LObject *object );
LObject *_from_state ( LObjectClass *cls,
                       LObject *state );



/* ltest_packable_class_init:
 */
static void ltest_packable_class_init ( LObjectClass *cls )
{
  cls->finalize = _finalize;
  cls->get_state = _get_state;
  cls->from_state = _from_state;
}



/* ltest_packable_new:
 */
LTestPackable *ltest_packable_new ( gint a,
                                    const gchar *b )
{
  LTestPackable *p;
  p = LTEST_PACKABLE(l_object_new(LTEST_CLASS_PACKABLE, NULL));
  p->a = a;
  p->b = g_strdup(b);
  return p;
}



/* _finalize:
 */
void _finalize ( LObject *object )
{
  g_free(LTEST_PACKABLE(object)->b);
  LTEST_PACKABLE(object)->b = NULL;
}



/* _get_state:
 */
LObject *_get_state ( LObject *object )
{
  LTestPackable *p = LTEST_PACKABLE(object);
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
      fprintf(stderr, "ERROR: invalid state\n");
      abort();
    }
  obj = l_object_new(cls, NULL);
  LTEST_PACKABLE(obj)->a = L_INT_VALUE(L_TUPLE_ITEM(state, 0));
  LTEST_PACKABLE(obj)->b = g_strdup(L_STRING(L_TUPLE_ITEM(state, 1))->str);
  return obj;
}
