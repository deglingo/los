/* lobject.c -
 */

#include "los/private.h"
#include "los/lobject.h"

/* [REMOVEME] */
#include <stdlib.h>
#include <string.h>



static GHashTable *_class_names = NULL;

static void _class_init ( LObjectClass *cls );
static void _dispose ( LObject *object );
static gchar *_to_string ( LObject *object );



/* l_object_get_class:
 */
LObjectClass *l_object_get_class ( void )
{
  static LObjectClass *cls=NULL;
  if (!cls) {
    LClassInfo info = { 0, };
    /* [FIXME] should be in some global init func */
    _class_names = g_hash_table_new(g_str_hash, g_str_equal);
    /* class init */
    info.class_size = sizeof(LObjectClass);
    info.class_init = _class_init;
    info.instance_size = sizeof(LObject);
    cls = l_object_class_register("LObject", NULL, &info);
  }
  return cls;
}



/* _class_init:
 */
static void _class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
  cls->to_string = _to_string;
}



/* l_object_class_register:
 */
LObjectClass *l_object_class_register ( const gchar *name,
                                        LObjectClass *parent,
                                        LClassInfo *info )
{
  LObjectClass *cls;
  if (l_object_class_from_name(name))
    CL_ERROR("class '%s' is already registered", name);
  cls = g_malloc0(info->class_size);
  /* raw-copy the base class */
  /* [FIXME] should only the relevant part and avoid the header */
  if (parent)
    memcpy(cls, parent, parent->l_class_info.class_size);
  /* copy info */
  cls->l_class_info = *info;
  /* [fixme] */
  cls->name = g_strdup(name);
  cls->l_parent_class = parent;
  /* class init handler */
  if (info->class_init)
    info->class_init(cls);
  /* register name */
  g_hash_table_insert(_class_names, cls->name, cls);
  return cls;
}



/* l_object_isclass:
 */
gboolean l_object_isclass ( LObject *obj )
{
  return obj->l_class ? FALSE : TRUE;
}



/* l_object_issubclass:
 */
gboolean l_object_issubclass ( LObject *cls1,
                               LObject *cls2 )
{
  ASSERT(l_object_isclass(cls1));
  ASSERT(l_object_isclass(cls2));
  while (cls1) {
    if (cls1 == cls2)
      return TRUE;
    cls1 = (LObject *) (((LObjectClass *) cls1)->l_parent_class);
  }
  return FALSE;
}



/* l_object_class_name:
 */
const gchar *l_object_class_name ( LObjectClass *cls )
{
  return cls->name;
}



/* l_object_class_from_name:
 */
LObjectClass *l_object_class_from_name ( const gchar *name )
{
  return g_hash_table_lookup(_class_names, name);
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
  /* [FIXME] use object allocator */
  obj = g_malloc0(cls->l_class_info.instance_size);
  obj->l_class = l_object_ref(cls);
  obj->ref_count = 1;
  /* instance init */
  _instance_init(obj, cls);
  return obj;
}



/* l_object_new_from_state:
 */
LObject *l_object_new_from_state ( LObjectClass *cls,
                                   LObject *state )
{
  ASSERT(cls->from_state);
  return cls->from_state(cls, state);
}



/* l_object_ref:
 */
gpointer l_object_ref ( gpointer obj )
{
  g_atomic_int_inc(&((LObject *)obj)->ref_count);
#ifdef L_DEBUG
  if (L_OBJECT(obj)->trace_ref) {
    /* [fixme] not mt-safe */
    CL_DEBUG("REF OBJECT: %s -> %d",
             l_object_to_string(obj),
             L_OBJECT(obj)->ref_count);
    G_BREAKPOINT();
  }
#endif
  return obj;
}



/* l_object_unref:
 */
void l_object_unref ( gpointer obj )
{
  if (g_atomic_int_dec_and_test(&((LObject *)obj)->ref_count))
    {
#ifdef L_DEBUG
      if (L_OBJECT(obj)->trace_ref) {
        /* [fixme] not mt-safe */
        CL_DEBUG("DESTROY OBJECT: %s -> %d",
                 l_object_to_string(obj),
                 L_OBJECT(obj)->ref_count);
        G_BREAKPOINT();
      }
#endif
      l_object_dispose(obj);
      if (L_OBJECT_GET_CLASS(obj)->finalize)
        L_OBJECT_GET_CLASS(obj)->finalize(obj);
      /* [FIXME] use object allocator */
      g_free(obj);
    }
#ifdef L_DEBUG
  else
    {
      if (L_OBJECT(obj)->trace_ref) {
        /* [fixme] not mt-safe */
        CL_DEBUG("UNREF OBJECT: %s -> %d",
                 l_object_to_string(obj),
                 L_OBJECT(obj)->ref_count);
        G_BREAKPOINT();
      }
    }
#endif
}



/* l_object_dispose:
 */
void l_object_dispose ( LObject *object )
{
  L_OBJECT_GET_CLASS(object)->dispose(object);
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
}



/* l_object_get_state:
 */
LObject *l_object_get_state ( LObject *object )
{
  ASSERT(L_OBJECT_GET_CLASS(object)->get_state);
  return L_OBJECT_GET_CLASS(object)->get_state(object);
}



/* l_object_to_string:
 */
gchar *l_object_to_string ( LObject *object )
{
  if (object)
    return L_OBJECT_GET_CLASS(object)->to_string(object);
  else
    return g_strdup("<NULL>");
}



/* _to_string:
 */
static gchar *_to_string ( LObject *object )
{
  return g_strdup_printf("<%s %p>",
                         l_object_class_name(L_OBJECT_GET_CLASS(object)),
                         object);
}



/* l_object_hash:
 */
guint l_object_hash ( LObject *object )
{
  ASSERT(L_OBJECT_GET_CLASS(object)->hash);
  return L_OBJECT_GET_CLASS(object)->hash(object);
}



/* l_object_eq:
 */
gboolean l_object_eq ( LObject *a,
                       LObject *b )
{
  ASSERT(L_OBJECT_GET_CLASS(a)->eq);
  return L_OBJECT_GET_CLASS(a)->eq(a, b);
}



/* l_object_set_trace_ref:
 */
#ifdef L_DEBUG
void l_object_set_trace_ref ( LObject *object,
                              gboolean enable )
{
  object->trace_ref = (enable ? 1 : 0);
}
#endif
