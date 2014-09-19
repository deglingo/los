/* ltuple.h -
 */

#ifndef _LTUPLE_H_
#define _LTUPLE_H_

#include "los/lobject.h"
#include "los/ltuple-def.h"



/* LTuple:
 */
struct _LTuple
{
  L_TUPLE_INSTANCE_HEADER;
};



/* LTupleClass:
 */
struct _LTupleClass
{
  L_TUPLE_CLASS_HEADER;
};



LTuple *l_tuple_new ( guint size );



#endif
