/* tprop.c -
 */

#include "tprop.h"
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




/* t_prop_class_init:
 */
static void t_prop_class_init ( LObjectClass *cls )
{
  pspecs[PROP_P1] = l_param_spec_int("p1",
                                     2);

  l_object_class_install_properties(cls, PROP_COUNT, pspecs);
}
