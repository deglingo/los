/* lpacker.c -
 */

#include "los/lpacker.h"
#include "los/lpacker.inl"



/* l_packer_new:
 */
LPacker *l_packer_new ( LStream *stream )
{
  LPacker *p;
  p = L_PACKER(l_object_new(L_CLASS_PACKER, NULL));
  return p;
}



/* l_packer_put:
 */
gboolean l_packer_put ( LPacker *packer,
                        LObject *object,
                        GError **error )
{
  return TRUE;
}
