/* lmemstream.c -
 */

#include "los/lmemstream.h"
#include "los/lmemstream.inl"

#include <string.h>



#define DEFAULT_BUFFER_SIZE (1024)

static void _write ( LStream *stream,
                     gpointer buffer,
                     gsize size,
                     GError **error );



/* l_mem_stream_class_init:
 */
static void l_mem_stream_class_init ( LObjectClass *cls )
{
  ((LStreamClass *) cls)->write = _write;
}



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
  if ((*len = stream->data_size))
    return stream->buffer;
  else
    return NULL;
}



/* _write:
 */
static void _write ( LStream *stream,
                     gpointer buffer,
                     gsize size,
                     GError **error )
{
#define m (L_MEM_STREAM(stream))
  gsize pos2 = m->pos + size;
  gsize new_size = MAX(pos2, m->data_size);
  if (new_size > m->buffer_size) {
    while (new_size > m->buffer_size)
      m->buffer_size = (m->buffer_size ? (m->buffer_size * 2) : DEFAULT_BUFFER_SIZE);
    m->buffer = g_realloc(m->buffer, m->buffer_size);
  }
  memcpy(m->buffer + m->pos, buffer, size);
  m->pos = pos2;
  m->data_size = new_size;
#undef m
}
