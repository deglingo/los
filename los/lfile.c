/* lfile.c -
 */

#include "los/private.h"
#include "los/lfile.h"
#include "los/lfile.inl"
#include <unistd.h>



static LStreamStatus _write ( LStream *stream,
                              gconstpointer buffer,
                              gint64 size,
                              gint64 *bytes_written,
                              GError **error );
static LStreamStatus _read ( LStream *stream,
                             gpointer buffer,
                             gint64 size,
                             gint64 *bytes_read,
                             GError **error );
static void _close ( LStream *stream );
static gboolean _eof ( LStream *stream );



/* l_file_class_init:
 */
static void l_file_class_init ( LObjectClass *cls )
{
  ((LStreamClass *) cls)->write = _write;
  ((LStreamClass *) cls)->read = _read;
  ((LStreamClass *) cls)->close = _close;
  ((LStreamClass *) cls)->eof = _eof;
}



/* l_file_fdopen:
 */
LStream *l_file_fdopen ( gint fd,
                         const gchar *mode,
                         GError **error )
{
  LStream *s;
  s = L_STREAM(l_object_new(L_CLASS_FILE, NULL));
  L_FILE(s)->fd = fd;
  L_FILE(s)->eof = FALSE;
  return s;
}



/* _write:
 */
static LStreamStatus _write ( LStream *stream,
                              gconstpointer buffer,
                              gint64 size,
                              gint64 *bytes_written,
                              GError **error )
{
  gint64 s;
  errno = 0;
  s = write(L_FILE(stream)->fd, buffer, size);
  if (s > 0)
    {
      if (bytes_written)
        *bytes_written = s;
      return L_STREAM_STATUS_OK;
    }
  else
    {
      if (errno == EAGAIN) {
        return L_STREAM_STATUS_AGAIN;
      } else {
        CL_ERROR("[TODO] write error");
        return -1;
      }
    }
}



/* _read:
 */
static LStreamStatus _read ( LStream *stream,
                             gpointer buffer,
                             gint64 size,
                             gint64 *bytes_read,
                             GError **error )
{
  gint64 s;
  errno = 0;
  if (((s = read(L_FILE(stream)->fd, buffer, size))) > 0)
    {
      *bytes_read = s;
      L_FILE(stream)->eof = FALSE;
      return L_STREAM_STATUS_OK;
    }
  else if (s == 0)
    {
      L_FILE(stream)->eof = TRUE;
      return L_STREAM_STATUS_EOF;
    }
  else
    {
      if (errno == EAGAIN) {
        return L_STREAM_STATUS_AGAIN;
      } else {
        CL_ERROR("[TODO] read error: %s", STRERROR);
        return -1;
      }
    }
}



/* _close:
 */
static void _close ( LStream *stream )
{
  if (close(L_FILE(stream)->fd) != 0)
    CL_ERROR("[TODO] close error");
}



/* _eof:
 */
static gboolean _eof ( LStream *stream )
{
  return L_FILE(stream)->eof;
}
