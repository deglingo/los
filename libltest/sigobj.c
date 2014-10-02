/* sigobj.c -
 */

#include "libltest/sigobj.h"
#include "los/lsignal.h"
#include "libltest/sigobj.inl"



/* Signals:
 */
enum
  {
    SIG_SIGNAL1 = 0,
    SIG_COUNT,
  };

static LSignalID signals[SIG_COUNT] = { 0, };



/* sigobj_class_init:
 */
static void sigobj_class_init ( LObjectClass *cls )
{
  signals[SIG_SIGNAL1] = l_signal_new(cls,
                                      "signal1");
}



/* sigobj_new:
 */
Sigobj *sigobj_new ( void )
{
  return SIGOBJ(l_object_new(CLASS_SIGOBJ, NULL));
}



/* sigobj_signal1:
 */
void sigobj_signal1 ( Sigobj *obj )
{
  l_signal_emit(L_OBJECT(obj), signals[SIG_SIGNAL1]);
}
