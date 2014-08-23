/* lobject.c -
 */

#include "los/lobject.h"

/* [REMOVEME] */
#include <stdlib.h>
#include <string.h>



/* l_object_get_type:
 */
LType l_object_get_type ( void )
{
  static LType type = 0;
  if (type == 0) {
    LTypeInfo info = { 0, };
    info.class_size = sizeof(LObjectClass);
    /* info.instance_size = sizeof(LObject); */
    type = l_type_register("LObject", 0, &info);
  }
  return type;
}



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
