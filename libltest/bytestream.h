/* bytestream.h -
 */

#ifndef _BYTESTREAM_H_
#define _BYTESTREAM_H_

#include "los/lstream.h"
#include "libltest/bytestream-def.h"



/* ByteStream:
 */
struct _ByteStream
{
  BYTE_STREAM_INSTANCE_HEADER;

  FILE *f;
  gboolean ready;
};



/* ByteStreamClass:
 */
struct _ByteStreamClass
{
  BYTE_STREAM_CLASS_HEADER;
};



LStream *byte_stream_new ( const gchar *fname,
                           const gchar *mode );



#endif
