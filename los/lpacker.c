/* lpacker.c -
 */

#include "los/private.h"
#include "los/lpacker.h"
#include "los/lpackprivate.h"
#include "los/lint.h" /* ?? */
#include "los/lstring.h"
#include "los/ltuple.h"
#include "los/ltrash.h"
#include "los/lpacker.inl"



#define BUFFER_SIZE 16
#define CONTEXT_STACK_SIZE 16



#define CONTEXT_HEADER \
  gint type; \
  LObject *object; \
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



/* ContextTuple:
 */
typedef struct _ContextTuple
{
  CONTEXT_HEADER;
  guint32 item;
}
  ContextTuple;



/* ContextObject:
 */
typedef struct _ContextObject
{
  CONTEXT_HEADER;
  guint32 clslen;
  const gchar *clsname;
}
  ContextObject;



/* Context:
 */
typedef union _Context
  {
    gint c_type;
    ContextAny c_any;
    ContextInt c_int;
    ContextTuple c_tuple;
    ContextObject c_object;
  }
  Context;



/* Private:
 */
typedef struct _Private
{
  gchar real_buffer[BUFFER_SIZE];
  const gchar *buffer;
  guint data_size;
  guint buffer_offset;
  GQueue *queue;
  Context context_stack[CONTEXT_STACK_SIZE];
  gint current_context;
}
  Private;

#define PRIVATE(p) ((Private *) (L_PACKER(p)->private))



enum
  {
    S_INT_START = 0,
    S_INT_WRITE_TYPE,
    S_INT_WRITE_VALUE,
  };



enum
  {
    S_STRING_START = 0,
    S_STRING_WRITE_TYPE,
    S_STRING_WRITE_LEN,
    S_STRING_WRITE_VALUE,
  };



enum
  {
    S_TUPLE_START = 0,
    S_TUPLE_WRITE_TYPE,
    S_TUPLE_WRITE_SIZE,
    S_TUPLE_WRITE_ITEM,
  };



enum
  {
    S_OBJECT_START = 0,
    S_OBJECT_WRITE_TYPE,
    S_OBJECT_WRITE_CLSLEN,
    S_OBJECT_WRITE_CLSNAME,
    S_OBJECT_WRITE_STATE,
    S_OBJECT_END,
  };



static void _dispose ( LObject *object );



/* l_packer_class_init:
 */
static void l_packer_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* l_packer_init:
 */
static void l_packer_init ( LObject *obj )
{
  L_PACKER(obj)->private = g_new0(Private, 1);
  PRIVATE(obj)->current_context = -1;
  PRIVATE(obj)->queue = g_queue_new();
}



/* l_packer_new:
 */
LPacker *l_packer_new ( LStream *stream )
{
  LPacker *p;
  p = L_PACKER(l_object_new(L_CLASS_PACKER, NULL));
  p->stream = l_object_ref(stream);
  return p;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  L_OBJECT_CLEAR(L_PACKER(object)->stream);
  if (PRIVATE(object))
    {
      g_queue_free(PRIVATE(object)->queue);
      g_free(PRIVATE(object));
      L_PACKER(object)->private = NULL;
    }
  /* [FIXME] */
  ((LObjectClass *) parent_class)->dispose(object);
}



gboolean _l_packer_put_int ( LPacker *packer,
                             LObject *object,
                             GError **error )
{
  gint64 w;
  guint8 tp = PACK_KEY_INT;
  gint32 val = GINT_TO_BE(L_INT_VALUE(object));
  /* write type */
  if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(guint8));
  /* write value */
  /* [FIXME] gint is not portable!! */
  if (l_stream_write(packer->stream, &val, sizeof(gint), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(gint));
  /* ok */
  return TRUE;
}



gboolean _l_packer_put_string ( LPacker *packer,
                                LObject *object,
                                GError **error )
{
  gint64 w;
  guint8 tp = PACK_KEY_STRING;
  /* [fixme] LString.len is guint!! */
  guint32 len = GUINT32_TO_BE((guint32)(L_STRING(object)->len));
  /* write type */
  if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(guint8));
  /* write len */
  if (l_stream_write(packer->stream, &len, sizeof(len), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(len));
  /* write string */
  if (l_stream_write(packer->stream, L_STRING(object)->str, L_STRING(object)->len, &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == L_STRING(object)->len);
  /* ok */
  return TRUE;
}



gboolean _l_packer_put_tuple ( LPacker *packer,
                               LObject *object,
                               GError **error )
{
  gint64 w;
  guint8 tp = PACK_KEY_TUPLE;
  guint32 nsize = GUINT32_TO_BE(L_TUPLE_SIZE(object));
  guint32 i;
  /* write type */
  if (l_stream_write(packer->stream, &tp, sizeof(guint8), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(guint8));
  /* write size */
  if (l_stream_write(packer->stream, &nsize, sizeof(nsize), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(nsize));
  /* write items */
  for (i = 0; i < L_TUPLE_SIZE(object); i++) {
    if (!l_packer_put(packer, L_TUPLE_ITEM(object, i), error))
      return FALSE;
  }
  /* ok */
  return TRUE;
}



gboolean _l_packer_put_object ( LPacker *packer,
                                LObject *object,
                                GError **error )
{
  guint8 tp = PACK_KEY_OBJECT;
  const gchar *clsname = l_object_class_name(L_OBJECT_GET_CLASS(object));
  guint32 clslen = strlen(clsname);
  guint32 clsnlen = GUINT32_TO_BE(clslen);
  LObject *state = l_object_get_state(object);
  gint64 w;
  gboolean r;
  /* write type */
  if (l_stream_write(packer->stream, &tp, sizeof(tp), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(tp));
  /* write clslen */
  if (l_stream_write(packer->stream, &clsnlen, sizeof(clsnlen), &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == sizeof(clsnlen));
  /* write clsname */
  if (l_stream_write(packer->stream, (gchar *) /* [FIXME] */ clsname, clslen, &w, error) != L_STREAM_STATUS_OK)
    return FALSE;
  ASSERT(w == clslen);
  /* write object */
  r = l_packer_put(packer, state, error);
  l_object_unref(state);
  return r;
}



/* l_packer_put:
 */
gboolean l_packer_put ( LPacker *packer,
                        LObject *object,
                        GError **error )
{
  gboolean r;
  /* l_trash_push(); */
  /* CL_DEBUG("PUT: %s", (gchar *) L_TRASH_GPOINTER(l_object_to_string(object))); */
  if (L_IS_INT(object)) {
    r = _l_packer_put_int(packer, object, error);
  } else if (L_IS_STRING(object)) {
    r = _l_packer_put_string(packer, object, error);
  } else if (L_IS_TUPLE(object)) {
    r = _l_packer_put_tuple(packer, object, error);
  } else {
    r = _l_packer_put_object(packer, object, error);
  }
  /* l_trash_pop(); */
  return r;
}



/* l_packer_add:
 */
void l_packer_add ( LPacker *packer,
                    LObject *object )
{
  g_queue_push_head(PRIVATE(packer)->queue, l_object_ref(object));
}



static gboolean _send ( LPacker *packer,
                        GError **error )
{
  Private *priv = PRIVATE(packer);
  gint64 w, size;
  while (priv->buffer_offset < priv->data_size)
    {
      LStreamStatus s;
      size = priv->data_size - priv->buffer_offset;
      s = l_stream_write(packer->stream,
                         priv->buffer + priv->buffer_offset,
                         size,
                         &w,
                         error);
      switch (s) {
      case L_STREAM_STATUS_OK:
        priv->buffer_offset += w;
        break;
      case L_STREAM_STATUS_AGAIN:
        return FALSE;
      default:
        CL_ERROR("[TODO] s = %d", s);
        return 0;
      }
    }
  return TRUE;
}



static Context *context_peek ( LPacker *packer )
{
  if (PRIVATE(packer)->current_context >= 0) {
    ASSERT(PRIVATE(packer)->current_context < CONTEXT_STACK_SIZE);
    return PRIVATE(packer)->context_stack + PRIVATE(packer)->current_context;
  } else {
    return NULL;
  }
}



static Context *context_push ( LPacker *packer,
                               LObject *object )
{
  Context *ctxt;
  /* CL_DEBUG("CTXTPUSH: %s", l_object_to_string(object)); */
  if (PRIVATE(packer)->current_context >= 0) {
    PRIVATE(packer)->current_context++;
    ASSERT(PRIVATE(packer)->current_context < CONTEXT_STACK_SIZE);
  } else {
    PRIVATE(packer)->current_context = 0;
  }
  ctxt = PRIVATE(packer)->context_stack + PRIVATE(packer)->current_context;
  /* [fixme] LObjectClass.pack_type */
  if (L_IS_INT(object)) ctxt->c_type = PACK_KEY_INT;
  else if (L_IS_STRING(object)) ctxt->c_type = PACK_KEY_STRING;
  else if (L_IS_TUPLE(object)) ctxt->c_type = PACK_KEY_TUPLE;
  else ctxt->c_type = PACK_KEY_OBJECT;
  ctxt->c_any.object = object;
  ctxt->c_any.stage = 0;
  return ctxt;
}



static void context_pop ( LPacker *packer )
{
  /* CL_DEBUG("CTXTPOP"); */
  ASSERT(PRIVATE(packer)->current_context >= 0);
  L_OBJECT_CLEAR(PRIVATE(packer)->context_stack[PRIVATE(packer)->current_context].c_any.object);
  PRIVATE(packer)->current_context--;
}



#define BUFFER_SET(priv, tp, val) do {          \
    ASSERT(sizeof(tp) <= BUFFER_SIZE);          \
    (priv)->buffer = (priv)->real_buffer;       \
    *((tp *)((priv)->buffer)) = (val);          \
    (priv)->data_size = sizeof(tp);             \
    (priv)->buffer_offset = 0;                  \
  } while (0)



#define BUFFER_DIVERT(priv, src, len) do {      \
    (priv)->buffer = (src);                     \
    (priv)->data_size = (len);                  \
    (priv)->buffer_offset = 0;                  \
  } while (0)



static gboolean _send_int ( LPacker *packer,
                            Context *ctxt,
                            GError **error )
{
  Private *priv = PRIVATE(packer);
  switch (ctxt->c_any.stage)
    {
    case S_INT_START:
      /* CL_DEBUG("pack_int_start"); */
      BUFFER_SET(priv, guint8, (guint8) PACK_KEY_INT);
      ctxt->c_any.stage = S_INT_WRITE_TYPE;
    case S_INT_WRITE_TYPE:
      {
        gint32 val;
        val = L_INT_VALUE(ctxt->c_any.object);
        if (!_send(packer, error))
          return FALSE;
        /* value */
        BUFFER_SET(priv, gint32, GINT32_TO_BE(val));
        ctxt->c_any.stage = S_INT_WRITE_VALUE;
      }
    case S_INT_WRITE_VALUE:
      if (!_send(packer, error))
        return FALSE;
    }
  /* CL_DEBUG("pack_int_ok"); */
  context_pop(packer);
  return TRUE;
}



static gboolean _send_string ( LPacker *packer,
                               Context *ctxt,
                               GError **error )
{
  Private *priv = PRIVATE(packer);
  switch (ctxt->c_any.stage)
    {
    case S_STRING_START:
      BUFFER_SET(priv, guint8, (guint8) PACK_KEY_STRING);
      ctxt->c_any.stage = S_STRING_WRITE_TYPE;
    case S_STRING_WRITE_TYPE:
      if (!_send(packer, error))
        return FALSE;
      /* string len */
      BUFFER_SET(priv, guint32, GUINT_TO_BE((guint32)(L_STRING(ctxt->c_any.object)->len)));
      ctxt->c_any.stage = S_STRING_WRITE_LEN;
    case S_STRING_WRITE_LEN:
      if (!_send(packer, error))
        return FALSE;
      /* string value */
      BUFFER_DIVERT(priv, L_STRING(ctxt->c_any.object)->str, L_STRING(ctxt->c_any.object)->len);
      ctxt->c_any.stage = S_STRING_WRITE_VALUE;
    case S_STRING_WRITE_VALUE:
      if (!_send(packer, error))
        return FALSE;
    }
  context_pop(packer);
  return TRUE;
}



static gboolean _send_tuple ( LPacker *packer,
                              Context *ctxt,
                              GError **error )
{
  Private *priv = PRIVATE(packer);
  switch (ctxt->c_any.stage)
    {
    case S_TUPLE_START:
      /* CL_DEBUG("pack_tuple_start"); */
      BUFFER_SET(priv, guint8, (guint8) PACK_KEY_TUPLE);
      ctxt->c_any.stage = S_TUPLE_WRITE_TYPE;
    case S_TUPLE_WRITE_TYPE:
      if (!_send(packer, error))
        return FALSE;
      /* size */
      BUFFER_SET(priv, guint32, GUINT32_TO_BE((guint32) L_TUPLE_SIZE(ctxt->c_any.object)));
      ctxt->c_any.stage = S_TUPLE_WRITE_SIZE;
    case S_TUPLE_WRITE_SIZE:
      if (!_send(packer, error))
        return FALSE;
      /* items */
      ctxt->c_tuple.item = 0;
      ctxt->c_tuple.stage = S_TUPLE_WRITE_ITEM;
    case S_TUPLE_WRITE_ITEM:
      if (L_TUPLE_SIZE(ctxt->c_tuple.object) > ctxt->c_tuple.item)
        {
          context_push(packer, l_object_ref(L_TUPLE_ITEM(ctxt->c_tuple.object, ctxt->c_tuple.item++)));
          return TRUE;
        }
      else
        {
          break;
        }
    default:
      ASSERT(0);
    }
  /* CL_DEBUG("pack_tuple_ok"); */
  context_pop(packer);
  return TRUE;
}



static gboolean _send_object ( LPacker *packer,
                               Context *ctxt,
                               GError **error )
{
  Private *priv = PRIVATE(packer);
  switch (ctxt->c_any.stage)
    {
    case S_OBJECT_START:
      BUFFER_SET(priv, guint8, (guint8) PACK_KEY_OBJECT);
      ctxt->c_any.stage = S_OBJECT_WRITE_TYPE;
    case S_OBJECT_WRITE_TYPE:
      if (!_send(packer, error))
        return FALSE;
      /* clslen */
      ctxt->c_object.clsname = l_object_class_name(L_OBJECT_GET_CLASS(ctxt->c_any.object));
      ctxt->c_object.clslen = strlen(ctxt->c_object.clsname);
      /* CL_DEBUG("clsname: '%s' (%d)", ctxt->c_object.clsname, ctxt->c_object.clslen); */
      BUFFER_SET(priv, guint32, GUINT32_TO_BE(ctxt->c_object.clslen));
      ctxt->c_any.stage = S_OBJECT_WRITE_CLSLEN;
    case S_OBJECT_WRITE_CLSLEN:
      if (!_send(packer, error))
        return FALSE;
      /* clsname */
      BUFFER_DIVERT(priv, ctxt->c_object.clsname, ctxt->c_object.clslen);
      ctxt->c_any.stage = S_OBJECT_WRITE_CLSNAME;
    case S_OBJECT_WRITE_CLSNAME:
      if (!_send(packer, error))
        return FALSE;
      /* state */
      {
        LObject *state = l_object_get_state(ctxt->c_any.object);
        ASSERT(state);
        /* CL_DEBUG("state: %s", l_object_to_string(state)); */
        ctxt->c_any.stage = S_OBJECT_END;
        /* this ref will be dropped by context_pop() */
        context_push(packer, state);
        return TRUE;
      }
    case S_OBJECT_END:
      break;
    default:
      ASSERT(0);
    }
  context_pop(packer);
  return TRUE;
}



/* l_packer_send:
 */
gboolean l_packer_send ( LPacker *packer,
                         GError **error )
{
  Private *priv = PRIVATE(packer);
  Context *ctxt;
  while (1)
    {
      /* if there is no current context, get the next object and push
         a new context */
      if (!(ctxt = context_peek(packer)))
        {
          LObject *obj = g_queue_pop_tail(priv->queue);
          /* no more objects in the queue: all done */
          if (!obj)
            return TRUE;
          /* init a new context */
          ctxt = context_push(packer, obj);
        }
      /* then process it */
      switch (ctxt->c_type) {
      case PACK_KEY_INT:
        if (!_send_int(packer, ctxt, error))
          return FALSE;
        break;
      case PACK_KEY_STRING:
        if (!_send_string(packer, ctxt, error))
          return FALSE;
        break;
      case PACK_KEY_TUPLE:
        if (!_send_tuple(packer, ctxt, error))
          return FALSE;
        break;
      case PACK_KEY_OBJECT:
        if (!_send_object(packer, ctxt, error))
          return FALSE;
        break;
      default:
        ASSERT(0);
      }
    }
}
