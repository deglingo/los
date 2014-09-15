/* lstream.c -
 */

#include "los/private.h"
#include "los/lstream.h"
#include "los/lstream.inl"



/* l_stream_write:
 */
gboolean l_stream_write ( LStream *stream,
                          gpointer buffer,
                          gsize size,
                          GError **error )
{
  GError *tmperr = NULL;
  ASSERT(L_STREAM_GET_CLASS(stream)->write);
  L_STREAM_GET_CLASS(stream)->write(stream, buffer, size, &tmperr);
  if (tmperr) {
    g_propagate_error(error, tmperr);
    return FALSE;
  } else {
    return TRUE;
  }
}



/* l_stream_seek:
 */
void l_stream_seek ( LStream *stream,
                     glong offset,
                     LStreamSeekType whence )
{
  /* [TODO] */
}
