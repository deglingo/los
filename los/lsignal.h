/* lsignal.h -
 */

#ifndef _LSIGNAL_H_
#define _LSIGNAL_H_

#include "los/lobject.h"



typedef gpointer LSignalID;
typedef guint LSignalHandlerID;
typedef struct _LSignalHandlerGroup LSignalHandlerGroup;

typedef void (* LSignalHandler) ( LObject *object,
                                  gpointer data );



void _l_signal_init ( void );

LSignalID l_signal_new ( LObjectClass *cls,
                         const gchar *name );
LSignalHandlerID l_signal_connect ( LObject *object,
                                    const gchar *name,
                                    LSignalHandler handler,
                                    gpointer data,
                                    GDestroyNotify destroy_data );
void l_signal_handler_remove ( LSignalHandlerID handler );
void l_signal_emit ( LObject *object,
                     LSignalID signal,
                     GQuark detail );

LSignalHandlerGroup *l_signal_handler_group_new ( void );
LSignalHandlerID l_signal_handler_group_connect ( LSignalHandlerGroup *group,
                                                  LObject *object,
                                                  const gchar *name,
                                                  LSignalHandler func,
                                                  gpointer data,
                                                  GDestroyNotify destroy_data );
void l_signal_handler_group_remove_all ( LSignalHandlerGroup *group );



#endif
