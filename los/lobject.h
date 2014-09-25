/* lobject.h -
 */

#ifndef _LOBJECT_H_
#define _LOBJECT_H_

#include "los/lbase.h"



/* [FIXME] */
#define L_CLASS_OBJECT (l_object_get_class())

typedef struct _LObject LObject;
typedef struct _LObjectClass LObjectClass;
typedef struct _LClassInfo LClassInfo;

typedef void (* LObjectClassInitFunc) ( LObjectClass *cls );
typedef void (* LObjectInstanceInitFunc) ( LObject *obj );

/* [TODO] check/cast macros */
#define L_OBJECT_CHECK_INSTANCE_CAST(obj, type, s_type) ((s_type *)(obj))
#define L_OBJECT_CHECK_INSTANCE_TYPE(obj, type) \
  (l_object_issubclass((LObject *) /* [fixme] */ L_OBJECT_GET_CLASS(obj), \
                       (LObject *) /* [fixme] */ (type)))
#define L_OBJECT_GET_CLASS_CAST(obj, type, s_type) ((s_type *)(((LObject *)(obj))->l_class))
#define L_OBJECT_CHECK_CLASS_CAST(cls, type, s_type) ((s_type *)(cls))

/* [FIXME] */
#define L_OBJECT(obj) (L_OBJECT_CHECK_INSTANCE_CAST((obj), L_CLASS_OBJECT, LObject))
#define L_OBJECT_GET_CLASS(obj) (L_OBJECT(obj)->l_class)
#define L_OBJECT_CLASS(cls) (L_OBJECT_CHECK_CLASS_CAST((cls), L_CLASS_OBJECT, LObjectClass))



/* LClassInfo:
 */
struct _LClassInfo
{
  guint class_size;
  LObjectClassInitFunc class_init;
  guint instance_size;
  LObjectInstanceInitFunc init;
};



/* LObject:
 */
struct _LObject
{
  LObjectClass *l_class;

  guint ref_count;

  /* debug */
#ifdef L_DEBUG
  guint trace_ref : 1;
#endif
};



/* LObjectClass:
 */
struct _LObjectClass
{
  LObject l_object;
  /* [fixme] */
  LClassInfo l_class_info;
  gchar *name;
  LObjectClass *l_parent_class;

  gchar * (* to_string) ( LObject *object );

  /* destructor and finalization */
  void (* dispose) ( LObject *object );
  void (* finalize) ( LObject *object );

  /* serialization */
  LObject * (* get_state) ( LObject *object );
  LObject * (* from_state) ( LObjectClass *cls,
                             LObject *state );
};



LObjectClass *l_object_get_class ( void );
LObjectClass *l_object_class_register ( const gchar *name,
                                        LObjectClass *parent,
                                        LClassInfo *info );
gboolean l_object_isclass ( LObject *obj );
gboolean l_object_issubclass ( LObject *cls1,
                               LObject *cls2 );
const gchar *l_object_class_name ( LObjectClass *cls );
LObjectClass *l_object_class_from_name ( const gchar *name );
LObject *l_object_new ( LObjectClass *cls,
                        const char *first_prop,
                        ... );
LObject *l_object_new_from_state ( LObjectClass *cls,
                                   LObject *state );
/* ref counting */
#define L_OBJECT_CLEAR(ptr) do {                \
    gpointer __l_object_clear_tmp = (ptr);      \
    (ptr) = NULL;                               \
    l_object_unref(__l_object_clear_tmp);       \
  } while (0)

gpointer l_object_ref ( gpointer obj );
void l_object_unref ( gpointer obj );
void l_object_dispose ( LObject *object );
LObject *l_object_get_state ( LObject *object );
gchar *l_object_to_string ( LObject *object );

/* debug */
#ifdef L_DEBUG
void l_object_set_trace_ref ( LObject *object,
                              gboolean enable );
#else
#define l_object_set_trace_ref(obj, enable)
#endif



#endif
