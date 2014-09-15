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
  if (!l_stream_read_u8(unpacker->stream, &t, &err))
    CL_ERROR("[TODO] read error");
  if (t == 0)
    return L_OBJECT(l_int_new(92));
  else
    return L_OBJECT(l_string_new("test-string"));
}
