/* lstream.h -
 */

#ifndef _LSTREAM_H_
#define _LSTREAM_H_

#include "los/lobject.h"
#include "los/lstream-def.h"



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



#endif
