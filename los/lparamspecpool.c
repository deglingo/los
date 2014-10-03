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
