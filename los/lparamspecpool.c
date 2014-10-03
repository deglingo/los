/* lparamspecpool.c -
 */

#include "los/lparamspecpool.h"



/* LParamSpecPool:
 */
struct _LParamSpecPool
{
  int _dummy;
};



/* l_param_spec_pool_new:
 */
LParamSpecPool *l_param_spec_pool_new ( void )
{
  LParamSpecPool *pool = g_new0(LParamSpecPool, 1);
  return pool;
}



/* l_param_spec_pool_free:
 */
void l_param_spec_pool_free ( LParamSpecPool *pool )
{
  g_free(pool);
}



/* l_param_spec_pool_register:
 */
void l_param_spec_pool_register ( LParamSpecPool *pool,
                                  LObjectClass *owner_type,
                                  guint param_id,
                                  LParamSpec *pspec )
{
}



/* l_param_spec_pool_lookup:
 */
LParamSpec *l_param_spec_pool_lookup ( LParamSpecPool *pool,
                                       LObjectClass *owner_type,
                                       const gchar *name )
{
  return NULL;
}
