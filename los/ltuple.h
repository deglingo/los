/* ltuple.h -
 */

#ifndef _LTUPLE_H_
#define _LTUPLE_H_

#include "los/lobject.h"
#include "los/ltuple-def.h"

struct _LList;



/* LTuple:
 */
struct _LTuple
{
  L_TUPLE_INSTANCE_HEADER;

  guint _size;
  LObject **_items;
};



/* LTupleClass:
 */
struct _LTupleClass
{
  L_TUPLE_CLASS_HEADER;
};



#define L_TUPLE_SIZE(t)    (L_TUPLE(t)->_size)
#define L_TUPLE_ITEM(t, i) (L_TUPLE(t)->_items[(i)])

LTuple *l_tuple_new ( guint size );
LTuple *l_tuple_newl_give ( guint size,
                            ... )
G_GNUC_NULL_TERMINATED;
LTuple *l_tuple_newv_give ( guint size,
                            va_list args );
LTuple *l_tuple_new_from_list ( struct _LList *list );
void l_tuple_give_item ( LTuple *tuple,
                         guint index,
                         LObject *item );



#endif
