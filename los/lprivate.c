/* lprivate.c -
 */

#include "los/lprivate.h"
#include "los/ltrash.h"



static void l_private_free ( LPrivate *priv );

static GPrivate l_private_key = G_PRIVATE_INIT((GDestroyNotify) l_private_free);



/* l_private_free:
 */
static void l_private_free ( LPrivate *priv )
{
  l_trash_pool_free(priv->trash_pool);
  g_free(priv);
}



/* l_private_get:
 */
LPrivate *l_private_get ( void )
{
  LPrivate *priv;
  if (G_UNLIKELY(!(priv = g_private_get(&l_private_key))))
    {
      priv = g_new0(LPrivate, 1);
      g_private_set(&l_private_key, priv);
    }
  return priv;
}
