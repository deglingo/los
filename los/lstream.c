/* lstream.c -
 */

#include "los/private.h"
#include "los/lstream.h"
#include "los/lstream.inl"



/* l_stream_write:
 */
LStreamStatus l_stream_write ( LStream *stream,
                               gconstpointer buffer,
                               gint64 size,
                               gint64 *bytes_written,
                               GError **error )
{
  ASSERT(L_STREAM_GET_CLASS(stream)->write);
  return L_STREAM_GET_CLASS(stream)->write(stream, buffer, size, bytes_written, error);
}



/* l_stream_read:
 */
LStreamStatus l_stream_read ( LStream *stream,
                              gpointer buffer,
                              gint64 size,
                              gint64 *bytes_read,
                              GError **error )
{
  ASSERT(L_STREAM_GET_CLASS(stream)->read);
  return L_STREAM_GET_CLASS(stream)->read(stream, buffer, size, bytes_read, error);
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



/* l_stream_close:
 */
void l_stream_close ( LStream *stream )
{
  ASSERT(L_STREAM_GET_CLASS(stream)->close);
  L_STREAM_GET_CLASS(stream)->close(stream);
}



/* l_stream_eof:
 */
gboolean l_stream_eof ( LStream *stream )
{
  ASSERT(L_STREAM_GET_CLASS(stream)->close);
  return L_STREAM_GET_CLASS(stream)->eof(stream);
}
