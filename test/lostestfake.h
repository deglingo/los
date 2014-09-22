/* lostestfake.h - some fake class to test
 */

#ifndef _LOSTESTFAKE_H_
#define _LOSTESTFAKE_H_

#include "los/lobject.h"

#define CLASS_FAKE (fake_get_class())
#define CLASS_FAKE2 (fake2_get_class())



/* Fake */
typedef struct _Fake
{
  LObject super;
  gint instval;
}
  Fake;

/* FakeClass */
typedef struct _FakeClass
{
  LObjectClass super_class;
  int dummy;
}
  FakeClass;

/* some globals to check the callbacks */
extern gint fake_dispose_count;
extern gint fake_finalize_count;

LObjectClass *fake_get_class ( void );

/* Fake2 */
typedef struct _Fake2
{
  Fake super;
}
  Fake2;

/* Fake2Class */
typedef struct _Fake2Class
{
  FakeClass super_class;
}
  Fake2Class;

LObjectClass *fake2_get_class ( void );


#endif
