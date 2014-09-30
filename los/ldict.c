/* ldict.c -
 */

#include "los/private.h"
#include "los/ldict.h"
#include "los/ldict.inl"



static void _dispose ( LObject *object );



/* l_dict_class_init:
 */
static void l_dict_class_init ( LObjectClass *cls )
{
  cls->dispose = _dispose;
}



/* l_dict_new:
 */
LDict *l_dict_new ( void )
{
  LDict *d = L_DICT(l_object_new(L_CLASS_DICT, NULL));
  d->ht = g_hash_table_new_full((GHashFunc) l_object_hash,
                                (GEqualFunc) l_object_eq,
                                (GDestroyNotify) l_object_unref,
                                (GDestroyNotify) l_object_unref);
  return d;
}



/* _dispose:
 */
static void _dispose ( LObject *object )
{
  LDict *d = L_DICT(object);
  if (d->ht) {
    g_hash_table_unref(d->ht);
    d->ht = NULL;
  }
  ((LObjectClass *) parent_class)->dispose(object);
}



/* l_dict_insert:
 */
void l_dict_insert ( LDict *dict,
                     LObject *key,
                     LObject *value )
{
  /* CL_DEBUG("insert: %p - %p", key, value); */
  g_hash_table_insert(dict->ht, l_object_ref(key), l_object_ref(value));
}



/* l_dict_lookup:
 */
LObject *l_dict_lookup ( LDict *dict,
                         LObject *key )
{
  return g_hash_table_lookup(dict->ht, key);
}
