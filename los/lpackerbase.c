/* lpackerbase.c -
 */

#include "los/lpackerbase.h"



/* l_pack_error_quark:
 */
GQuark l_pack_error_quark ( void )
{
  /* [FIXME] thread lock */
  static GQuark q = 0;
  if (q == 0)
    q = g_quark_from_string("l-pack-error");
  return q;
}
