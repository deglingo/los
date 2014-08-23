/* ltype.h -
 */

#ifndef _LTYPE_H_
#define _LTYPE_H_

#include "los/lbase.h"



typedef guint LType;
typedef struct _LTypeInfo LTypeInfo;



/* LTypeInfo:
 */
struct _LTypeInfo
{
  guint class_size;
  guint instance_size;
};



void l_type_init ( void );
LType l_type_register ( const char *name,
                        LType parent,
                        LTypeInfo *info );
void *l_type_instantiate ( LType type );



#endif
