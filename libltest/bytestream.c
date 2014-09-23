/* bytestream.c -
 */

#include "libltest/bytestream.h"
#include "libltest/bytestream.inl"

#include <string.h>
#include <errno.h>



LStreamStatus _read ( LStream *stream,
                      gpointer buffer,
                      gint64 size,
                      gint64 *bytes_read,
                      GError **error );
LStreamStatus _write ( LStream *stream,
                       gpointer buffer,
                       gint64 size,
                       gint64 *bytes_read,
                       GError **error );



/* byte_stream_class_init:
 */
static void byte_stream_class_init ( LObjectClass *cls )
{
  ((LStreamClass *) cls)->read = _read;
  ((LStreamClass *) cls)->write = _write;
}



/* byte_stream_new:
 */
LStream *byte_stream_new ( FILE *f )
{
  ByteStream *b = BYTE_STREAM(l_object_new(CLASS_BYTE_STREAM, NULL));
  b->f = f;
  b->ready = FALSE;
  return L_STREAM(b);
}



/* _read:
 */
LStreamStatus _read ( LStream *stream,
                      gpointer buffer,
                      gint64 size,
                      gint64 *bytes_read,
                      GError **error )
{
#define bs (BYTE_STREAM(stream))
  if (bs->ready)
    {
      size_t s = fread(buffer, 1, 1, bs->f);
      if (s != 1) {
        fprintf(stderr, "fread error: %s", strerror(errno));
        abort();
      }
      *bytes_read = 1;
      bs->ready = FALSE;
      return L_STREAM_STATUS_OK;
    }
  else
    {
      bs->ready = TRUE;
      return L_STREAM_STATUS_AGAIN;
    }
#undef bs
}



/* _write:
 */
LStreamStatus _write ( LStream *stream,
                       gpointer buffer,
                       gint64 size,
                       gint64 *bytes_written,
                       GError **error )
{
#define bs (BYTE_STREAM(stream))
  if (bs->ready)
    {
      size_t s = fwrite(buffer, 1, 1, bs->f);
      if (s != 1) {
        fprintf(stderr, "fwrite error: %s", strerror(errno));
        abort();
      }
      *bytes_written = 1;
      bs->ready = FALSE;
      return L_STREAM_STATUS_OK;
    }
  else
    {
      bs->ready = TRUE;
      return L_STREAM_STATUS_AGAIN;
    }
  bs->ready = !bs->ready;
#undef bs
}
