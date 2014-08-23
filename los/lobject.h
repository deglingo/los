/* lobject.h -
 */

#ifndef _LOBJECT_H_
#define _LOBJECT_H_



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



#endif
