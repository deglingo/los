/* lunpacker.h -
 */

#ifndef _LUNPACKER_H_
#define _LUNPACKER_H_

#include "los/lobject.h"
#include "los/lstream.h"
#include "los/lunpacker-def.h"



/* LUnpacker:
 */
struct _LUnpacker
{
  L_UNPACKER_INSTANCE_HEADER;
};



/* LUnpackerClass:
 */
struct _LUnpackerClass
{
  L_UNPACKER_CLASS_HEADER;
};



LUnpacker *l_unpacker_new ( LStream *stream );
LObject *l_unpacker_get ( LUnpacker *unpacker,
                          GError **error );



#endif
