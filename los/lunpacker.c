/* lunpacker.c -
 */

#include "los/private.h"
#include "los/lunpacker.h"
#include "los/lint.h" /* ?? */
#include "los/lstring.h" /* ?? */
#include "los/lunpacker.inl"



/* l_unpacker_new:
 */
LUnpacker *l_unpacker_new ( LStream *stream )
{
  LUnpacker *u;
  u = L_UNPACKER(l_object_new(L_CLASS_UNPACKER, NULL));
  u->stream = l_object_ref(stream);
  return u;
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
  if (t == 0)
    return _l_unpacker_get_int(unpacker, error);
  else
    return L_OBJECT(l_string_new("test-string"));
}
