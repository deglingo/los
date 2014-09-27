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
  gint stage; \
  LObject *object; \
  gboolean done



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



/* ContextTuple:
 */
typedef struct _ContextTuple
{
  CONTEXT_HEADER;
  guint size;
  guint item;
}
  ContextTuple;



/* ContextObject:
 */
typedef struct _ContextObject
{
  CONTEXT_HEADER;
  guint32 clslen;
  LObjectClass *cls;
}
  ContextObject;



/* Context:
 */
typedef union _Context
{
  gint c_type;
  ContextAny c_any;
  ContextInt c_int;
  ContextString c_string;
  ContextTuple c_tuple;
  ContextObject c_object;
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



enum
  {
    S_TUPLE_START = 0,
    S_TUPLE_READ_SIZE,
    S_TUPLE_READ_ITEM,
  };



enum
  {
    S_OBJECT_START = 0,
    S_OBJECT_READ_CLSLEN,
    S_OBJECT_READ_CLSNAME,
    S_OBJECT_READ_STATE,
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
  GPtrArray *object_stack;
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
  PRIVATE(obj)->object_stack = g_ptr_array_new();
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
  if (PRIVATE(object))
    {
      g_free(PRIVATE(object)->buffer);
      PRIVATE(object)->buffer = NULL;
      PRIVATE(object)->buffer_size = 0;
      g_ptr_array_unref(PRIVATE(object)->object_stack);
      g_free(L_UNPACKER(object)->private);
      L_UNPACKER(object)->private = NULL;
    }
  /* [FIXME] */
  ((LObjectClass *) parent_class)->dispose(object);
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
  case L_STREAM_STATUS_EOF:
    g_set_error(error, L_PACK_ERROR, L_PACK_ERROR_EOF, "EOF reached");
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



static Context *context_peek ( LUnpacker *unpacker )
{
  if (PRIVATE(unpacker)->current_context < 0)
    return NULL;
  else
    return PRIVATE(unpacker)->context_stack + PRIVATE(unpacker)->current_context;
}



static Context *context_push ( LUnpacker *unpacker )
{
  Context *ctxt;
  /* fprintf(stderr, "\nPUSH\n"); */
  if (PRIVATE(unpacker)->current_context < 0)
    {
      PRIVATE(unpacker)->current_context = 0;
    }
  else
    {
      PRIVATE(unpacker)->current_context++;
      ASSERT(PRIVATE(unpacker)->current_context < CONTEXT_STACK_SIZE);
    }
  ctxt = PRIVATE(unpacker)->context_stack + PRIVATE(unpacker)->current_context;
  ctxt->c_type = -1;
  ctxt->c_any.object = NULL;
  ctxt->c_any.done = FALSE;
  BUFFER_SET(PRIVATE(unpacker), sizeof(guint8));
  return ctxt;
}



static void context_pop ( LUnpacker *unpacker )
{
  /* fprintf(stderr, "\nPOP\n"); */
  ASSERT(PRIVATE(unpacker)->current_context >= 0);
  PRIVATE(unpacker)->current_context--;
}



static void object_push ( LUnpacker *unpacker,
                          LObject *object )
{
  /* CL_DEBUG("OBJPUSH: %s", l_object_to_string(object)); */
  g_ptr_array_add(PRIVATE(unpacker)->object_stack, object);
}



static LObject *object_pop ( LUnpacker *unpacker )
{
  ASSERT(PRIVATE(unpacker)->object_stack->len > 0);
  /* CL_DEBUG("OBJPOP"); */
  return L_OBJECT(g_ptr_array_remove_index(PRIVATE(unpacker)->object_stack,
                                           PRIVATE(unpacker)->object_stack->len - 1));
}



static gboolean _recv_int ( LUnpacker *unpacker,
                            Context *ctxt,
                            GError **error )
{
  Private *priv = PRIVATE(unpacker);
  /* fprintf(stderr, "\nRECV_INT(%d)\n", ctxt->c_any.stage); */
  switch (ctxt->c_int.stage)
    {
    case S_INT_START:
      /* CL_DEBUG("int-start"); */
      BUFFER_SET(priv, sizeof(gint32));
      ctxt->c_int.stage = S_INT_READ_VALUE;
    case S_INT_READ_VALUE:
      if (!_recv(unpacker, error))
        return FALSE;
      break;
    default:
      CL_ERROR("stage = %d", ctxt->c_int.stage);
    }
  ctxt->c_any.object = L_OBJECT(l_int_new(GINT32_FROM_BE(*((gint32 *)(priv->buffer)))));
  /* CL_DEBUG("int-ok: %s", l_object_to_string(ctxt->c_any.object)); */
  ctxt->c_any.done = TRUE;
  return TRUE;
}



static gboolean _recv_string ( LUnpacker *unpacker,
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
        return FALSE;
      ctxt->c_string.len = GUINT32_FROM_BE(*((guint32 *)(priv->buffer)));
      /* fprintf(stderr, "\nLEN=%d\n", ctxt->c_string.len); */
      BUFFER_SET(priv, ctxt->c_string.len);
      ctxt->c_string.stage = S_STRING_READ_VALUE;
    case S_STRING_READ_VALUE:
      if (!_recv(unpacker, error))
        return FALSE;
      break;
    default:
      ASSERT(0);
    }
  ctxt->c_any.object = L_OBJECT(l_string_new_len(priv->buffer, ctxt->c_string.len));
  ctxt->c_any.done = TRUE;
  return TRUE;
}



static gboolean _recv_tuple ( LUnpacker *unpacker,
                              Context *ctxt,
                              GError **error )
{
  Private *priv = PRIVATE(unpacker);
  switch (ctxt->c_any.stage)
    {
    case S_TUPLE_START:
      /* CL_DEBUG("tuple-start"); */
      BUFFER_SET(priv, sizeof(guint32));
      ctxt->c_tuple.stage = S_TUPLE_READ_SIZE;
    case S_TUPLE_READ_SIZE:
      if (!_recv(unpacker, error))
        return FALSE;
      ctxt->c_tuple.size = GUINT32_FROM_BE(*((guint32 *)(priv->buffer)));
      ctxt->c_tuple.object = L_OBJECT(l_tuple_new(ctxt->c_tuple.size));
      ctxt->c_tuple.item = 0;
      ctxt->c_tuple.stage = S_TUPLE_READ_ITEM;
    case S_TUPLE_READ_ITEM:
      if (ctxt->c_tuple.item > 0)
        {
          l_tuple_give_item(L_TUPLE(ctxt->c_tuple.object),
                            ctxt->c_tuple.item - 1,
                            object_pop(unpacker));
        }
      if (ctxt->c_tuple.item < ctxt->c_tuple.size)
        {
          ctxt->c_tuple.item++;
          context_push(unpacker);
          return TRUE;
        }
      else
        {
          break;
        }
      break;
    default:
      ASSERT(0);
    }
  /* CL_DEBUG("tuple-ok (%d)", L_TUPLE_SIZE(ctxt->c_any.object)); */
  ctxt->c_any.done = TRUE;
  return TRUE;
}



static gboolean _recv_object ( LUnpacker *unpacker,
                               Context *ctxt,
                               GError **error )
{
  Private *priv = PRIVATE(unpacker);
  /* CL_DEBUG("RECVOBJ(%d)", ctxt->c_any.stage); */
  switch (ctxt->c_any.stage)
    {
    case S_OBJECT_START:
      BUFFER_SET(priv, sizeof(guint32));
      ctxt->c_any.stage = S_OBJECT_READ_CLSLEN;
    case S_OBJECT_READ_CLSLEN:
      if (!_recv(unpacker, error))
        return FALSE;
      ctxt->c_object.clslen = GUINT32_FROM_BE(*((guint32 *)(priv->buffer)));
      /* CL_DEBUG("clslen: %d", ctxt->c_object.clslen); */
      /* [fixme] give one more byte for the trailing 0, but we don't
         want to read it */
      BUFFER_SET(priv, ctxt->c_object.clslen+1);
      BUFFER_SET(priv, ctxt->c_object.clslen);
      ctxt->c_any.stage = S_OBJECT_READ_CLSNAME;
    case S_OBJECT_READ_CLSNAME:
      if (!_recv(unpacker, error))
        return FALSE;
      priv->buffer[ctxt->c_object.clslen] = 0;
      /* CL_DEBUG("clsname: '%s'", (gchar *) priv->buffer); */
      ctxt->c_object.cls = l_object_class_from_name(priv->buffer);
      ASSERT(ctxt->c_object.cls);
      l_object_ref(ctxt->c_object.cls);
      ctxt->c_any.stage = S_OBJECT_READ_STATE;
      context_push(unpacker);
      return TRUE;
    case S_OBJECT_READ_STATE:
      {
        LObject *state = object_pop(unpacker);
        /* CL_DEBUG("state: %s", l_object_to_string(state)); */
        ctxt->c_any.object = l_object_new_from_state(ctxt->c_object.cls, state);
        l_object_unref(ctxt->c_object.cls);
        l_object_unref(state);
        ctxt->c_any.done = TRUE;
        return TRUE;
      }
    default:
      CL_ERROR("stage = %d", ctxt->c_any.stage);
      return FALSE;
    }
}



/* l_unpacker_recv:
 */
LObject *l_unpacker_recv ( LUnpacker *unpacker,
                           GError **error )
{
  Private *priv = PRIVATE(unpacker);
  Context *ctxt;
  while (1)
    {
      /* get context */
      if (!(ctxt = context_peek(unpacker)))
        {
          ctxt = context_push(unpacker);
        }
      /* get type */
      if (ctxt->c_type < 0)
        {
          if (!_recv(unpacker, error))
            return NULL;
          ctxt->c_type = *((guint8 *)(priv->buffer));
          /* CL_DEBUG("TYPE: %d", ctxt->c_type); */
          /* fprintf(stderr, "\nRECV: %d\n", ctxt->c_type); */
          ctxt->c_any.stage = 0;
        }
      /* get object */
      switch (ctxt->c_type)
        {
        case PACK_KEY_INT:
          if (!_recv_int(unpacker, ctxt, error))
            return NULL;
          break;
        case PACK_KEY_STRING:
          if (!_recv_string(unpacker, ctxt, error))
            return NULL;
          break;
        case PACK_KEY_TUPLE:
          if (!_recv_tuple(unpacker, ctxt, error))
            return NULL;
          break;
        case PACK_KEY_OBJECT:
          if (!_recv_object(unpacker, ctxt, error))
            return NULL;
          break;
        default:
          CL_ERROR("[TODO] type = %d", ctxt->c_type);
        }
      if (ctxt->c_any.done)
        {
          LObject *obj = ctxt->c_any.object;
          ASSERT(obj);
          context_pop(unpacker);
          if (priv->current_context < 0)
            return obj;
          else
            object_push(unpacker, obj);
        }
    }
}
