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



/* Context:
 */
typedef union _Context
{
  gint c_type;
  ContextAny c_any;
  ContextInt c_int;
}
  Context;



enum
  {
    S_INT_START = 0,
    S_INT_READ_VALUE,
  };



/* Private:
 */
typedef struct _Private
{
  Context context_stack[CONTEXT_STACK_SIZE];
  gint current_context;
  gchar buffer[BUFFER_SIZE];
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



#define BUFFER_SET(priv, tp) do {               \
    ASSERT(sizeof(tp) <= BUFFER_SIZE);          \
    (priv)->data_size = sizeof(tp);             \
    (priv)->buffer_offset = 0;                  \
  } while (0)



static LObject *_recv_int ( LUnpacker *unpacker,
                           Context *ctxt,
                           GError **error )
{
  Private *priv = PRIVATE(unpacker);
  switch (ctxt->c_int.stage)
    {
    case S_INT_START:
      BUFFER_SET(priv, gint32);
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
      BUFFER_SET(priv, guint8);
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
      return L_OBJECT(l_string_new("test-string-1"));
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
