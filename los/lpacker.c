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
  gint64 w;
  guint8 tp = L_IS_INT(object) ? 0 : 1;
  if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK)
    CL_ERROR("[TODO] write error");
  ASSERT(w == sizeof(guint8));
  return TRUE;
}
