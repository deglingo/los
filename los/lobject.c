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
  static LObjectClass *cls=NULL;
  if (!cls) {
    cls = g_malloc0(sizeof(LObjectClass));
    cls->l_class_info.class_size = sizeof(LObjectClass);
    cls->l_class_info.instance_size = sizeof(LObject);
  }
  return cls;
}



/* l_object_class_register:
 */
LObjectClass *l_object_class_register ( const gchar *name,
                                        LObjectClass *parent,
                                        LClassInfo *info )
{
  LObjectClass *cls;
  cls = g_malloc0(info->class_size);
  cls->l_class_info = *info;
  return cls;
}



/* l_object_new:
 */
LObject *l_object_new ( LObjectClass *cls,
                        const char *first_prop,
                        ... )
{
  LObject *obj;
  obj = g_malloc0(cls->l_class_info.instance_size);
  obj->l_class = l_object_ref(cls);
  return obj;
}
