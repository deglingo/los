/* parser.c -
 */

#include "parser.h"
#include "lexer.h"



/* Parser:
 */
struct _Parser
{
  Lexer *lexer;
  GSList *stack;
  AST *root;
  /* builtins */
  AST *l_ns;
  AST *l_object;
};



/* parser_new:
 */
Parser *parser_new ( void )
{
  Parser *parser = g_new0(Parser, 1);
  return parser;
}



/* parser_parse:
 */
AST *parser_parse ( Parser *parser,
                    FILE *file )
{
  ast_init();
  parser->lexer = lexer_new(file);
  parser->root = ast_namespace_decl_new(NULL, ast_ident_new(""));
  parser_push(parser, parser->root);
  /* builtins */
  {
    parser->l_ns = ast_namespace_decl_new(parser->root, ast_ident_new("L"));
    parser->l_object = ast_class_decl_new(parser->l_ns, ast_ident_new("Object"), NULL);
    AST_DECL(parser->l_ns)->isextern = TRUE;
    AST_DECL(parser->l_object)->isextern = TRUE;
  }
  if (yyparse(parser) == 0)
    {
      ASSERT(parser->stack && parser->stack->data == parser->root);
      parser_pop(parser);
      ASSERT(!parser->stack);
      return parser->root;
    }
  else
    {
      return NULL;
    }
}



/* yylex:
 */
int yylex ( AST **lvalp,
            Location *llocp,
            Parser *parser )
{
  return lexer_lex(parser->lexer, lvalp, llocp);
}



/* yyerror:
 */
void yyerror ( Location *loc,
               Parser *parser,
               char const *msg )
{
  fprintf(stderr, "los-gentypes:%s:%d: %s\n",
          g_quark_to_string(loc->qfile),  loc->lineno, msg);
}



/* parser_context:
 */
AST *parser_context ( Parser *parser )
{
  return parser->stack ? parser->stack->data : NULL;
}



/* parser_push:
 */
AST *parser_push ( Parser *parser,
                   AST *node )
{
  parser->stack = g_slist_prepend(parser->stack, node);
  return node;
}



/* parser_pop:
 */
AST *parser_pop ( Parser *parser )
{
  AST *node;
  ASSERT(parser->stack);
  node = parser->stack->data;
  parser->stack = g_slist_delete_link(parser->stack, parser->stack);
  return node;
}



/* parser_get_namespace:
 */
AST *parser_get_namespace ( Parser *parser,
                            AST *ident )
{
  AST *node;
  if ((node = ast_decl_get_member(parser_context(parser), ident)))
    {
      ASSERT(AST_IS_NAMESPACE_DECL(node));
    }
  else
    {
      node = ast_namespace_decl_new(parser_context(parser), ident);
    }
  return node;
}



/* _find_class_decl:
 */
static gboolean _find_class_decl ( AST *item,
                                   gpointer data )
{
  return AST_IS_CLASS_DECL(item);
}



/* parser_get_class:
 */
AST *parser_get_class ( Parser *parser,
                        gboolean isdef,
                        AST *type_specs,
                        AST *ident,
                        AST *bases )
{
  AST *node, *l;
  /* [FIXME] lookup existing */
  if (isdef)
    {
      if (!ast_list_find_p(bases, _find_class_decl, NULL))
        bases = ast_list_prepend(bases, parser->l_object);
    }
  else
    {
      ASSERT(!bases);
    }
  node = ast_class_decl_new(parser_context(parser), ident, bases);
  for (l = type_specs; l; l = AST_LIST_NEXT(l))
    {
      AST *spec = AST_LIST_ITEM(l);
      if (spec == ast_ident_new("extern")) { /* [FIXME] */
        AST_DECL(node)->isextern = TRUE;
      } else {
        CL_ERROR("[TODO] %s", ast_type_name(spec->type));
      }
    }
  if (isdef)
    {
      ASSERT(!AST_CLASS_DECL_DEFINED(node));
      AST_CLASS_DECL(node)->defined = TRUE;
    }
  return node;
}



/* parser_get_func:
 */
AST *parser_get_func ( Parser *parser,
                       AST *ident )
{
  AST *decl;
  decl = ast_func_decl_new(parser_context(parser), ident);
  return decl;
}



/* parser_collect_class:
 */
AST *parser_collect_class ( Parser *parser,
                            AST *ident,
                            AST *base_clause,
                            gboolean defined,
                            AST *body )
{
  CL_ERROR("[TODO]");
  return NULL;
}



/* _get_type:
 */
static AST *_get_type ( Parser *parser,
                        AST *ctxt,
                        AST *name_list )
{
  AST *l;
  AST *tp = ctxt;
  ASSERT(AST_IS_LIST(name_list));
  for (l = name_list; l; l = AST_LIST_NEXT(l))
    {
      AST *id = AST_LIST_ITEM(l);
      ASSERT(AST_IS_IDENT(id));
      tp = ast_decl_get_member(tp, id);
      if (!tp)
        return NULL;
    }
  return tp;
}



static gchar *make_fqname ( AST *name_list )
{
  GString *s = g_string_new("");
  gboolean first = TRUE;
  AST *l;
  for (l = name_list; l; l = AST_LIST_NEXT(l))
    {
      if (first)
        first = FALSE;
      else
        g_string_append(s, "::");
      g_string_append(s, AST_IDENT_NAME(AST_LIST_ITEM(l)));
    }
  return g_string_free(s, FALSE);
}



/* get_rel_type:
 */
static AST *get_rel_type ( Parser *parser,
                           AST *name_list )
{
  AST *found = NULL;
  AST *ctxt;
  for (ctxt = parser_context(parser); ctxt; ctxt = AST_DECL(ctxt)->context)
    {
      AST *tp;
      if ((tp = _get_type(parser, ctxt, name_list)))
        {
          ASSERT(!found); /* [TODO] */
          found = tp;
        }
    }
  if (!found)
    CL_ERROR("type not found: '%s'", make_fqname(name_list));
  return found;
}



/* parser_get_type:
 */
AST *parser_get_type ( Parser *parser,
                       gboolean absolute,
                       AST *name_list )
{
  if (absolute)
    {
      CL_ERROR("[TODO]");
      return NULL;
    }
  else
    {
      return get_rel_type(parser, name_list);
    }
}
