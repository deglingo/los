/* lparamspecpool.c -
 */

#include "los/private.h"
#include "los/lparamspecpool.h"



/* PspecKey:
 */
typedef struct _PSpecKey
{
  LObjectClass *owner_type;
  /* [FIXME] use a GQuark instead of name ? */
  const gchar *name;
}
  PSpecKey;



/* LParamSpecPool:
 */
struct _LParamSpecPool
{
  GHashTable *pspecs;
  GHashTable *pspecs_cache;
};



/* pspec_key_hash:
 */
static guint pspec_key_hash ( gconstpointer key )
{
  /* just in case */
  G_STATIC_ASSERT(sizeof(guint) <= sizeof(LObjectClass *));
#define k ((const PSpecKey *) key)
  const gchar *p;
  guint h = 5381;
  for (p = k->name; *p; p++)
    h = (h << 5) + h + *p;
  return h ^ *((guint *) k->owner_type);
#undef k
}



/* pspec_key_equal:
 */
static gboolean pspec_key_equal ( gconstpointer k1_,
                                  gconstpointer k2_ )
{
#define k1 ((const PSpecKey *) k1_)
#define k2 ((const PSpecKey *) k2_)
  return k1->owner_type == k2->owner_type && !strcmp(k1->name, k2->name);
#undef k1
#undef k2
}
                              


/* l_param_spec_pool_new:
 */
LParamSpecPool *l_param_spec_pool_new ( void )
{
  LParamSpecPool *pool = g_new0(LParamSpecPool, 1);
  pool->pspecs = g_hash_table_new_full(pspec_key_hash, pspec_key_equal,
                                       (GDestroyNotify) g_free,
                                       (GDestroyNotify) l_param_spec_unref);
  pool->pspecs_cache = g_hash_table_new_full(pspec_key_hash, pspec_key_equal,
                                             (GDestroyNotify) g_free,
                                             (GDestroyNotify) l_param_spec_unref);
  return pool;
}



/* l_param_spec_pool_free:
 */
void l_param_spec_pool_free ( LParamSpecPool *pool )
{
  g_hash_table_unref(pool->pspecs);
  g_hash_table_unref(pool->pspecs_cache);
  g_free(pool);
}



/* l_param_spec_pool_register:
 */
void l_param_spec_pool_register ( LParamSpecPool *pool,
                                  LObjectClass *owner_type,
                                  guint param_id,
                                  LParamSpec *pspec )
{
  PSpecKey *key;
  l_param_spec_ref(pspec);
  ASSERT(!pspec->owner_type);
  ASSERT(pspec->param_id == 0);
  pspec->owner_type = owner_type; /* [FIXME] ref ? (would imply a ref-cycle) */
  pspec->param_id = param_id;
  key = g_new(PSpecKey, 1);
  key->owner_type = owner_type;
  key->name = pspec->name;
  ASSERT(!g_hash_table_lookup(pool->pspecs, key));
  g_hash_table_insert(pool->pspecs, key, pspec);
}



/* _lookup_full:
 */
static LParamSpec *_lookup_full ( LParamSpecPool *pool,
                                  LObjectClass *owner_type,
                                  const gchar *name )
{
  PSpecKey key;
  LParamSpec *p;
  key.owner_type = owner_type;
  key.name = name;
  while (key.owner_type)
    {
      if ((p = g_hash_table_lookup(pool->pspecs, &key)))
        return p;
      key.owner_type = key.owner_type->l_parent_class;
    }
  return NULL;
}



/* l_param_spec_pool_lookup:
 */
LParamSpec *l_param_spec_pool_lookup ( LParamSpecPool *pool,
                                       LObjectClass *owner_type,
                                       const gchar *name )
{
  PSpecKey key;
  LParamSpec *p;
  key.owner_type = owner_type;
  key.name = name;
  if (G_UNLIKELY(!g_hash_table_lookup_extended(pool->pspecs_cache, &key, NULL, (gpointer *) &p)))
    {
      PSpecKey *new_key;
      p = _lookup_full(pool, owner_type, name);
      new_key = g_new(PSpecKey, 1);
      *new_key = key;
      g_hash_table_insert(pool->pspecs_cache, new_key, l_param_spec_ref(p));
    }
  return p;
}
