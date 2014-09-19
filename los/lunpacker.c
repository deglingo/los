/* lunpacker.c -
 */

#include "los/private.h"
#include "los/lunpacker.h"
#include "los/lint.h" /* ?? */
#include "los/lstring.h" /* ?? */
#include "los/lunpacker.inl"



static void _dispose ( LObject *object );



/* l_unpacker_class_init:
 */
static void l_unpacker_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* l_unpacker_new:
 */
LUnpacker *l_unpacker_new ( LStream *stream )
{
  LUnpacker *u;
  u = L_UNPACKER(l_object_new(L_CLASS_UNPACKER, NULL));
  u->stream = l_object_ref(stream);
  return u;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  L_OBJECT_CLEAR(L_UNPACKER(object)->stream);
  /* [FIXME] */
  ((LObjectClass *) parent_class)->dispose(object);
}



LObject *_l_unpacker_get_int ( LUnpacker *unpacker,
                              GError **error )
{
  gint val;
  gint64 w;
  if (l_stream_read(unpacker->stream, &val, sizeof(gint), &w, error) != L_STREAM_STATUS_OK)
    return NULL;
  ASSERT(w == sizeof(gint));
  return L_OBJECT(l_int_new(GINT_FROM_BE(val)));
}



LObject *_l_unpacker_get_string ( LUnpacker *unpacker,
                                  GError **error )
{
  guint32 nlen, len;
  gint64 w;
  gchar *str;
  LString *lstr;
  /* read len */
  if (l_stream_read(unpacker->stream, &nlen, sizeof(nlen), &w, error) != L_STREAM_STATUS_OK)
    return NULL;
  ASSERT(w == sizeof(nlen));
  len = GUINT32_FROM_BE(nlen);
  /* read string */
  str = g_malloc(len+1);
  if (l_stream_read(unpacker->stream, str, len, &w, error) != L_STREAM_STATUS_OK) {
    g_free(str);
    return NULL;
  }
  ASSERT(w == len);
  str[len] = 0;
  /* create the LString */
  /* [FIXME] we should have a way to give str without copying it */
  lstr = l_string_new(str);
  g_free(str);
  /* ok */
  return L_OBJECT(lstr);
}



/* l_unpacker_get:
 */
LObject *l_unpacker_get ( LUnpacker *unpacker,
                          GError **error )
{
  guint8 t;
  GError *err = NULL;
  gint64 r;
  LStreamStatus s;
  /* read tp */
  s = l_stream_read(unpacker->stream, &t, sizeof(guint8), &r, &err);
  switch (s) {
  case L_STREAM_STATUS_OK:
    break;
  case L_STREAM_STATUS_EOF:
    g_set_error(error, L_PACK_ERROR, L_PACK_ERROR_EOF, "no more object to read");
    return NULL;
  default:
    CL_ERROR("[TODO] read error");
  }
  ASSERT(r == sizeof(guint8));
  if (t == 0) { /* [FIXME] */
    return _l_unpacker_get_int(unpacker, error);
  } else if (t == 1) {
    return _l_unpacker_get_string(unpacker, error);
  } else {
    CL_ERROR("[TODO] tp %d", t);
    return NULL;
  }
}
