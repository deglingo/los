/* los_init -
 */

#include "los/private.h"
#include "los/lbase.h"



/* los_init_once:
 */
static gpointer los_init_once ( gpointer dummy )
{
  return NULL;
}



/* los_init:
 */
void los_init ( void )
{
  static GOnce once = G_ONCE_INIT;
  g_once(&once, los_init_once, NULL);
}
