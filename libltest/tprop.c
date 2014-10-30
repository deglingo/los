/* tprop.c -
 */

#include "los/private.h"
#include "tprop.h"
#include "los/lint.h"
#include "tprop.inl"



/* Properties:
 */
enum
  {
    PROP_0,
    PROP_P1,
    PROP_COUNT,
  };

static LParamSpec *pspecs[PROP_COUNT] = { NULL, };



static void _dispose ( LObject *object );
static LObject *_get_property ( LObject *object,
                                LParamSpec *pspec );
static void _set_property ( LObject *object,
                            LParamSpec *pspec,
                            LObject *value );




/* t_prop_init:
 */
static void t_prop_init ( LObject *obj )
{
  /* [fixme] pspec default */
  T_PROP(obj)->p1 = L_OBJECT(l_int_new(0));
}



/* t_prop_class_init:
 */
static void t_prop_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
  cls->get_property = _get_property;
  cls->set_property = _set_property;

  pspecs[PROP_P1] = l_param_spec_int("p1",
                                     2);

  l_object_class_install_properties(cls, PROP_COUNT, pspecs);
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  L_OBJECT_CLEAR(T_PROP(object)->p1);
  L_OBJECT_CLASS(parent_class)->dispose(object);
}



/* _get_property:
 */
static LObject *_get_property ( LObject *object,
                                LParamSpec *pspec )
{
  switch (pspec->param_id)
    {
    case PROP_P1:
      return l_object_ref(T_PROP(object)->p1);
    default:
      ASSERT(0);
      return NULL;
    }
}



/* _set_property:
 */
static void _set_property ( LObject *object,
                            LParamSpec *pspec,
                            LObject *value )
{
  switch (pspec->param_id)
    {
    case PROP_P1:
      L_OBJECT_CLEAR(T_PROP(object)->p1);
      T_PROP(object)->p1 = l_object_ref(value);
      l_object_notify(object, pspec);
      break;
    default:
      ASSERT(0);
      break;
    }
}
