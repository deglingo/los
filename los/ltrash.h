/* ltrash.h -
 */

#ifndef _LTRASH_H_
#define _LTRASH_H_

#include "los/lbase.h"



typedef void (* LTrashFunc) ( gpointer item );



/* LTrashItem:
 */
typedef struct _LTrashItem
{
  gpointer ptr;
  LTrashFunc func;
}
  LTrashItem;



/* LTrash:
 */
typedef struct _LTrash
{
  guint size;
  guint n_items;
  LTrashItem *items;
}
  LTrash;



#define L_TRASH_INIT(size) LTrash *_l_trash = (l_trash_init(g_alloca(sizeof(LTrash) + sizeof(LTrashItem) * size), size))

#define L_TRASH_CLEANUP() l_trash_cleanup(_l_trash)

#define L_TRASH_GPOINTER(ptr) (l_trash_add(_l_trash, (ptr), g_free))

#define L_TRASH_OBJECT(obj) (l_trash_add(_l_trash, (obj), l_object_unref))



LTrash *l_trash_init ( LTrash *trash,
                       guint size );
void l_trash_cleanup ( LTrash *trash );
gpointer l_trash_add ( LTrash *trash,
                       gpointer item,
                       LTrashFunc func );




#endif
