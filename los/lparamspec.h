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
  guint param_id;
  struct LObjectClass *owner_type;
};



LParamSpec *l_param_spec_int ( const gchar *name,
                               gint defo );
void l_param_spec_unref ( LParamSpec *pspec );



#endif
