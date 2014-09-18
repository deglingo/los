/* lpacker.c -
 */

#include "los/private.h"
#include "los/lpacker.h"
#include "los/lint.h" /* ?? */
#include "los/lstring.h"
#include "los/lpacker.inl"



enum
  {
    PACK_KEY_INT = 0,
    PACK_KEY_STRING,
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



gboolean _l_packer_put_string ( LPacker *packer,
                                LObject *object,
                                GError **error )
{
  gint64 w;
  guint8 tp = PACK_KEY_STRING;
  /* [fixme] LString.len is guint!! */
  guint32 len = GUINT32_TO_BE((guint32)(L_STRING(object)->len));
  /* write type */
  if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(guint8));
  /* write len */
  if (l_stream_write(packer->stream, &len, sizeof(len), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(len));
  /* write string */
  if (l_stream_write(packer->stream, L_STRING(object)->str, L_STRING(object)->len, &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == L_STRING(object)->len);
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
  } else if (L_IS_STRING(object)) {
    return _l_packer_put_string(packer, object, error);
  } else {
    CL_ERROR("[TODO] pack type...");
    return 0;
    /* gint64 w; */
    /* guint8 tp = 1; */
    /* if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK) */
    /*   CL_ERROR("[TODO] write error"); */
    /* ASSERT(w == sizeof(guint8)); */
    /* return TRUE; */
  }
}
