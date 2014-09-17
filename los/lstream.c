/* lstream.c -
 */

#include "los/private.h"
#include "los/lstream.h"
#include "los/lstream.inl"



/* l_stream_write:
 */
LStreamStatus l_stream_write ( LStream *stream,
                               gpointer buffer,
                               gint64 size,
                               gint64 *bytes_written,
                               GError **error )
{
  ASSERT(L_STREAM_GET_CLASS(stream)->write);
  return L_STREAM_GET_CLASS(stream)->write(stream, buffer, size, bytes_written, error);
}



/* l_stream_read:
 */
gint64 l_stream_read ( LStream *stream,
                       gpointer buffer,
                       gint64 size,
                       GError **error )
{
  GError *tmperr = NULL;
  gint64 r;
  ASSERT(L_STREAM_GET_CLASS(stream)->read);
  r = L_STREAM_GET_CLASS(stream)->read(stream, buffer, size, &tmperr);
  if (size < 0) {
    ASSERT(tmperr);
    g_propagate_error(error, tmperr);
  } else {
    ASSERT(!tmperr);
  }
  return r;
}



/* l_stream_read_u8:
 */
gint64 l_stream_read_u8 ( LStream *stream,
                          guint8 *value,
                          GError **error )
{
  return l_stream_read(stream, value, sizeof(guint8), error);
}



/* l_stream_seek:
 */
void l_stream_seek ( LStream *stream,
                     gint64 offset,
                     LStreamSeekType whence )
{
  ASSERT(L_STREAM_GET_CLASS(stream)->seek);
  L_STREAM_GET_CLASS(stream)->seek(stream, offset, whence);
}
