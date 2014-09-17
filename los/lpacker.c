/* lpacker.c -
 */

#include "los/private.h"
#include "los/lpacker.h"
#include "los/lint.h" /* ?? */
#include "los/lpacker.inl"



enum
  {
    PACK_KEY_INT = 0,
  };



/* l_packer_new:
 */
LPacker *l_packer_new ( LStream *stream )
{
  LPacker *p;
  p = L_PACKER(l_object_new(L_CLASS_PACKER, NULL));
  p->stream = l_object_ref(stream);
  return p;
}



gboolean _l_packer_put_int ( LPacker *packer,
                             LObject *object,
                             GError **error )
{
  gint64 w;
  guint8 tp = PACK_KEY_INT;
  gint32 val = GINT_TO_BE(L_INT_VALUE(object));
  /* write type */
  if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(guint8));
  /* write value */
  /* [FIXME] gint is not portable!! */
  if (l_stream_write(packer->stream, &val, sizeof(gint), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(gint));
  /* ok */
  return TRUE;
}



/* l_packer_put:
 */
gboolean l_packer_put ( LPacker *packer,
                        LObject *object,
                        GError **error )
{
  if (L_IS_INT(object)) {
    return _l_packer_put_int(packer, object, error);
  } else {
    gint64 w;
    guint8 tp = 1;
    if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK)
      CL_ERROR("[TODO] write error");
    ASSERT(w == sizeof(guint8));
    return TRUE;
  }
}
