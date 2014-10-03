/* lparamspecpool.h -
 */

#ifndef _LPARAMSPECPOOL_H_
#define _LPARAMSPECPOOL_H_

#include "los/lbase.h"
#include "los/lparamspec.h"
#include "los/lobject.h"



typedef struct _LParamSpecPool LParamSpecPool;



LParamSpecPool *l_param_spec_pool_new ( void );
void l_param_spec_pool_free ( LParamSpecPool *pool );
void l_param_spec_pool_register ( LParamSpecPool *pool,
                                  LObjectClass *owner_type,
                                  guint param_id,
                                  LParamSpec *pspec );



#endif
