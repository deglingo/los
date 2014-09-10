/* lint.h -
 */

#ifndef _LINT_H_
#define _LINT_H_

#include "los/lobject.h"
#include "los/lint-def.h"



/* LInt:
 */
struct _LInt
{
  L_INT_INSTANCE_HEADER;

  gint value;
};



/* LIntClass:
 */
struct _LIntClass
{
  L_INT_CLASS_HEADER;
};



#define L_INT_VALUE(obj) (L_INT(obj)->value)

LInt *l_int_new ( gint value );



#endif
