/* lmemstream.h -
 */

#ifndef _LMEMSTREAM_H_
#define _LMEMSTREAM_H_

#include "los/lstream.h"
#include "los/lmemstream-def.h"



/* LMemStream:
 */
struct _LMemStream
{
  L_MEM_STREAM_INSTANCE_HEADER;

  gpointer buffer;
  gint64 buffer_size;
  gint64 data_size;
  gint64 pos;
};



/* LMemStreamClass:
 */
struct _LMemStreamClass
{
  L_MEM_STREAM_CLASS_HEADER;
};



LStream *l_mem_stream_new ( const gchar *content,
                            gint64 size );
gpointer l_mem_stream_get_buffer ( LMemStream *stream,
                                   gint64 *len );



#endif
