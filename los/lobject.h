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

/* [TODO] check/cast macros */
#define L_OBJECT_CHECK_INSTANCE_CAST(obj, type, s_type) ((s_type *)(obj))



/* LClassInfo:
 */
struct _LClassInfo
{
  guint class_size;
  guint instance_size;
};



/* LObject:
 */
struct _LObject
{
  int _dummy;
};



/* LObjectClass:
 */
struct _LObjectClass
{
  LClassInfo l_class_info;
};



LObjectClass *l_object_get_class ( void );
LObjectClass *l_object_class_register ( const gchar *name,
                                        LObjectClass *parent,
                                        LClassInfo *info );
LObject *l_object_new ( LObjectClass *cls,
                        const char *first_prop,
                        ... );



#endif
