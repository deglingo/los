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
};



/* LPackerClass:
 */
struct _LPackerClass
{
  L_PACKER_CLASS_HEADER;
};



LPacker *l_packer_new ( LStream *stream );



#endif
