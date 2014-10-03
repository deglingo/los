/* tprop.c -
 */

#include "tprop.h"
#include "tprop.inl"



/* Properties:
 */
enum
  {
    PROP_P1,
    PROP_COUNT,
  };

static LParamSpec *pspecs[PROP_COUNT] = { NULL, };




/* t_prop_class_init:
 */
static void t_prop_class_init ( LObjectClass *cls )
{
  /* props[PROP_P1] = */
  /*   l_object_class_install_property */
  /*   (cls, */
  /*    "p1", */
  /*    L_CLASS_INT); */
}
