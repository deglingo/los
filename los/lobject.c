/* lobject.c -
 */

#include "los/lobject.h"

/* [REMOVEME] */
#include <stdlib.h>
#include <string.h>



/* l_object_new:
 */
LObject *l_object_new ( LType type,
                        const char *first_prop,
                        ... )
{
  LObject *obj;
  obj = malloc(sizeof(LObject));
  memset(obj, 0, sizeof(LObject));
  return obj;
}
