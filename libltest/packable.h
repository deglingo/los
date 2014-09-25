/* packable.h -
 */

#ifndef _PACKABLE_H_
#define _PACKABLE_H_

#include "los/lobject.h"
#include "libltest/packable-def.h"



/* Packable:
 */
struct _Packable
{
  PACKABLE_INSTANCE_HEADER;

  gint a;
  gchar *b;
};



/* PackableClass:
 */
struct _PackableClass
{
  PACKABLE_CLASS_HEADER;
};



Packable *packable_new ( gint a,
                         const gchar *b );



#endif
