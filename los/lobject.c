/* lobject.c -
 */

#include "los/private.h"
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
  /* raw-copy the base class */
  /* [FIXME] should only the relevant part and avoid the header */
  memcpy(cls, parent, parent->l_class_info.class_size);
  /* copy info */
  cls->l_class_info = *info;
  /* [fixme] */
  cls->l_parent_class = parent;
  /* class init handler */
  if (info->class_init)
    info->class_init(cls);
  return cls;
}



/* _instance_init:
 *
 * [fixme] use some mro list to avoid these recursive calls
 */
static void _instance_init ( LObject *obj,
                             LObjectClass *cls )
{
  if (cls->l_parent_class)
    _instance_init(obj, cls->l_parent_class);
  if (cls->l_class_info.init)
    cls->l_class_info.init(obj);
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
  obj->ref_count = 1;
  /* instance init */
  _instance_init(obj, cls);
  return obj;
}



/* l_object_ref:
 */
gpointer l_object_ref ( gpointer obj )
{
  g_atomic_int_inc(&((LObject *)obj)->ref_count);
  return obj;
}



/* l_object_unref:
 */
void l_object_unref ( gpointer obj )
{
  if (g_atomic_int_dec_and_test(&((LObject *)obj)->ref_count))
    {
      CL_DEBUG("[TODO] destroy object %p", obj);
    }
}
