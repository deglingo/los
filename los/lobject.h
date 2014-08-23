/* lobject.h -
 */

#ifndef _LOBJECT_H_
#define _LOBJECT_H_

#include "los/ltype.h"



/* [FIXME] */
#define L_TYPE_OBJECT (l_object_get_type())

typedef struct _LObject LObject;
typedef struct _LObjectClass LObjectClass;

/* [TODO] check/cast macros */
#define L_OBJECT_CHECK_INSTANCE_CAST(obj, type, s_type) ((s_type *)(obj))



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
  int _dummy;
};



LType l_object_get_type ( void );
LObject *l_object_new ( LType type,
                        const char *first_prop,
                        ... );



#endif
