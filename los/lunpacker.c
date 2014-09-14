/* lunpacker.c -
 */

#include "los/lunpacker.h"
#include "los/lunpacker.inl"



/* l_unpacker_new:
 */
LUnpacker *l_unpacker_new ( LStream *stream )
{
  LUnpacker *u;
  u = L_UNPACKER(l_object_new(L_CLASS_UNPACKER, NULL));
  return u;
}
