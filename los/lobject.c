/* lobject.c -
 */

#include "los/private.h"
#include "los/lobject.h"
#include "los/lsignal.h"
#include "los/lparamspecpool.h"

/* [REMOVEME] */
#include <stdlib.h>
#include <string.h>

/* [FIXME] */
#define MAX_PARAMS 64



static GHashTable *class_names = NULL;
static LParamSpecPool *pspec_pool = NULL;

static void _class_init ( LObjectClass *cls );
static LObject *_constructor ( LObjectClass *cls,
                               LObjectConstructParam *params,
                               gint n_params );
static void _dispose ( LObject *object );
static gchar *_to_string ( LObject *object );



/* Signals:
 */
enum
  {
    SIG_NOTIFY,
    SIG_COUNT,
  };

static LSignalID signals[SIG_COUNT] = { 0, };



/* _l_object_init:
 *
 * Global initialization (MT safe).
 */
void _l_object_init ( void )
{
  ASSERT(!pspec_pool);
  class_names = g_hash_table_new(g_str_hash, g_str_equal);
  pspec_pool = l_param_spec_pool_new();
}



/* l_object_get_class:
 */
LObjectClass *l_object_get_class ( void )
{
  static LObjectClass *cls=NULL;
  if (!cls) {
    LClassInfo info = { 0, };
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
  cls->constructor = _constructor;
  cls->dispose = _dispose;
  cls->to_string = _to_string;

  signals[SIG_NOTIFY] = l_signal_new(cls,
                                     "notify",
                                     0,
                                     0,
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL);
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
  g_hash_table_insert(class_names, cls->name, cls);
  return cls;
}



/* l_object_class_install_property:
 */
void l_object_class_install_property ( LObjectClass *cls,
                                       guint param_id,
                                       LParamSpec *pspec )
{
  ASSERT(pspec_pool);
  l_param_spec_pool_register
    (pspec_pool, cls, param_id, pspec);
}



/* l_object_class_install_property:
 */
void l_object_class_install_properties ( LObjectClass *cls,
                                         guint count,
                                         LParamSpec **pspecs )
{
  guint p;
  ASSERT(count >= 1);
  ASSERT(!pspecs[0]);
  for (p = 1; p < count; p++)
    l_object_class_install_property(cls, p, pspecs[p]);
}



/* l_object_class_lookup_property:
 */
LParamSpec *l_object_class_lookup_property ( LObjectClass *cls,
                                             const gchar *name )
{
  ASSERT(pspec_pool);
  return l_param_spec_pool_lookup(pspec_pool, cls, name);
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
  return g_hash_table_lookup(class_names, name);
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



/* _l_object_new:
 */
static LObject *_l_object_new ( LObjectClass *cls,
                                gboolean give,
                                va_list args )
{
  LObject *obj;
  LObjectConstructParam params[MAX_PARAMS];
  LObjectConstructParam *param;
  gint n_params = 0;
  const gchar *prop_name;
  /* fill params */
  for (prop_name = va_arg(args, const gchar *), param = params;
       prop_name;
       prop_name = va_arg(args, const gchar *), param++, n_params++)
    {
      ASSERT(n_params < MAX_PARAMS);
      param->value = va_arg(args, LObject *);
      param->pspec = l_param_spec_pool_lookup(pspec_pool,
                                              cls,
                                              prop_name);
      ASSERT(param->pspec);
    }
  obj = cls->constructor(cls, params, n_params);
  if (give)
    {
      gint n;
      for (n = 0; n < n_params; n++)
        l_object_unref(params[n].value);
    }
  return obj;
}



/* l_object_new:
 */
LObject *l_object_new ( LObjectClass *cls,
                        ... )
{
  LObject *obj;
  va_list args;
  va_start(args, cls);
  obj = _l_object_new(cls, FALSE, args);
  va_end(args);
  return obj;
}



/* l_object_new_give:
 *
 * [FIXME] not tested
 *
 */
LObject *l_object_new_give ( LObjectClass *cls,
                             ... )
{
  LObject *obj;
  va_list args;
  va_start(args, cls);
  obj = _l_object_new(cls, TRUE, args);
  va_end(args);
  return obj;
}



/* constructor:
 */
static LObject *_constructor ( LObjectClass *cls,
                               LObjectConstructParam *params,
                               gint n_params )
{
  gint n;
  LObjectConstructParam *p;
  LObject *obj;
  /* [FIXME] use object allocator */
  obj = g_malloc0(cls->l_class_info.instance_size);
  obj->l_class = l_object_ref(cls);
  obj->ref_count = 1;
  /* instance init */
  _instance_init(obj, cls);
  /* set properties */
  for (n = 0, p = params; n < n_params; n++, p++)
    {
      p->pspec->owner_type->set_property(obj, p->pspec, p->value);
    }
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



#ifdef L_DEBUG
#define TRACE_REF(obj, msg) do {                                        \
    if (L_OBJECT(obj)->trace_ref) {                                     \
      gchar *str = l_object_to_string(obj);                             \
      CL_DEBUG("%s: %s -> %d", (msg), str, L_OBJECT(obj)->ref_count);   \
      g_free(str);                                                      \
      CL_BACKTRACE();                                                   \
    }                                                                   \
  } while (0)
#else
#define TRACE_REF(obj, msg)
#endif



/* l_object_trace_ref:
 */
#ifdef L_DEBUG
gpointer l_object_trace_ref ( gpointer object,
                              gboolean enable )
{
  L_OBJECT(object)->trace_ref = (enable ? 1 : 0);
  return object;
}
#endif



/* l_object_ref:
 */
gpointer l_object_ref ( gpointer obj )
{
  g_atomic_int_inc(&((LObject *)obj)->ref_count);
  TRACE_REF(obj, "REF");
  return obj;
}



/* l_object_unref:
 */
void l_object_unref ( gpointer obj )
{
  gint val = g_atomic_int_add(&((LObject *)obj)->ref_count, -1);
  ASSERT(val != 0);
  if (val == 1)
    {
      TRACE_REF(obj, "DESTROY");
      l_object_dispose(obj);
      if (L_OBJECT_GET_CLASS(obj)->finalize)
        L_OBJECT_GET_CLASS(obj)->finalize(obj);
      /* [FIXME] use object allocator */
      g_free(obj);
    }
  else
    {
      TRACE_REF(obj, "UNREF");
    }
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
  ASSERT(object);
  /* [FIXME] this lookup for every destroyed object is painfull (also,
     maybe it should be in finalize ?) */
  _l_signal_object_dispose(object);
}



/* l_object_get_property:
 */
LObject *l_object_get_property ( LObject *object,
                                 const gchar *name )
{
  LParamSpec *pspec;
  if (!(pspec = l_param_spec_pool_lookup(pspec_pool,
                                         L_OBJECT_GET_CLASS(object),
                                         name)))
    CL_ERROR("param not found: %s.%s",
             l_object_class_name(L_OBJECT_GET_CLASS(object)),
             name);
  ASSERT(pspec->owner_type->get_property);
  return pspec->owner_type->get_property(object, pspec);
}



/* l_object_set_property:
 */
void l_object_set_property ( LObject *object,
                             const gchar *name,
                             LObject *value )
{
  LParamSpec *pspec;
  if (!(pspec = l_param_spec_pool_lookup(pspec_pool,
                                         L_OBJECT_GET_CLASS(object),
                                         name)))
    CL_ERROR("param not found: %s.%s",
             l_object_class_name(L_OBJECT_GET_CLASS(object)),
             name);
  ASSERT(pspec->owner_type->set_property);
  pspec->owner_type->set_property(object, pspec, value);
}



/* l_object_notify:
 */
void l_object_notify ( LObject *object,
                       LParamSpec *pspec )
{
  l_signal_emit(object, signals[SIG_NOTIFY], pspec->qname, NULL);
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



/* l_object_repr:
 */
const gchar *l_object_repr ( LObject *object )
{
  /* [TODO] */
  return l_object_to_string(object);
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
