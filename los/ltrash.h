/* ltrash.h -
 */

#ifndef _LTRASH_H_
#define _LTRASH_H_

#include "los/lbase.h"



typedef void (* LTrashFunc) ( gpointer item );



#define L_TRASH_GPOINTER(ptr) (l_trash_add((ptr), g_free))

#define L_TRASH_OBJECT(obj) (l_trash_add((obj), l_object_unref))



void l_trash_push ( void );
void l_trash_pop ( void );
gpointer l_trash_add ( gpointer item,
                       LTrashFunc func );




#endif
