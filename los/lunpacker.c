/* lunpacker.c -
 */

#include "los/private.h"
#include "los/lunpacker.h"
#include "los/lpackprivate.h"
#include "los/lint.h" /* ?? */
#include "los/lstring.h" /* ?? */
#include "los/ltuple.h"
#include "los/lunpacker.inl"



#define BUFFER_SIZE 16
#define CONTEXT_STACK_SIZE 16

#define CONTEXT_HEADER \
  gint type; \
  gint stage



/* ContextAny:
 */
typedef struct _ContextAny
{
  CONTEXT_HEADER;
}
  ContextAny;



/* ContextInt:
 */
typedef struct _ContextInt
{
  CONTEXT_HEADER;
}
  ContextInt;



/* ContextString:
 */
typedef struct _ContextString
{
  CONTEXT_HEADER;
  guint32 len;
}
  ContextString;



/* Context:
 */
typedef union _Context
{
  gint c_type;
  ContextAny c_any;
  ContextInt c_int;
  ContextString c_string;
}
  Context;



enum
  {
    S_INT_START = 0,
    S_INT_READ_VALUE,
  };



enum
  {
    S_STRING_START = 0,
    S_STRING_READ_LEN,
    S_STRING_READ_VALUE,
  };



/* Private:
 */
typedef struct _Private
{
  Context context_stack[CONTEXT_STACK_SIZE];
  gint current_context;
  gchar *buffer;
  guint buffer_size;
  guint data_size;
  guint buffer_offset;
}
  Private;

#define PRIVATE(u) ((Private *) (L_UNPACKER(u)->private))



static void _dispose ( LObject *object );



/* l_unpacker_class_init:
 */
static void l_unpacker_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* l_unpacker_init:
 */
static void l_unpacker_init ( LObject *obj )
{
  L_UNPACKER(obj)->private = g_new0(Private, 1);
  PRIVATE(obj)->current_context = -1;
}



/* l_unpacker_new:
 */
LUnpacker *l_unpacker_new ( LStream *stream )
{
  LUnpacker *u;
  u = L_UNPACKER(l_object_new(L_CLASS_UNPACKER, NULL));
  u->stream = l_object_ref(stream);
  return u;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  L_OBJECT_CLEAR(L_UNPACKER(object)->stream);
  g_free(PRIVATE(object)->buffer);
  PRIVATE(object)->buffer = NULL;
  PRIVATE(object)->buffer_size = 0;
  g_free(L_UNPACKER(object)->private);
  L_UNPACKER(object)->private = NULL;
  /* [FIXME] */
  ((LObjectClass *) parent_class)->dispose(object);
}



LObject *_l_unpacker_get_int ( LUnpacker *unpacker,
                              GError **error )
{
  gint val;
  gint64 w;
  if (l_stream_read(unpacker->stream, &val, sizeof(gint), &w, error) != L_STREAM_STATUS_OK)
    return NULL;
  ASSERT(w == sizeof(gint));
  return L_OBJECT(l_int_new(GINT_FROM_BE(val)));
}



LObject *_l_unpacker_get_string ( LUnpacker *unpacker,
                                  GError **error )
{
  guint32 nlen, len;
  gint64 w;
  gchar *str;
  LString *lstr;
  /* read len */
  if (l_stream_read(unpacker->stream, &nlen, sizeof(nlen), &w, error) != L_STREAM_STATUS_OK)
    return NULL;
  ASSERT(w == sizeof(nlen));
  len = GUINT32_FROM_BE(nlen);
  /* read string */
  str = g_malloc(len+1);
  if (l_stream_read(unpacker->stream, str, len, &w, error) != L_STREAM_STATUS_OK) {
    g_free(str);
    return NULL;
  }
  ASSERT(w == len);
  str[len] = 0;
  /* create the LString */
  /* [FIXME] we should have a way to give str without copying it */
  lstr = l_string_new(str, -1);
  g_free(str);
  /* ok */
  return L_OBJECT(lstr);
}



LObject *_l_unpacker_get_tuple ( LUnpacker *unpacker,
                                 GError **error )
{
  guint32 size, nsize, i;
  gint64 w;
  LTuple *t;
  /* get size */
  if (l_stream_read(unpacker->stream, &nsize, sizeof(nsize), &w, error) != L_STREAM_STATUS_OK)
    return NULL;
  size = GUINT32_FROM_BE(nsize);
  /* create the tuple */
  t = l_tuple_new(size);
  /* get the items */
  for (i = 0; i < size; i++) {
    LObject *item;
    if (!(item = l_unpacker_get(unpacker, error))) {
      l_object_unref(t);
      return NULL;
    }
    l_tuple_give_item(t, i, item);
  }
  /* ok */
  return L_OBJECT(t);
}



LObject *_l_unpacker_get_object ( LUnpacker *unpacker,
                                 GError **error )
{
  guint32 clsnlen, clslen;
  gchar *clsname;
  LObjectClass *cls;
  LObject *state, *obj;
  gint64 w;
  /* get class name */
  if (l_stream_read(unpacker->stream, &clsnlen, sizeof(clsnlen), &w, error) != L_STREAM_STATUS_OK)
    return NULL;
  ASSERT(w == sizeof(clsnlen));
  clslen = GUINT32_FROM_BE(clsnlen);
  clsname = g_malloc(clslen + 1);
  if (l_stream_read(unpacker->stream, clsname, clslen, &w, error) != L_STREAM_STATUS_OK)
    return NULL;
  ASSERT(w == clslen);
  clsname[clslen] = 0;
  /* get the class */
  cls = l_object_class_from_name(clsname);
  ASSERT(cls);
  g_free(clsname);
  /* get the state */
  state = l_unpacker_get(unpacker, error);
  ASSERT(state);
  obj = l_object_new_from_state(cls, state);
  ASSERT(obj);
  l_object_unref(state);
  return obj;
}



/* l_unpacker_get:
 */
LObject *l_unpacker_get ( LUnpacker *unpacker,
                          GError **error )
{
  guint8 t;
  GError *err = NULL;
  gint64 r;
  LStreamStatus s;
  /* read tp */
  s = l_stream_read(unpacker->stream, &t, sizeof(guint8), &r, &err);
  switch (s) {
  case L_STREAM_STATUS_OK:
    break;
  case L_STREAM_STATUS_EOF:
    g_set_error(error, L_PACK_ERROR, L_PACK_ERROR_EOF, "no more object to read");
    return NULL;
  default:
    CL_ERROR("[TODO] read error");
  }
  ASSERT(r == sizeof(guint8));
  if (t == PACK_KEY_INT) { /* [FIXME] */
    return _l_unpacker_get_int(unpacker, error);
  } else if (t == PACK_KEY_STRING) {
    return _l_unpacker_get_string(unpacker, error);
  } else if (t == PACK_KEY_TUPLE) {
    return _l_unpacker_get_tuple(unpacker, error);
  } else if (t == PACK_KEY_OBJECT) {
    return _l_unpacker_get_object(unpacker, error);
  } else {
    CL_ERROR("[TODO] tp %d", t);
    return NULL;
  }
}



static gboolean _recv ( LUnpacker *unpacker,
                        GError **error )
{
  Private *priv = PRIVATE(unpacker);
  LStreamStatus s;
  gint64 size, w;
  size = priv->data_size - priv->buffer_offset;
  ASSERT(size >= 0);
  if (size == 0) /* just in case ? */
    return TRUE;
  s = l_stream_read(unpacker->stream,
                    priv->buffer + priv->buffer_offset,
                    size,
                    &w,
                    error);
  switch (s) {
  case L_STREAM_STATUS_OK:
    priv->buffer_offset += w;
    ASSERT(priv->buffer_offset <= priv->data_size);
    if (priv->buffer_offset == priv->data_size)
      return TRUE;
    else
      return FALSE;
  case L_STREAM_STATUS_AGAIN:
    return FALSE;
  default:
    CL_ERROR("[TODO] s = %d", s);
    return FALSE;
  }
}



#define BUFFER_SET(priv, size) do {                                     \
    if ((size) > (priv)->buffer_size) {                                 \
      while ((size) > (priv)->buffer_size)                              \
        (priv)->buffer_size = (priv)->buffer_size ? ((priv)->buffer_size * 2) : 64; \
      (priv)->buffer = g_realloc((priv)->buffer, (priv)->buffer_size);  \
    }                                                                   \
    (priv)->data_size = size;                                           \
    (priv)->buffer_offset = 0;                                          \
  } while (0)



static LObject *_recv_int ( LUnpacker *unpacker,
                           Context *ctxt,
                           GError **error )
{
  Private *priv = PRIVATE(unpacker);
  switch (ctxt->c_int.stage)
    {
    case S_INT_START:
      BUFFER_SET(priv, sizeof(gint32));
      ctxt->c_int.stage = S_INT_READ_VALUE;
    case S_INT_READ_VALUE:
      if (!_recv(unpacker, error))
        return NULL;
      break;
    default:
      ASSERT(0);
    }
  return L_OBJECT(l_int_new(GINT32_FROM_BE(*((gint32 *)(priv->buffer)))));
}



static LObject *_recv_string ( LUnpacker *unpacker,
                               Context *ctxt,
                               GError **error )
{
  Private *priv = PRIVATE(unpacker);
  switch (ctxt->c_string.stage)
    {
    case S_STRING_START:
      BUFFER_SET(priv, sizeof(guint32));
      ctxt->c_string.stage = S_STRING_READ_LEN;
    case S_STRING_READ_LEN:
      if (!_recv(unpacker, error))
        return NULL;
      ctxt->c_string.len = GUINT32_FROM_BE(*((guint32 *)(priv->buffer)));
      /* fprintf(stderr, "\nLEN=%d\n", ctxt->c_string.len); */
      BUFFER_SET(priv, ctxt->c_string.len);
      ctxt->c_string.stage = S_STRING_READ_VALUE;
    case S_STRING_READ_VALUE:
      if (!_recv(unpacker, error))
        return NULL;
      break;
    default:
      ASSERT(0);
    }
  return L_OBJECT(l_string_new(priv->buffer, ctxt->c_string.len));
}



static LObject *_recv_tuple ( LUnpacker *unpacker,
                              Context *ctxt,
                              GError **error )
{
  return L_OBJECT(l_tuple_new(0));
}



/* l_unpacker_recv:
 */
LObject *l_unpacker_recv ( LUnpacker *unpacker,
                           GError **error )
{
  Private *priv = PRIVATE(unpacker);
  Context *ctxt;
  /* get context */
  if (priv->current_context < 0)
    {
      priv->current_context = 0;
      ctxt = priv->context_stack + priv->current_context;
      ctxt->c_type = -1;
      BUFFER_SET(priv, sizeof(guint8));
    }
  else
    {
      ctxt = priv->context_stack + priv->current_context;
    }
  /* get type */
  if (ctxt->c_type < 0)
    {
      if (!_recv(unpacker, error))
        return NULL;
      ctxt->c_type = *((guint8 *)(priv->buffer));
      ctxt->c_any.stage = 0;
    }
  /* get object */
  switch (ctxt->c_type)
    {
    case PACK_KEY_INT:
      return  _recv_int(unpacker, ctxt, error);
    case PACK_KEY_STRING:
      return _recv_string(unpacker, ctxt, error);
    case PACK_KEY_TUPLE:
      return _recv_tuple(unpacker, ctxt, error);
    default:
      CL_ERROR("[TODO] type = %d", ctxt->c_type);
    }
  return NULL;

  /* if (!priv->running) */
  /*   { */
  /*     priv->data_size = sizeof(gint32); */
  /*     priv->buffer_offset = 0; */
  /*     priv->running = 1; */
  /*   } */
  /* return _recv(unpacker, error); */
}
