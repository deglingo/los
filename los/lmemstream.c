/* lmemstream.c -
 */

#include "los/lmemstream.h"
#include "los/lmemstream.inl"



/* l_mem_stream_new:
 */
LStream *l_mem_stream_new ( const gchar *content,
                            gsize size )
{
  LStream *s;
  s = L_STREAM(l_object_new(L_CLASS_MEM_STREAM, NULL));
  return s;
}



/* l_mem_stream_get_buffer:
 */
gpointer l_mem_stream_get_buffer ( LMemStream *stream,
                                   gsize *len )
{
  *len = 0;
  return NULL;
}
