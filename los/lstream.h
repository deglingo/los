/* lstream.h -
 */

#ifndef _LSTREAM_H_
#define _LSTREAM_H_

#include "los/lobject.h"
#include "los/lstream-def.h"



/* LStreamStatus:
 */
typedef enum _LStreamStatus
  {
    L_STREAM_STATUS_OK,
    L_STREAM_STATUS_EOF,
    L_STREAM_STATUS_AGAIN,
  }
  LStreamStatus;



/* LStreamSeekType:
 */
typedef enum _LStreamSeekType
  {
    L_STREAM_SEEK_SET,
    L_STREAM_SEEK_CUR,
    L_STREAM_SEEK_END,
  }
  LStreamSeekType;



/* LStream:
 */
struct _LStream
{
  L_STREAM_INSTANCE_HEADER;
};



/* LStreamClass:
 */
struct _LStreamClass
{
  L_STREAM_CLASS_HEADER;

  LStreamStatus (* write) ( LStream *stream,
                            gconstpointer buffer,
                            gint64 size,
                            gint64 *bytes_written,
                            GError **error );

  LStreamStatus (* read) ( LStream *stream,
                           gpointer buffer,
                           gint64 size,
                           gint64 *bytes_read,
                           GError **error );

  void (* seek) ( LStream *stream,
                  gint64 offset,
                  LStreamSeekType whence );

  void (* close) ( LStream *stream );

  gboolean (* eof) ( LStream *stream );
};



LStreamStatus l_stream_write ( LStream *stream,
                               gconstpointer buffer,
                               gint64 size,
                               gint64 *bytes_written,
                               GError **error );
LStreamStatus l_stream_read ( LStream *stream,
                              gpointer buffer,
                              gint64 size,
                              gint64 *bytes_read,
                              GError **error );
void l_stream_seek ( LStream *stream,
                     gint64 offset,
                     LStreamSeekType whence );
void l_stream_close ( LStream *stream );
gboolean l_stream_eof ( LStream *stream );




#endif
