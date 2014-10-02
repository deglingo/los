/* lsignal.c -
 */

#include "los/lsignal.h"



/* l_signal_new:
 */
LSignalID l_signal_new ( LObjectClass *cls,
                         const gchar *name )
{
  return 0;
}



/* l_signal_connect:
 */
void l_signal_connect ( LObject *object,
                        const gchar *name,
                        LSignalHandler handler,
                        gpointer data,
                        GDestroyNotify destroy_data )
{
}
