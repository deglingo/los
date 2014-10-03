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
  pspec->_ref_count = 1;
  return pspec;
}



/* l_param_spec_ref:
 */
LParamSpec *l_param_spec_ref ( LParamSpec *pspec )
{
  g_atomic_int_inc(&pspec->_ref_count);
  return pspec;
}



/* l_param_spec_unref:
 */
void l_param_spec_unref ( LParamSpec *pspec )
{
  if (g_atomic_int_dec_and_test(&pspec->_ref_count))
    {
      g_free(pspec);
    }
}
