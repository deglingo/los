/* lstream.h -
 */

#ifndef _LSTREAM_H_
#define _LSTREAM_H_

#include "los/lobject.h"
#include "los/lstream-def.h"



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
};



void l_stream_seek ( LStream *stream,
                     glong offset,
                     LStreamSeekType whence );




#endif
