/* lsignal.h -
 */

#ifndef _LSIGNAL_H_
#define _LSIGNAL_H_

#include "los/lobject.h"



typedef guint LSignalID;

typedef void (* LSignalHandler) ( LObject *object,
                                  gpointer data );



LSignalID l_signal_new ( LObjectClass *cls,
                         const gchar *name );
void l_signal_connect ( LObject *object,
                        const gchar *name,
                        LSignalHandler handler,
                        gpointer data,
                        GDestroyNotify destroy_data );



#endif
