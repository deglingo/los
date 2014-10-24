/* ast.c -
 */

#include "ast.h"

#include <string.h>



/* ident_cache:
 */
static GHashTable *ident_cache = NULL;



/* TypeInfo:
 */
typedef struct _TypeInfo
{
  ASTType type;
  ASTType parent;
  gchar *name;
}
  TypeInfo;



/* TypeNode:
 */
typedef struct _TypeNode TypeNode;
struct _TypeNode
{
  ASTType type;
  TypeNode *parent;
  gchar *name;
};

static GPtrArray *type_nodes = NULL;



/* type_lookup:
 */
static inline TypeNode *type_lookup ( ASTType type )
{
  TypeNode *node;
  ASSERT(type > 0 && type < type_nodes->len);
  node = type_nodes->pdata[type];
  ASSERT(node);
  return node;
}



/* type_register:
 */
static void type_register ( TypeInfo *info )
{
  TypeNode *node;
  ASSERT(info->type > 0);
  ASSERT(info->name);
  node = g_new0(TypeNode, 1);
  node->type = info->type;
  node->name = g_strdup(info->name);
  if (info->parent == 0) {
    node->parent = NULL;
  } else {
    node->parent = type_lookup(info->parent);
  }
  if (info->type >= type_nodes->len)
    g_ptr_array_set_size(type_nodes, info->type+1);
  ASSERT(!type_nodes->pdata[info->type]);
  type_nodes->pdata[node->type] = node;
}



/* ast_init:
 */
void ast_init ( void )
{
  static gboolean done = FALSE;
  TypeInfo type_info[] = {
    { AST_TYPE_NODE,           0,             "Node" },
    { AST_TYPE_LIST,           AST_TYPE_NODE, "List" },
    { AST_TYPE_IDENT,          AST_TYPE_NODE, "Ident" },
    { AST_TYPE_DECL,           AST_TYPE_NODE, "Decl" },
    { AST_TYPE_NAMESPACE_DECL, AST_TYPE_DECL, "NamespaceDecl" },
    { AST_TYPE_CLASS_DECL,     AST_TYPE_DECL, "ClassDecl" },
    { AST_TYPE_FUNC_DECL,      AST_TYPE_DECL, "FuncDecl" },
    { 0, },
  };
  TypeInfo *info;
  if (done)
    return;
  done = TRUE;
  ident_cache = g_hash_table_new(g_str_hash, g_str_equal);
  /* types */
  type_nodes = g_ptr_array_new();
  g_ptr_array_add(type_nodes, NULL);
  for (info = type_info; info->type; info++)
    {
      type_register(info);
    }
}



/* ast_type_isa:
 */
gboolean ast_type_isa ( ASTType type1,
                        ASTType type2 )
{
  TypeNode *n1 = type_lookup(type1);
  TypeNode *n2 = type_lookup(type2);
  while (n1)
    {
      if (n1 == n2)
        return TRUE;
      n1 = n1->parent;
    }
  return FALSE;
}



/* ast_type_parent:
 */
ASTType ast_type_parent ( ASTType type )
{
  TypeNode *n = type_lookup(type);
  return n->parent ? n->parent->type : 0;
}



/* ast_type_name:
 */
const gchar *ast_type_name ( ASTType type )
{
  TypeNode *n = type_lookup(type);
  return n->name;
}



/* ast_cast:
 */
gpointer ast_cast ( gpointer ast,
                    ASTType type )
{
  ASSERT(ast);
  ASSERT(ast_check(ast, type));
  return ast;
}



/* ast_check:
 */
gboolean ast_check ( gpointer ast,
                     ASTType type )
{
  ASSERT(ast);
  return ast_type_isa(((AST *) ast)->type, type);
}



/* ast_new:
 */
static AST *ast_new ( ASTType type )
{
  AST *node = g_new0(AST, 1);
  node->type = type;
  return node;
}



/* ast_list_prepend:
 */
AST *ast_list_prepend ( AST *list,
                        AST *item )
{
  AST *link;
  link = ast_new(AST_TYPE_LIST);
  AST_LIST(link)->item = item;
  if (list)
    {
      ASSERT(AST_IS_LIST(list));
      ASSERT(!AST_LIST_PREV(list));
      AST_LIST(list)->prev = link;
      AST_LIST(link)->next = list;
    }
  return link;
}



/* ast_list_append:
 */
AST *ast_list_append ( AST *list,
                       AST *item )
{
  AST *link;
  link = ast_new(AST_TYPE_LIST);
  AST_LIST(link)->item = item;
  if (list)
    {
      AST *tail;
      ASSERT(AST_IS_LIST(list));
      tail = ast_list_tail(list);
      AST_LIST(tail)->next = link;
      AST_LIST(link)->prev = tail;
      return list;
    }
  else
    {
      return link;
    }
}



/* ast_list_merge:
 */
AST *ast_list_merge ( AST *list1,
                      AST *list2 )
{
  AST *tail;
  ASSERT((!list1) || AST_IS_LIST(list1));
  ASSERT((!list2) || AST_IS_LIST(list2));
  if (!list1)
    return list2;
  if (!list2)
    return list1;
  tail = ast_list_tail(list1);
  AST_LIST(tail)->next = list2;
  AST_LIST(list2)->prev = tail;
  return list1;
}



/* ast_list_tail:
 */
AST *ast_list_tail ( AST *list )
{
  if (list)
    {
      ASSERT(AST_IS_LIST(list));
      while (AST_LIST_NEXT(list))
        list = AST_LIST_NEXT(list);
      return list;
    }
  else
    {
      return NULL;
    }
}



/* ast_list_find_p:
 */
AST *ast_list_find_p ( AST *list,
                       ASTListFindFunc func,
                       gpointer data )
{
  while (list)
    {
      if (func(AST_LIST_ITEM(list), data))
        return AST_LIST_ITEM(list);
      list = AST_LIST_NEXT(list);
    }
  return NULL;
}



/* ast_ident_new:
 */
AST *ast_ident_new ( const gchar *name )
{
  AST *node;
  if (!(node = g_hash_table_lookup(ident_cache, name)))
    {
      node = ast_new(AST_TYPE_IDENT);
      node->ident.name = g_strdup(name);
      g_hash_table_insert(ident_cache, node->ident.name, node);
    }
  return node;
}



/* ast_decl_new:
 */
static AST *ast_decl_new ( ASTType type,
                           AST *context,
                           CIdentType ident_type,
                           AST *ident )
{
  AST *node;
  ASSERT(ast_type_isa(type, AST_TYPE_DECL));
  ASSERT(AST_IS_IDENT(ident));
  node = ast_new(type);
  AST_DECL(node)->ident = ident;
  if (context)
    {
      ASSERT(AST_IS_DECL(context));
      AST_DECL(node)->context = context;
      AST_DECL(context)->members = ast_list_append(AST_DECL(context)->members, node);
      AST_DECL(node)->cident = c_ident_new(ident_type, AST_IDENT_NAME(ident), AST_DECL(context)->cident);
    }
  else
    {
      AST_DECL(node)->cident = c_ident_new(ident_type, AST_IDENT_NAME(ident), NULL);
    }
  return node;
}



/* ast_decl_get_member:
 */
AST *ast_decl_get_member ( AST *decl,
                           AST *ident )
{
  AST *l;
  ASSERT(AST_IS_DECL(decl));
  ASSERT(AST_IS_IDENT(ident));
  for (l = AST_DECL_MEMBERS(decl); l; l = AST_LIST_NEXT(l))
    {
      AST *m = AST_LIST_ITEM(l);
      if (AST_DECL_IDENT(m) == ident)
        return m;
    }
  return NULL;
}



/* ast_namespace_decl_new:
 */
AST *ast_namespace_decl_new ( AST *context,
                              AST *ident )
{
  AST *node;
  node = ast_decl_new(AST_TYPE_NAMESPACE_DECL, context, C_IDENT_TYPE_CML, ident);
  return node;
}



/* ast_class_decl_new:
 */
AST *ast_class_decl_new ( AST *context,
                          AST *ident,
                          AST *bases )
{
  AST *node, *l;
  node = ast_decl_new(AST_TYPE_CLASS_DECL, context, C_IDENT_TYPE_CML, ident);
  /* bases */
  ASSERT((!bases) || AST_IS_LIST(bases));
  for (l = bases; l; l = AST_LIST_NEXT(l))
    {
      AST *b = AST_LIST_ITEM(l);
      if (AST_IS_CLASS_DECL(b))
        {
          ASSERT(!AST_CLASS_DECL_BASE_CLASS(node));
          AST_CLASS_DECL(node)->base_class = b;
        }
      else
        {
          CL_ERROR("[TODO] %s", ast_type_name(b->type));
        }
    }
  return node;
}



/* ast_func_decl_new:
 */
AST *ast_func_decl_new ( AST *context,
                         AST *ident )
{
  AST *decl;
  decl = ast_decl_new(AST_TYPE_FUNC_DECL, context, C_IDENT_TYPE_VAR, ident);
  return decl;
}
