/* lparamspec.c -
 */

#include "los/lparamspec.h"



/* l_param_spec_int:
 */
LParamSpec *l_param_spec_int ( const gchar *name,
                               gint defo )
{
  LParamSpec *pspec;
  pspec = g_new0(LParamSpec, 1);
  return pspec;
}



/* l_param_spec_unref:
 */
void l_param_spec_unref ( LParamSpec *pspec )
{
  /* [FIXME] */
  g_free(pspec);
}

