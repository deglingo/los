/* ltype.c -
 */

#include "los/ltype.h"



static gboolean l_type_initialized = FALSE;



/* l_type_init:
 */
void l_type_init ( void )
{
  if (l_type_initialized)
    return;
  l_type_initialized = TRUE;
}



/* l_type_register:
 */
LType l_type_register ( const char *name,
                        LType parent,
                        LTypeInfo *info )
{
  if (!l_type_initialized) {
    fprintf(stderr, "ERROR: l_type_init() must be called first\n");
    exit(1);
  }
  return 1;
}
