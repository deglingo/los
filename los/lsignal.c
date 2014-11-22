/* lsignal.c -
 */

#include "los/private.h"
#include "los/lsignal.h"



/* SignalNode:
 */
typedef struct _SignalNode
{
  /* LSignalID sigid; */
  LObjectClass *cls;
  gchar *name;
}
  SignalNode;

#define SIGNAL_NODE(n) ((SignalNode *)(n))



/* HandlerKey:
 */
typedef struct _HandlerKey
{
  LObject *object;
  SignalNode *signal;
}
  HandlerKey;



/* HandlerNode:
 */
typedef struct _HandlerNode
{
  HandlerKey key;
  GQuark detail;
  LSignalHandler func;
  gpointer data;
  GDestroyNotify destroy_data;
}
  HandlerNode;



static GHashTable *signal_nodes = NULL;
static GHashTable *signal_handlers = NULL; /* map < (object, sigid), handler > */
/* static volatile LSignalID sigid_counter = 1; */



/* signal_node_new:
 */
static SignalNode *signal_node_new ( LObjectClass *cls,
                                     const gchar *name )
{
  SignalNode *node = g_new0(SignalNode, 1);
  node->cls = cls; /* [FIXME] weakref ? */
  node->name = g_strdup(name);
  return node;
}



/* signal_node_free:
 */
static void signal_node_free ( SignalNode *node )
{
  g_free(node->name);
  g_free(node);
}



/* signal_node_hash:
 */
static guint signal_node_hash ( gconstpointer node )
{
  const gchar *p;
  guint32 h = 5381;
  for (p = SIGNAL_NODE(node)->name; *p; p++)
    h = (h << 5) + h + *p;
  return h ^ GPOINTER_TO_UINT(SIGNAL_NODE(node)->cls);
}



/* signal_node_equal:
 */
static gboolean signal_node_equal ( gconstpointer n1,
                                    gconstpointer n2 )
{
  return SIGNAL_NODE(n1)->cls == SIGNAL_NODE(n2)->cls &&
    !strcmp(SIGNAL_NODE(n1)->name, SIGNAL_NODE(n2)->name);
}



/* signal_node_lookup:
 */
static SignalNode *signal_node_lookup ( LObject *obj,
                                        const gchar *name )
{
  SignalNode key, *node;
  /* [FIXME] cache the result ? */
  /* [FIXME] not sure that discarding const is safe */
  key.name = (gchar *) name;
  for (key.cls = L_OBJECT_GET_CLASS(obj); key.cls; key.cls = key.cls->l_parent_class)
    {
      if ((node = g_hash_table_lookup(signal_nodes, &key)))
        return node;
    }
  return NULL;
}



/* handler_key_hash:
 */
static guint handler_key_hash ( gconstpointer key )
{
  const gchar *p, *end;
  guint h = 5381;
  end = key + sizeof(HandlerKey);
  for (p = key; p != end; p++)
    h = (h << 5) + h + *p;
  return h;
}



/* handler_key_equal:
 */
static gboolean handler_key_equal ( gconstpointer k1_,
                                    gconstpointer k2_ )
{
  const HandlerKey *k1 = k1_, *k2 = k2_;
  return k1->object == k2->object && k1->signal == k2->signal;
}



void _l_signal_init ( void )
{
  signal_nodes = g_hash_table_new_full(signal_node_hash,
                                       signal_node_equal,
                                       NULL,
                                       (GDestroyNotify) signal_node_free);
  signal_handlers = g_hash_table_new(handler_key_hash, handler_key_equal);
}



/* handler_node_new:
 */
HandlerNode *handler_node_new ( LObject *object,
                                SignalNode *signal,
                                GQuark detail,
                                LSignalHandler func,
                                gpointer data,
                                GDestroyNotify destroy_data )
{
  HandlerNode *node = g_new0(HandlerNode, 1);
  node->key.object = object; /* [TODO] weakref */
  node->key.signal = signal;
  node->detail = detail;
  node->func = func;
  node->data = data;
  node->destroy_data = destroy_data;
  return node;
}



/* l_signal_new:
 */
LSignalID l_signal_new ( LObjectClass *cls,
                         const gchar *name )
{
  SignalNode *node;
  /* create the node */
  node = signal_node_new(cls, name);
  /* g_hash_table_insert(signal_nodes, GUINT_TO_POINTER(node->sigid), node); */
  g_hash_table_insert(signal_nodes, node, node);
  return (LSignalID) node;
}



/* l_signal_connect:
 */
void l_signal_connect ( LObject *object,
                        const gchar *name,
                        LSignalHandler func,
                        gpointer data,
                        GDestroyNotify destroy_data )
{
  SignalNode *node;
  HandlerNode *handler;
  GList *hlist;
  const gchar *colon;
  GQuark detail;
  if ((colon = strchr(name, ':')))
    {
      gint len = colon - name;
      gchar *strip_name = g_alloca(len+1);
      memcpy(strip_name, name, len);
      strip_name[len] = 0;
      name = strip_name;
      detail = g_quark_from_string(colon + 1);
    }
  else
    {
      detail = 0;
    }
  node = signal_node_lookup(object, name);
  ASSERT(node);
  handler = handler_node_new(object, node, detail, func, data, destroy_data);
  if ((hlist = g_hash_table_lookup(signal_handlers, &handler->key)))
    /* NOTE: we only get the return value to avoid a warning, but no
       need to re-insert hlist which should not have changed */
    hlist = g_list_append(hlist, handler);
  else
    g_hash_table_insert(signal_handlers, &handler->key, g_list_append(NULL, handler));
}



/* l_signal_emit:
 */
void l_signal_emit ( LObject *object,
                     LSignalID signal,
                     GQuark detail )
{
  SignalNode *node;
  GList *hlist, *l;
  HandlerKey key;
  node = (SignalNode *) signal;
  ASSERT(node);
  key.object = object;
  key.signal = node;
  hlist = g_hash_table_lookup(signal_handlers, &key);
  l_object_ref(object);
  for (l = hlist; l; l = l->next)
    {
      HandlerNode *handler = l->data;
      if (handler->detail == 0 || handler->detail == detail)
        handler->func(object, handler->data);
    }
  l_object_unref(object);
}
