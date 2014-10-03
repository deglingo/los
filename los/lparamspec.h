/* lparamspec.h -
 */

#ifndef _LPARAMSPEC_H_
#define _LPARAMSPEC_H_

#include "los/lbase.h"

struct _LObjectClass;



typedef struct _LParamSpec LParamSpec;



/* LParamSpec:
 */
struct _LParamSpec
{
  volatile guint _ref_count;
  guint param_id;
  struct _LObjectClass *owner_type;
  gchar *name;
  struct _LObjectClass *value_type;
};



LParamSpec *l_param_spec_int ( const gchar *name,
                               gint defo );
LParamSpec *l_param_spec_ref ( LParamSpec *pspec );
void l_param_spec_unref ( LParamSpec *pspec );



#endif
