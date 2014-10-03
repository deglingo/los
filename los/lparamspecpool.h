/* lparamspecpool.h -
 */

#ifndef _LPARAMSPECPOOL_H_
#define _LPARAMSPECPOOL_H_

#include "los/lbase.h"
#include "los/lparamspec.h"



typedef struct _LParamSpecPool LParamSpecPool;



LParamSpecPool *l_param_spec_pool_new ( void );
void l_param_spec_pool_free ( LParamSpecPool *pool );



#endif
