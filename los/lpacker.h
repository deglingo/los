/* lpacker.h -
 */

#ifndef _LPACKER_H_
#define _LPACKER_H_

#include "los/lobject.h"
#include "los/lstream.h"
#include "los/lpacker-def.h"



/* LPacker:
 */
struct _LPacker
{
  L_PACKER_INSTANCE_HEADER;

  LStream *stream;
  gpointer private;
};



/* LPackerClass:
 */
struct _LPackerClass
{
  L_PACKER_CLASS_HEADER;
};



LPacker *l_packer_new ( LStream *stream );
void l_packer_add ( LPacker *packer,
                    LObject *object );
gboolean l_packer_send ( LPacker *packer,
                         GError **error );



#endif
