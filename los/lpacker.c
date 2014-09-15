/* lpacker.c -
 */

#include "los/private.h"
#include "los/lpacker.h"
#include "los/lint.h" /* ?? */
#include "los/lpacker.inl"



/* l_packer_new:
 */
LPacker *l_packer_new ( LStream *stream )
{
  LPacker *p;
  p = L_PACKER(l_object_new(L_CLASS_PACKER, NULL));
  p->stream = l_object_ref(stream);
  return p;
}



/* l_packer_put:
 */
gboolean l_packer_put ( LPacker *packer,
                        LObject *object,
                        GError **error )
{
  if (!l_stream_write_u8(packer->stream, (L_IS_INT(object) ? 0 : 1), error))
    CL_ERROR("[TODO] write error");
  return TRUE;
}
