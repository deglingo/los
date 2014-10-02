/* lsignal.h -
 */

#ifndef _LSIGNAL_H_
#define _LSIGNAL_H_

#include "los/lobject.h"



typedef guint LSignalID;



LSignalID l_signal_new ( LObjectClass *cls,
                         const gchar *name );



#endif
