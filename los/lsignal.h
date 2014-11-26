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

/* [FIXME] */
typedef LObject (* LSignalAccumulator) ( void );



/* LSignalFlags:
 */
typedef enum _LSignalFlags
  {
    L_SIGNAL_FLAG_DUMMY = 0,
  }
  LSignalFlags;



void _l_signal_init ( void );
void _l_signal_object_dispose ( LObject *object );

LSignalID l_signal_new ( LObjectClass *cls,
                         const gchar *name,
                         LSignalFlags flags,
                         guint class_offset,
                         LSignalAccumulator accu,
                         gpointer accu_data,
                         LObjectClass *return_type,
                         ... )
  G_GNUC_NULL_TERMINATED;
LSignalHandlerID l_signal_connect ( LObject *object,
                                    const gchar *name,
                                    LSignalHandler handler,
                                    gpointer data,
                                    GDestroyNotify destroy_data );
void l_signal_handler_remove ( LSignalHandlerID handler );
LObject *l_signal_emit ( LObject *object,
                         LSignalID signal,
                         GQuark detail,
                         ... )
  G_GNUC_NULL_TERMINATED;

LSignalHandlerGroup *l_signal_handler_group_new ( void );
LSignalHandlerID l_signal_handler_group_connect ( LSignalHandlerGroup *group,
                                                  LObject *object,
                                                  const gchar *name,
                                                  LSignalHandler func,
                                                  gpointer data,
                                                  GDestroyNotify destroy_data );
void l_signal_handler_group_remove_all ( LSignalHandlerGroup *group );



#endif
