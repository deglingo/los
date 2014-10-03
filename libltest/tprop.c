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



static LObject *_get_property ( LObject *object,
                                LParamSpec *pspec );




/* t_prop_class_init:
 */
static void t_prop_class_init ( LObjectClass *cls )
{
  cls->get_property = _get_property;

  pspecs[PROP_P1] = l_param_spec_int("p1",
                                     2);

  l_object_class_install_properties(cls, PROP_COUNT, pspecs);
}



/* _get_property:
 */
static LObject *_get_property ( LObject *object,
                                LParamSpec *pspec )
{
  switch (pspec->param_id)
    {
    case PROP_P1:
      return L_OBJECT(l_int_new(0));
    default:
      ASSERT(0);
      return NULL;
    }
}
