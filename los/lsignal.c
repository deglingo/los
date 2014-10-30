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



/* static GHashTable *signal_nodes = NULL; /\* map < sigid, node > *\/ */
static GHashTable *signal_names = NULL; /* map < name, node > */
static GHashTable *signal_handlers = NULL; /* map < (object, sigid), handler > */
/* static volatile LSignalID sigid_counter = 1; */



/* signal_node_new:
 */
SignalNode *signal_node_new ( LObjectClass *cls,
                              const gchar *name )
{
  SignalNode *node = g_new0(SignalNode, 1);
  /* node->sigid = g_atomic_int_add(&sigid_counter, 1); */
  node->cls = l_object_ref(cls);
  node->name = g_strdup(name);
  return node;
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



/* l_signal_new:
 */
LSignalID l_signal_new ( LObjectClass *cls,
                         const gchar *name )
{
  SignalNode *node;
  /* [FIXME] global init */
  if (!signal_names) {
    /* signal_nodes = g_hash_table_new(NULL, NULL); */
    signal_names = g_hash_table_new(g_str_hash, g_str_equal);
    signal_handlers = g_hash_table_new(handler_key_hash, handler_key_equal);
  }
  /* create the node */
  node = signal_node_new(cls, name);
  /* g_hash_table_insert(signal_nodes, GUINT_TO_POINTER(node->sigid), node); */
  g_hash_table_insert(signal_names, node->name, node);
  return (LSignalID) node;
}



/* l_signal_connect:
 */
void l_signal_connect ( LObject *object,
                        const gchar *name,
                        GQuark detail,
                        LSignalHandler func,
                        gpointer data,
                        GDestroyNotify destroy_data )
{
  SignalNode *node;
  HandlerNode *handler;
  GList *hlist;
  node = g_hash_table_lookup(signal_names, name);
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
