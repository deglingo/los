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
#define L_OBJECT_GET_CLASS_CAST(obj, type, s_type) ((s_type *)(((LObject *)(obj))->l_class))



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
};



/* LObjectClass:
 */
struct _LObjectClass
{
  /* [fixme] */
  LClassInfo l_class_info;
  LObjectClass *l_parent_class;
};



LObjectClass *l_object_get_class ( void );
LObjectClass *l_object_class_register ( const gchar *name,
                                        LObjectClass *parent,
                                        LClassInfo *info );
LObject *l_object_new ( LObjectClass *cls,
                        const char *first_prop,
                        ... );
/* ref counting */
#define L_OBJECT_CLEAR(ptr) do {                \
    gpointer __l_object_clear_tmp = (ptr);      \
    (ptr) = NULL;                               \
    l_object_unref(__l_object_clear_tmp);       \
  } while (0)

gpointer l_object_ref ( gpointer obj );
void l_object_unref ( gpointer obj );



#endif
