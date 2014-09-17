/* lfile.c -
 */

#include "los/private.h"
#include "los/lfile.h"
#include "los/lfile.inl"
#include <unistd.h>



static LStreamStatus _write ( LStream *stream,
                              gpointer buffer,
                              gint64 size,
                              gint64 *bytes_written,
                              GError **error );
static LStreamStatus _read ( LStream *stream,
                             gpointer buffer,
                             gint64 size,
                             gint64 *bytes_read,
                             GError **error );
static void _close ( LStream *stream );



/* l_file_class_init:
 */
static void l_file_class_init ( LObjectClass *cls )
{
  ((LStreamClass *) cls)->write = _write;
  ((LStreamClass *) cls)->read = _read;
  ((LStreamClass *) cls)->close = _close;
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
  return s;
}



/* _write:
 */
static LStreamStatus _write ( LStream *stream,
                              gpointer buffer,
                              gint64 size,
                              gint64 *bytes_written,
                              GError **error )
{
  if (write(L_FILE(stream)->fd, buffer, size) != size)
    CL_ERROR("[TODO] write error");
  *bytes_written = size;
  return L_STREAM_STATUS_OK;
}



/* _read:
 */
static LStreamStatus _read ( LStream *stream,
                             gpointer buffer,
                             gint64 size,
                             gint64 *bytes_read,
                             GError **error )
{
  if (read(L_FILE(stream)->fd, buffer, size) != size)
    CL_ERROR("[TODO] read error");
  *bytes_read = size;
  return L_STREAM_STATUS_OK;
}



/* _close:
 */
static void _close ( LStream *stream )
{
  if (close(L_FILE(stream)->fd) != 0)
    CL_ERROR("[TODO] close error");
}
