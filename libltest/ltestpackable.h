/* ltestpackable.h -
 */

#ifndef _LTESTPACKABLE_H_
#define _LTESTPACKABLE_H_

#include "los/lobject.h"
#include "libltest/ltestpackable-def.h"



/* LTestPackable:
 */
struct _LTestPackable
{
  LTEST_PACKABLE_INSTANCE_HEADER;

  gint a;
  gchar *b;
};



/* LTestPackableClass:
 */
struct _LTestPackableClass
{
  LTEST_PACKABLE_CLASS_HEADER;
};



LTestPackable *ltest_packable_new ( gint a,
                                    const gchar *b );



#endif
