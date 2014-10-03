/* lparamspec.h -
 */

#ifndef _LPARAMSPEC_H_
#define _LPARAMSPEC_H_

#include "los/lbase.h"



typedef struct _LParamSpec LParamSpec;



LParamSpec *l_param_spec_int ( const gchar *name,
                               gint defo );



#endif
