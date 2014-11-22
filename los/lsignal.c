/* lsignal.c -
 */

#include "los/private.h"
#include "los/lsignal.h"



typedef struct _HandlerNode HandlerNode;
typedef struct _HandlerKey HandlerKey;
typedef struct _HandlerList HandlerList;



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



/* HandlerNode:
 */
struct _HandlerNode
{
  GQuark detail;
  LSignalHandler func;
  gpointer data;
  GDestroyNotify destroy_data;
  /* list links */
  HandlerList *list;
  HandlerNode *next;
  HandlerNode *prev;
};



/* HandlerKey:
 */
struct _HandlerKey
{
  LObject *object;
  SignalNode *signal;
};



/* HandlerList:
 */
struct _HandlerList
{
  HandlerKey key;
  HandlerNode *first;
  HandlerNode *last;
};

#define HANDLER_KEY(k) ((HandlerKey *)(k))



static GHashTable *signal_nodes = NULL; /* set < SignalNode > */
static GHashTable *handler_lists = NULL; /* set < HandlerList > */



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
  return GPOINTER_TO_UINT(HANDLER_KEY(key)->object) ^ GPOINTER_TO_UINT(HANDLER_KEY(key)->signal);
}



/* handler_key_equal:
 */
static gboolean handler_key_equal ( gconstpointer k1,
                                    gconstpointer k2 )
{
  return HANDLER_KEY(k1)->object == HANDLER_KEY(k2)->object &&
    HANDLER_KEY(k1)->signal == HANDLER_KEY(k2)->signal;
}



/* handler_list_new:
 */
static HandlerList *handler_list_new ( LObject *object,
                                       SignalNode *signal )
{
  HandlerList *l = g_new0(HandlerList, 1);
  l->key.object = object;
  l->key.signal = signal;
  return l;
}



/* handler_list_lookup:
 */
static HandlerList *handler_list_lookup ( LObject *object,
                                          SignalNode *signal )
{
  HandlerKey key;
  key.object = object;
  key.signal = signal;
  return g_hash_table_lookup(handler_lists, &key);
}



/* handler_list_append:
 */
static void handler_list_append ( HandlerList *l,
                                  HandlerNode *n )
{
  ASSERT(!n->list);
  n->list = l;
  if (l->first)
    {
      ASSERT(l->last);
      l->last->next = n;
    }
  else
    {
      ASSERT(!l->last);
      l->first = n;
    }
  n->prev = l->last;
  l->last = n;
}



void _l_signal_init ( void )
{
  signal_nodes = g_hash_table_new_full(signal_node_hash,
                                       signal_node_equal,
                                       NULL,
                                       (GDestroyNotify) signal_node_free);
  handler_lists = g_hash_table_new(handler_key_hash, handler_key_equal);
}



/* handler_node_new:
 */
HandlerNode *handler_node_new ( GQuark detail,
                                LSignalHandler func,
                                gpointer data,
                                GDestroyNotify destroy_data )
{
  HandlerNode *node = g_new0(HandlerNode, 1);
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
  HandlerList *hlist;
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
  handler = handler_node_new(detail, func, data, destroy_data);
  if (!(hlist = handler_list_lookup(object, node)))
    {
      hlist = handler_list_new(object, node);
      g_hash_table_insert(handler_lists, hlist, hlist);
    }
  handler_list_append(hlist, handler);
}



/* l_signal_emit:
 */
void l_signal_emit ( LObject *object,
                     LSignalID signal,
                     GQuark detail )
{
  SignalNode *node;
  HandlerList *hlist;
  node = (SignalNode *) signal;
  ASSERT(node);
  if ((hlist = handler_list_lookup(object, signal)))
    {
      HandlerNode *handler;
      l_object_ref(object);
      for (handler = hlist->first; handler; handler = handler->next)
        {
          if (handler->detail == 0 || handler->detail == detail)
            handler->func(object, handler->data);
        }
      l_object_unref(object);
    }
}
