/* lprivate.h
 */

#ifndef _LPRIVATE_H_
#define _LPRIVATE_H_

#include "los/lbase.h"



/* LPrivate:
 */
typedef struct _LPrivate
{
  gpointer trash_pool;
}
  LPrivate;



LPrivate *l_private_get ( void );



#endif

