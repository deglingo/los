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



/* l_unpacker_get:
 */
LObject *l_unpacker_get ( LUnpacker *unpacker,
                          GError **error )
{
  guint8 t;
  GError *err = NULL;
  gint64 r;
  if (l_stream_read(unpacker->stream, &t, sizeof(guint8), &r, &err) != L_STREAM_STATUS_OK)
    CL_ERROR("[TODO] read error");
  ASSERT(r == sizeof(guint8));
  if (t == 0)
    return L_OBJECT(l_int_new(92));
  else
    return L_OBJECT(l_string_new("test-string"));
}
