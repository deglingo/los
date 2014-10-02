/* sigobj.h -
 */

#ifndef _SIGOBJ_H_
#define _SIGOBJ_H_

#include "los/lobject.h"
#include "libltest/sigobj-def.h"



/* Sigobj:
 */
struct _Sigobj
{
  SIGOBJ_INSTANCE_HEADER;
};



/* SigobjClass:
 */
struct _SigobjClass
{
  SIGOBJ_CLASS_HEADER;
};



Sigobj *sigobj_new ( void );
void sigobj_signal1 ( Sigobj *obj );



#endif
