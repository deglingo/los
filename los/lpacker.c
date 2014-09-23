/* lpacker.c -
 */

#include "los/private.h"
#include "los/lpacker.h"
#include "los/lpackprivate.h"
#include "los/lint.h" /* ?? */
#include "los/lstring.h"
#include "los/ltuple.h"
#include "los/lpacker.inl"



/* Private:
 */
typedef struct _Private
{
  int _dummy;
}
  Private;

#define PRIVATE(p) ((Private *) (L_PACKER(p)->private))



static void _dispose ( LObject *object );



/* l_packer_class_init:
 */
static void l_packer_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* l_packer_init:
 */
static void l_packer_init ( LObject *obj )
{
  L_PACKER(obj)->private = g_new0(Private, 1);
}



/* l_packer_new:
 */
LPacker *l_packer_new ( LStream *stream )
{
  LPacker *p;
  p = L_PACKER(l_object_new(L_CLASS_PACKER, NULL));
  p->stream = l_object_ref(stream);
  return p;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  L_OBJECT_CLEAR(L_PACKER(object)->stream);
  g_free(PRIVATE(object));
  /* [FIXME] */
  ((LObjectClass *) parent_class)->dispose(object);
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



gboolean _l_packer_put_tuple ( LPacker *packer,
                               LObject *object,
                               GError **error )
{
  gint64 w;
  guint8 tp = PACK_KEY_TUPLE;
  guint32 nsize = GUINT32_TO_BE(L_TUPLE_SIZE(object));
  guint32 i;
  /* write type */
  if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(guint8));
  /* write size */
  if (l_stream_write(packer->stream, &nsize, sizeof(nsize), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(nsize));
  /* write items */
  for (i = 0; i < L_TUPLE_SIZE(object); i++) {
    if (!l_packer_put(packer, L_TUPLE_ITEM(object, i), error))
      return FALSE;
  }
  /* ok */
  return TRUE;
}



gboolean _l_packer_put_object ( LPacker *packer,
                                LObject *object,
                                GError **error )
{
  guint8 tp = PACK_KEY_OBJECT;
  const gchar *clsname = l_object_class_name(L_OBJECT_GET_CLASS(object));
  guint32 clslen = strlen(clsname);
  guint32 clsnlen = GUINT32_TO_BE(clslen);
  LObject *state = l_object_get_state(object);
  gint64 w;
  gboolean r;
  /* write type */
  if (l_stream_write(packer->stream, &tp, sizeof(tp), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(tp));
  /* write clslen */
  if (l_stream_write(packer->stream, &clsnlen, sizeof(clsnlen), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(clsnlen));
  /* write clsname */
  if (l_stream_write(packer->stream, (gchar *) /* [FIXME] */ clsname, clslen, &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == clslen);
  /* write object */
  r = l_packer_put(packer, state, error);
  l_object_unref(state);
  return r;
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
  } else if (L_IS_TUPLE(object)) {
    return _l_packer_put_tuple(packer, object, error);
  } else {
    return _l_packer_put_object(packer, object, error);
  }
}



/* l_packer_add:
 */
void l_packer_add ( LPacker *packer,
                    LObject *object )
{
}



/* l_packer_send:
 */
gboolean l_packer_send ( LPacker *packer,
                         GError **error )
{
  return TRUE;
}
