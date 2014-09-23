/* bytestream.c -
 */

#include "libltest/bytestream.h"
#include "libltest/bytestream.inl"

#include <unistd.h>
#include <string.h>
#include <errno.h>



static void _dispose ( LObject *object );
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
  cls->dispose = _dispose;
  ((LStreamClass *) cls)->read = _read;
  ((LStreamClass *) cls)->write = _write;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  ByteStream *b = BYTE_STREAM(object);
  if (b->f) {
    fclose(b->f);
    b->f = NULL;
  }
  /* [fixme] */
  ((LObjectClass *) parent_class)->dispose(object);
}



/* byte_stream_new:
 */
LStream *byte_stream_new ( const gchar *fname,
                           const gchar *mode )
{
  ByteStream *b = BYTE_STREAM(l_object_new(CLASS_BYTE_STREAM, NULL));
  if (!(b->f = fopen(fname, mode))) {
    fprintf(stderr, "fopen error: %s", strerror(errno));
    abort();
  }
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
      ssize_t s = read(fileno(bs->f), buffer, 1);
      if (s != 1) {
        fprintf(stderr, "fread error: %s (%d)\n", strerror(errno), s);
        abort();
      }
      *bytes_read = 1;
      bs->ready = FALSE;
      /* fprintf(stderr, "%%"); */
      /* fflush(stderr); */
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
        fprintf(stderr, "fwrite error: %s\n", strerror(errno));
        abort();
      }
      *bytes_written = 1;
      bs->ready = FALSE;
      /* fprintf(stderr, "#"); */
      /* fflush(stderr); */
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
