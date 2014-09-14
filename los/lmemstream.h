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
};



/* LMemStreamClass:
 */
struct _LMemStreamClass
{
  L_MEM_STREAM_CLASS_HEADER;
};



LStream *l_mem_stream_new ( const gchar *content,
                            gsize size );



#endif