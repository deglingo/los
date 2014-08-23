/* ltype.c -
 */

#include "los/ltype.h"



static gboolean l_type_initialized = FALSE;
static GPtrArray *type_nodes = NULL;



/* TypeNode:
 */
typedef struct _TypeNode TypeNode;

struct _TypeNode
{
  LType type;
  LTypeInfo info;
};



/* l_type_init:
 */
void l_type_init ( void )
{
  if (l_type_initialized)
    return;
  l_type_initialized = TRUE;
  type_nodes = g_ptr_array_new();
  g_ptr_array_add(type_nodes, NULL);
}



/* l_type_register:
 */
LType l_type_register ( const char *name,
                        LType parent,
                        LTypeInfo *info )
{
  TypeNode *node;
  if (!l_type_initialized) {
    fprintf(stderr, "ERROR: l_type_init() must be called first\n");
    exit(1);
  }
  node = g_new0(TypeNode, 1);
  node->type = type_nodes->len;
  node->info = *info;
  g_ptr_array_add(type_nodes, node);
  return node->type;
}



/* l_type_instantiate:
 */
void *l_type_instantiate ( LType type )
{
  TypeNode *node;
  void *instance;
  /* [FIXME] check type validity */
  node = type_nodes->pdata[type];
  instance = g_malloc0(node->info.instance_size);
  return instance;
}
