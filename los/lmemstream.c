/* lmemstream.c -
 */

#include "los/private.h"
#include "los/lmemstream.h"
#include "los/lmemstream.inl"

#include <string.h>



#define DEFAULT_BUFFER_SIZE (1024)

static void _write ( LStream *stream,
                     gpointer buffer,
                     gint64 size,
                     GError **error );
static gint64 _read ( LStream *stream,
                      gpointer buffer,
                      gint64 size,
                      GError **error );



/* _grow_buffer:
 */
static inline void _grow_buffer ( LMemStream *m,
                                  gint64 size )
{
  if (size > m->buffer_size) {
    while (size > m->buffer_size)
      m->buffer_size = (m->buffer_size ? (m->buffer_size * 2) : DEFAULT_BUFFER_SIZE);
    m->buffer = g_realloc(m->buffer, m->buffer_size);
  }
}



/* l_mem_stream_class_init:
 */
static void l_mem_stream_class_init ( LObjectClass *cls )
{
  ((LStreamClass *) cls)->write = _write;
  ((LStreamClass *) cls)->read = _read;
}



/* l_mem_stream_new:
 */
LStream *l_mem_stream_new ( const gchar *content,
                            gint64 size )
{
  LMemStream *m;
  m = L_MEM_STREAM(l_object_new(L_CLASS_MEM_STREAM, NULL));
  if (content)
    {
      if (size < 0)
        size = strlen(content);
      if (size > 0) {
        _grow_buffer(m, size);
        memcpy(m->buffer, content, size);
        m->data_size = size;
      }
    }
  else
    {
      ASSERT(size <= 0);
    }
  return L_STREAM(m);
}



/* l_mem_stream_get_buffer:
 */
gpointer l_mem_stream_get_buffer ( LMemStream *stream,
                                   gint64 *len )
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
                     gint64 size,
                     GError **error )
{
#define m (L_MEM_STREAM(stream))
  gint64 pos2 = m->pos + size;
  gint64 new_size = MAX(pos2, m->data_size);
  _grow_buffer(m, new_size);
  memcpy(m->buffer + m->pos, buffer, size);
  m->pos = pos2;
  m->data_size = new_size;
#undef m
}



/* _read:
 */
static gint64 _read ( LStream *stream,
                      gpointer buffer,
                      gint64 size,
                      GError **error )
{
#define m (L_MEM_STREAM(stream))
  gint64 pos2 = m->pos + size;
  ASSERT(pos2 <= m->data_size); /* [TODO] */
  memcpy(buffer, m->buffer + m->pos, size);
  m->pos = pos2;
  return size;
#undef m
}
