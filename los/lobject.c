/* lobject.c -
 */

#include "los/lobject.h"

/* [REMOVEME] */
#include <stdlib.h>
#include <string.h>



/* l_object_get_class:
 */
LObjectClass *l_object_get_class ( void )
{
  return NULL;
}



/* l_object_class_register:
 */
LObjectClass *l_object_class_register ( const gchar *name,
                                        LObjectClass *parent,
                                        LClassInfo *info )
{
  LObjectClass *cls;
  cls = g_malloc0(info->class_size);
  return cls;
}



/* l_object_get_type:
 */
LType l_object_get_type ( void )
{
  static LType type = 0;
  if (type == 0) {
    LTypeInfo info = { 0, };
    info.class_size = sizeof(LObjectClass);
    info.instance_size = sizeof(LObject);
    type = l_type_register("LObject", 0, &info);
  }
  return type;
}



/* l_object_new:
 */
LObject *l_object_new ( LType type,
                        const char *first_prop,
                        ... )
{
  LObject *obj;
  obj = l_type_instantiate(type);
  return obj;
}
