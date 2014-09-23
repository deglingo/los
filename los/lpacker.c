/* lpacker.c -
 */

#include "los/private.h"
#include "los/lpacker.h"
#include "los/lpackprivate.h"
#include "los/lint.h" /* ?? */
#include "los/lstring.h"
#include "los/ltuple.h"
#include "los/lpacker.inl"



#define BUFFER_SIZE 16



/* Private:
 */
typedef struct _Private
{
  LObject *object;
  gint stage;
  gchar real_buffer[BUFFER_SIZE];
  gchar *buffer;
  guint data_size;
  guint buffer_offset;
}
  Private;

#define PRIVATE(p) ((Private *) (L_PACKER(p)->private))



enum
  {
    S_INT_START = 0,
    S_INT_WRITE_TYPE,
    S_INT_WRITE_VALUE,
  };



enum
  {
    S_STRING_START = 0,
    S_STRING_WRITE_TYPE,
    S_STRING_WRITE_LEN,
    S_STRING_WRITE_VALUE,
  };



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
  PRIVATE(packer)->object = l_object_ref(object);
  PRIVATE(packer)->stage = 0;
}



static gboolean _send ( LPacker *packer,
                        GError **error )
{
  Private *priv = PRIVATE(packer);
  gint64 w, size;
  while (priv->buffer_offset < priv->data_size)
    {
      LStreamStatus s;
      size = priv->data_size - priv->buffer_offset;
      s = l_stream_write(packer->stream,
                         priv->buffer + priv->buffer_offset,
                         size,
                         &w,
                         error);
      switch (s) {
      case L_STREAM_STATUS_OK:
        priv->buffer_offset += w;
        break;
      case L_STREAM_STATUS_AGAIN:
        return FALSE;
      default:
        CL_ERROR("[TODO] s = %d", s);
        return 0;
      }
    }
  return TRUE;
}



#define BUFFER_SET(priv, tp, val) do {          \
    ASSERT(sizeof(tp) <= BUFFER_SIZE);          \
    (priv)->buffer = (priv)->real_buffer;       \
    *((tp *)((priv)->buffer)) = (val);          \
    (priv)->data_size = sizeof(tp);             \
    (priv)->buffer_offset = 0;                  \
  } while (0)



#define BUFFER_DIVERT(priv, src, len) do {      \
    (priv)->buffer = (src);                     \
    (priv)->data_size = (len);                  \
    (priv)->buffer_offset = 0;                  \
  } while (0)



static gboolean _send_int ( LPacker *packer,
                            GError **error )
{
  Private *priv = PRIVATE(packer);
  switch (priv->stage)
    {
    case S_INT_START:
      BUFFER_SET(priv, guint8, (guint8) PACK_KEY_INT);
      priv->stage = S_INT_WRITE_TYPE;
    case S_INT_WRITE_TYPE:
      {
        gint32 val;
        val = L_INT_VALUE(priv->object);
        if (!_send(packer, error))
          return FALSE;
        /* value */
        BUFFER_SET(priv, gint32, GINT32_TO_BE(val));
        priv->stage = S_INT_WRITE_VALUE;
      }
    case S_INT_WRITE_VALUE:
      if (!_send(packer, error))
        return FALSE;
    }
  L_OBJECT_CLEAR(priv->object);
  return TRUE;
}



static gboolean _send_string ( LPacker *packer,
                               GError **error )
{
  Private *priv = PRIVATE(packer);
  switch (priv->stage)
    {
    case S_STRING_START:
      BUFFER_SET(priv, guint8, (guint8) PACK_KEY_STRING);
      priv->stage = S_STRING_WRITE_TYPE;
    case S_STRING_WRITE_TYPE:
      if (!_send(packer, error))
        return FALSE;
      /* string len */
      BUFFER_SET(priv, guint32, GUINT_TO_BE((guint32)(L_STRING(priv->object)->len)));
      priv->stage = S_STRING_WRITE_LEN;
    case S_STRING_WRITE_LEN:
      if (!_send(packer, error))
        return FALSE;
      /* string value */
      BUFFER_DIVERT(priv, L_STRING(priv->object)->str, L_STRING(priv->object)->len);
      priv->stage = S_STRING_WRITE_VALUE;
    case S_STRING_WRITE_VALUE:
      if (!_send(packer, error))
        return FALSE;
    }
  L_OBJECT_CLEAR(priv->object);
  return TRUE;
}



/* l_packer_send:
 */
gboolean l_packer_send ( LPacker *packer,
                         GError **error )
{
  if (L_IS_INT(PRIVATE(packer)->object)) {
    return _send_int(packer, error);
  } else if (L_IS_STRING(PRIVATE(packer)->object)) {
    return _send_string(packer, error);
  } else {
    ASSERT(0);
    return FALSE;
  }
}
