/* lunpacker.h -
 */

#ifndef _LUNPACKER_H_
#define _LUNPACKER_H_

#include "los/lobject.h"
#include "los/lpackerbase.h"
#include "los/lstream.h"
#include "los/lunpacker-def.h"



/* LUnpacker:
 */
struct _LUnpacker
{
  L_UNPACKER_INSTANCE_HEADER;

  gpointer private;
  LStream *stream;
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
LObject *l_unpacker_recv ( LUnpacker *unpacker,
                           GError **error );



#endif
