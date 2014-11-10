/* ltrashstack.h -
 */

#ifndef _LTRASHSTACK_H_
#define _LTRASHSTACK_H_

#include "los/lbase.h"



typedef struct _LTrashStack LTrashStack;



LTrashStack *l_trash_stack_new ( gsize item_size,
                                 gssize max_height );
void l_trash_stack_destroy ( LTrashStack *stack );
gpointer l_trash_stack_alloc ( LTrashStack *stack );
gpointer l_trash_stack_alloc0 ( LTrashStack *stack );
void l_trash_stack_free ( LTrashStack *stack,
                          gpointer item );



#endif
