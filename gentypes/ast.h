/* ast.h -
 */

#ifndef _AST_H_
#define _AST_H_

#include "base.h"
#include "cident.h"

G_BEGIN_DECLS



typedef enum _ASTType ASTType;
typedef union _AST AST;
typedef struct _ASTNode ASTNode;
typedef struct _ASTList ASTList;
typedef struct _ASTIdent ASTIdent;
typedef struct _ASTDecl ASTDecl;
typedef struct _ASTNamespaceDecl ASTNamespaceDecl;
typedef struct _ASTClassDecl ASTClassDecl;
typedef struct _ASTFuncDecl ASTFuncDecl;



/* ASTType:
 */
enum _ASTType
  {
    AST_TYPE_NONE = 0,
    AST_TYPE_NODE,
    AST_TYPE_LIST,
    AST_TYPE_IDENT,
    AST_TYPE_DECL,
    AST_TYPE_NAMESPACE_DECL,
    AST_TYPE_CLASS_DECL,
    AST_TYPE_FUNC_DECL,
  };



/* ASTNode:
 */
struct _ASTNode
{
  ASTType type;
};



/* ASTList:
 */
struct _ASTList
{
  ASTNode super;
  AST *item;
  AST *prev;
  AST *next;
};



/* ASTIdent:
 */
struct _ASTIdent
{
  ASTNode super;
  gchar *name;
};



/* ASTDecl:
 */
struct _ASTDecl
{
  ASTNode super;

  AST *ident;
  CIdent *cident;
  AST *context;
  AST *members;
  gboolean isextern;
};



/* ASTNamespaceDecl:
 */
struct _ASTNamespaceDecl
{
  ASTDecl super;
};



/* ASTClassDecl:
 */
struct _ASTClassDecl
{
  ASTDecl super;

  gboolean defined;
  AST *base_class;
};



/* ASTFuncDecl:
 */
struct _ASTFuncDecl
{
  ASTDecl super;
};



/* AST:
 */
union _AST
{
  ASTType type;
  ASTNode node;
  ASTList list;
  ASTIdent ident;
  ASTDecl decl;
  ASTNamespaceDecl namespace_decl;
  ASTClassDecl class_decl;
};



void ast_init ( void );

/* generic type macros */
#define AST_CAST(n, tp, s_tp) ((s_tp *)(ast_cast((n), (tp))))
#define AST_CHECK(n, tp)      (ast_check((n), (tp)))

gboolean ast_type_isa ( ASTType type1,
                        ASTType type2 );
const gchar *ast_type_name ( ASTType type );
ASTType ast_type_parent ( ASTType type );
gpointer ast_cast ( gpointer ast,
                    ASTType type );
gboolean ast_check ( gpointer ast,
                     ASTType type );

/* ASTList */
#define AST_LIST(n) (AST_CAST((n), AST_TYPE_LIST, ASTList))
#define AST_IS_LIST(n) (AST_CHECK((n), AST_TYPE_LIST))

#define AST_LIST_ITEM(n) (AST_LIST(n)->item)
#define AST_LIST_PREV(n) (AST_LIST(n)->prev)
#define AST_LIST_NEXT(n) (AST_LIST(n)->next)

typedef gboolean (* ASTListFindFunc) ( AST *item,
                                       gpointer data );

AST *ast_list_prepend ( AST *list,
                        AST *item );
AST *ast_list_append ( AST *list,
                       AST *item );
AST *ast_list_merge ( AST *list1,
                      AST *list2 );
AST *ast_list_tail ( AST *list );
AST *ast_list_find_p ( AST *list,
                       ASTListFindFunc func,
                       gpointer data );

/* ASTIdent */
#define AST_IDENT(n) (AST_CAST((n), AST_TYPE_IDENT, ASTIdent))
#define AST_IS_IDENT(n) (AST_CHECK((n), AST_TYPE_IDENT))

#define AST_IDENT_NAME(n) (AST_IDENT(n)->name)

AST *ast_ident_new ( const gchar *name );

/* ASTDecl */
#define AST_DECL(n) (AST_CAST((n), AST_TYPE_DECL, ASTDecl))
#define AST_IS_DECL(n) (AST_CHECK((n), AST_TYPE_DECL))

#define AST_DECL_CONTEXT(n) (AST_DECL(n)->context)
#define AST_DECL_IDENT(n) (AST_DECL(n)->ident)
#define AST_DECL_NAME(n) (AST_DECL_IDENT(n) ? AST_IDENT_NAME(AST_DECL_IDENT(n)) : NULL)
#define AST_DECL_CIDENT(n) (AST_DECL(n)->cident)
#define AST_DECL_MEMBERS(n) (AST_DECL(n)->members)
#define AST_DECL_EXTERN(n) (AST_DECL(n)->isextern)

AST *ast_decl_get_member ( AST *decl,
                           AST *ident );

/* ASTNamespace */
#define AST_NAMESPACE_DECL(n) (AST_CAST((n), AST_TYPE_NAMESPACE_DECL, ASTNamespaceDecl))
#define AST_IS_NAMESPACE_DECL(n) (AST_CHECK((n), AST_TYPE_NAMESPACE_DECL))

AST *ast_namespace_decl_new ( AST *context,
                              AST *ident );

/* ASTClassDecl */
#define AST_CLASS_DECL(n) (AST_CAST((n), AST_TYPE_CLASS_DECL, ASTClassDecl))
#define AST_IS_CLASS_DECL(n) (AST_CHECK((n), AST_TYPE_CLASS_DECL))

#define AST_CLASS_DECL_DEFINED(n) (AST_CLASS_DECL(n)->defined)
#define AST_CLASS_DECL_BASE_CLASS(n) (AST_CLASS_DECL(n)->base_class)

AST *ast_class_decl_new ( AST *context,
                          AST *ident,
                          AST *base );

/* ASTFuncDecl */
#define AST_FUNC_DECL(n) (AST_CAST((n), AST_TYPE_FUNC_DECL, ASTFuncDecl))
#define AST_IS_FUNC_DECL(n) (AST_CHECK((n), AST_TYPE_FUNC_DECL))

AST *ast_func_decl_new ( AST *context,
                         AST *ident );



G_END_DECLS

#endif
