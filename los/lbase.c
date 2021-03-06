/* los_init -
 */

#include "los/private.h"
#include "los/lbase.h"
#include "los/lobject.h"
#include "los/lnone.h"
#include "los/lsignal.h"



/* los_init_once:
 */
static gpointer los_init_once ( gpointer dummy )
{
  _l_signal_init();
  _l_object_init();
  _l_none_init();
  return NULL;
}



/* los_init:
 */
void los_init ( void )
{
  static GOnce once = G_ONCE_INIT;
  g_once(&once, los_init_once, NULL);
}
