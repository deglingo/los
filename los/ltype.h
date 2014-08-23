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
  unsigned int class_size;
};



LType l_type_register ( const char *name,
                        LType parent,
                        LTypeInfo *info );



#endif
