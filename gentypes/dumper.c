/* dumper.c -
 */

#include "dumper.h"
#include "writer.h"

#include <string.h>



/* Context:
 */
typedef struct _Context
{
  Writer *hfile;
  Writer *ifile;
  Writer *htmpl;
  Writer *ctmpl;

  WNode *s_typedefs;
  WNode *s_defines;
  WNode *s_fndecls;

  WNode *i_vrdefs;
  WNode *i_fndecls;
  WNode *i_fndefs;
  WNode *i_clsinit;
  WNode *i_clsinit_head;
  WNode *i_clsinit_body;
  WNode *i_clsinit_foot;
  WNode *i_init;
  WNode *i_init_head;
  WNode *i_init_body;
  WNode *i_init_foot;
}
  Context;



/* Dumper:
 */
struct _Dumper
{
  gchar *srcdir;
  gchar *incsubdir;
  GSList *stack;
  GPtrArray *writers;
};



/* DumperFuncs:
 */
typedef void (* DumperFunc) ( Dumper *dumper,
                              AST *ast );

typedef struct _DumperFuncs
{
  ASTType type;
  DumperFunc accept;
  DumperFunc enter;
  DumperFunc leave;
}
  DumperFuncs;



static void visit ( Dumper *dumper,
                    AST *ast );

static void accept_default ( Dumper *dumper,
                             AST *ast );
static void enter_default ( Dumper *dumper,
                            AST *ast );
static void leave_default ( Dumper *dumper,
                            AST *ast );
static void accept_list ( Dumper *dumper,
                          AST *ast );
static void accept_decl ( Dumper *dumper,
                          AST *ast );
static void enter_namespace_decl ( Dumper *dumper,
                                   AST *ast );
static void leave_namespace_decl ( Dumper *dumper,
                                   AST *ast );
static void enter_class_decl ( Dumper *dumper,
                               AST *ast );
static void leave_class_decl ( Dumper *dumper,
                               AST *ast );
static void enter_func_decl ( Dumper *dumper,
                              AST *ast );
static void leave_func_decl ( Dumper *dumper,
                              AST *ast );



/* VTABLE:
 */
static DumperFuncs VTABLE[] =
  {
    { AST_TYPE_NODE, accept_default, enter_default, leave_default },
    { AST_TYPE_LIST, accept_list, },
    { AST_TYPE_DECL, accept_decl, },
    { AST_TYPE_NAMESPACE_DECL, NULL, enter_namespace_decl, leave_namespace_decl },
    { AST_TYPE_CLASS_DECL, NULL, enter_class_decl, leave_class_decl },
    { AST_TYPE_FUNC_DECL, NULL, enter_func_decl, leave_func_decl },
    { 0, },
  };



/* _dumper_funcs_lookup:
 */
static DumperFuncs *_dumper_funcs_lookup ( ASTType type )
{
  DumperFuncs *f;
  for (f = VTABLE; f->type; f++)
    {
      if (f->type == type)
        return f;
    }
  return NULL;
}



/* dumper_funcs_lookup:
 */
static void dumper_funcs_lookup ( ASTType type,
                                  DumperFuncs *dest )
{
  DumperFuncs *f;
  memset(dest, 0, sizeof(DumperFuncs));
  while (type)
    {
      if ((f = _dumper_funcs_lookup(type)))
        {
          if ((!dest->accept) && f->accept)
            dest->accept = f->accept;
          if ((!dest->enter) && f->enter)
            dest->enter = f->enter;
          if ((!dest->leave) && f->leave)
            dest->leave = f->leave;
          if (dest->accept && dest->enter && dest->leave)
            return;
        }
      type = ast_type_parent(type);
    }
}



/* dumper_new:
 */
Dumper *dumper_new ( const gchar *srcdir,
                     const gchar *incsubdir )
{
  Dumper *dumper = g_new0(Dumper, 1);
  dumper->srcdir = g_strdup(srcdir);
  dumper->incsubdir = g_strdup(incsubdir);
  dumper->writers = g_ptr_array_new();
  return dumper;
}



/* dumper_dump:
 */
void dumper_dump ( Dumper *dumper,
                   AST *ast )
{
  visit(dumper, ast);
}



/* dumper_write:
 */
void dumper_write ( Dumper *dumper )
{
  guint f;
  for (f = 0; f < dumper->writers->len; f++)
    {
      writer_write(dumper->writers->pdata[f]);
    }
}



/* dumper_writer_new:
 */
static Writer *dumper_writer_new ( Dumper *dumper,
                                   const gchar *fname )
{
  Writer *w = writer_new(fname);
  g_ptr_array_add(dumper->writers, w);
  return w;
}



/* dumper_push:
 */
static Context *dumper_push ( Dumper *dumper )
{
  Context *ctxt = g_new(Context, 1);
  if (dumper->stack)
    *ctxt = *((Context *)(dumper->stack->data));
  else
    memset(ctxt, 0, sizeof(Context));
  dumper->stack = g_slist_prepend(dumper->stack, ctxt);
  return ctxt;
}



/* dumper_pop:
 */
static void dumper_pop ( Dumper *dumper )
{
  ASSERT(dumper->stack);
  dumper->stack = g_slist_delete_link(dumper->stack, dumper->stack);
}



/* visit:
 */
static void visit ( Dumper *dumper,
                    AST *ast )
{
  DumperFuncs funcs;
  if (!ast)
    return;
  dumper_funcs_lookup(ast->type, &funcs);
  funcs.enter(dumper, ast);
  funcs.accept(dumper, ast);
  funcs.leave(dumper, ast);
}



/* accept_default:
 */
static void accept_default ( Dumper *dumper,
                             AST *ast )
{
  CL_TRACE("%s", ast_type_name(ast->type));
}



/* enter_default:
 */
static void enter_default ( Dumper *dumper,
                            AST *ast )
{
  /* CL_TRACE("%s", ast_type_name(ast->type)); */
}



/* leave_default:
 */
static void leave_default ( Dumper *dumper,
                            AST *ast )
{
  /* CL_TRACE("%s", ast_type_name(ast->type)); */
}



/* accept_list:
 */
static void accept_list ( Dumper *dumper,
                          AST *ast )
{
  AST *l;
  for (l = ast; l; l = AST_LIST_NEXT(l))
    visit(dumper, AST_LIST_ITEM(l));
}



/* accept_decl:
 */
static void accept_decl ( Dumper *dumper,
                          AST *ast )
{
  visit(dumper, AST_DECL_MEMBERS(ast));
}



/* enter_namespace_decl:
 */
static void enter_namespace_decl ( Dumper *dumper,
                                   AST *ast )
{
  /* CL_TRACE("%s", AST_DECL(ast)->cident->full.cml); */
}



/* leave_namespace_decl:
 */
static void leave_namespace_decl ( Dumper *dumper,
                                   AST *ast )
{
  /* CL_TRACE("%p", ast); */
}



/* _write_class_header:
 */
static void _write_class_header ( Context *ctxt,
                                  CIdent *id,
                                  CIdent *base_id )

{
  /* get_class macro */
  wsection_add(ctxt->s_defines,
               wtext_newf("#define %sCLASS_%s (%s_get_class())\n",
                          id->pfx.cst, id->sfx.cst, id->full.var),
               NULL);
  /* standard macros */
  wsection_writef(ctxt->s_defines,
                  "#define %s(obj) (L_OBJECT_CHECK_INSTANCE_CAST((obj), %sCLASS_%s, %s))\n",
                  id->full.cst, id->pfx.cst, id->sfx.cst, id->full.cml);
  wsection_writef(ctxt->s_defines,
                  "#define %sIS_%s(obj) (L_OBJECT_CHECK_INSTANCE_TYPE((obj), %sCLASS_%s))\n",
                  id->pfx.cst, id->sfx.cst, id->pfx.cst, id->sfx.cst);
  wsection_writef(ctxt->s_defines,
                  "#define %s_CLASS(cls) (L_OBJECT_CHECK_CLASS_CAST((cls), %sCLASS_%s, %sClass))\n",
                  id->full.cst, id->pfx.cst, id->sfx.cst, id->full.cml);
  wsection_writef(ctxt->s_defines,
                  "#define %s_GET_CLASS(obj) (L_OBJECT_GET_CLASS_CAST((obj), %sCLASS_%s, %sClass))\n",
                  id->full.cst, id->pfx.cst, id->sfx.cst, id->full.cml);
  wsection_writef(ctxt->s_defines,
                  "#define %s_NEW(v...) (%s(l_object_new(%sCLASS_%s, v)))\n",
                  id->full.cst, id->full.cst, id->pfx.cst, id->sfx.cst);
  /* instance header macro */
  wsection_add(ctxt->s_defines,
               wtext_newf("#define %s_INSTANCE_HEADER \\\n", id->full.cst),
               wtext_newf("  %s _super\n", base_id->full.cml),
               NULL);
  /* class header macro */
  wsection_add(ctxt->s_defines,
               wtext_newf("#define %s_CLASS_HEADER \\\n", id->full.cst),
               wtext_newf("  %sClass _super_class\n", base_id->full.cml),
               NULL);
  /* typedefs */
  wsection_writef(ctxt->s_typedefs,
                  "typedef struct _%s %s;\n",
                  id->full.cml, id->full.cml);
  wsection_writef(ctxt->s_typedefs,
                  "typedef struct _%sClass %sClass;\n",
                  id->full.cml, id->full.cml);
  /* get_class func decl */
  wsection_writef(ctxt->s_fndecls,
                  "LObjectClass *%s_get_class ( void );\n",
                  id->full.var);
}



/* _write_class_inl:
 */
static void _write_class_inl ( Context *ctxt,
                               CIdent *id,
                               CIdent *base_id )
{
  /* parent_class */
  wsection_add(ctxt->i_vrdefs,
               wtext_newf("static LObjectClass *parent_class = NULL;\n"),
               NULL);
  /* class_init func */
  wsection_add(ctxt->i_fndecls,
               wtext_newf("static void __%s_class_init ( LObjectClass *cls );\n",
                          id->full.var),
               NULL);
  wsection_add(ctxt->i_clsinit_head,
               wtext_newf("static void __%s_class_init ( LObjectClass *cls )\n",
                          id->full.var),
               wtext_newf("{\n"),
               wtext_newf("  parent_class = cls->l_parent_class;\n"),
               NULL);
  wsection_add(ctxt->i_clsinit_foot,
               wtext_newf("}\n"),
               NULL);
  /* instance_init func */
  wsection_add(ctxt->i_fndecls,
               wtext_newf("static void __%s_init ( LObject *obj );\n",
                          id->full.var),
               NULL);
  wsection_add(ctxt->i_init_head,
               wtext_newf("static void __%s_init ( LObject *obj )\n",
                          id->full.var),
               wtext_newf("{\n"),
               NULL);
  wsection_add(ctxt->i_init_foot,
               wtext_newf("}\n"),
               NULL);
  /* get_class func */
  wsection_add(ctxt->i_fndefs,
               wtext_newf("LObjectClass *%s_get_class ( void )\n", id->full.var),
               wtext_newf("{\n"),
               wtext_newf("  static LObjectClass *cls = NULL;\n"),
               wtext_newf("  if (!cls)\n"),
               wtext_newf("  {\n"),
               wtext_newf("    LClassInfo info = { 0, };\n"),
               wtext_newf("    info.class_size = sizeof(%sClass);\n", id->full.cml),
               wtext_newf("    info.instance_size = sizeof(%s);\n", id->full.cml),
               wtext_newf("    info.class_init = __%s_class_init;\n", id->full.var),
               wtext_newf("    info.init = __%s_init;\n", id->full.var),
               wtext_newf("    cls = l_object_class_register(\"%s\", %sCLASS_%s, &info);\n",
                          id->full.cml, base_id->pfx.cst, base_id->sfx.cst),
               wtext_newf("  }\n"),
               wtext_newf("  return cls;\n"),
               wtext_newf("}\n"),
               NULL);
}



/* _write_template_header:
 */
static void _write_template_header ( Dumper *dumper,
                                     CIdent *id,
                                     CIdent *base_id )
{
  Context *ctxt = dumper->stack->data; /* check! */
  GString *hguard = g_string_new("");
  g_string_printf(hguard, "_%s_H_", id->fname);
  g_string_ascii_up(hguard);
  wsection_add(ctxt->htmpl->root,
               wtext_newf("#error FIX THIS FILE AND REMOVE ME\n"),
               wtext_newf("/* %s.h -\n", id->fname),
               wtext_newf(" */\n"),
               wtext_newf("\n"),
               wtext_newf("#ifndef %s\n", hguard->str),
               wtext_newf("#define %s\n", hguard->str),
               wtext_newf("\n"),
               wtext_newf("#include \"%s%s.h\"\n", dumper->incsubdir, base_id->fname),
               wtext_newf("#include \"%s%s-def.h\"\n", dumper->incsubdir, id->fname),
               wtext_newf("\n"),
               wtext_newf("G_BEGIN_DECLS\n"),
               wtext_newf("\n\n\n"),
               wtext_newf("/* %s:\n", id->full.cml),
               wtext_newf(" */\n"),
               wtext_newf("struct _%s\n", id->full.cml),
               wtext_newf("{\n"),
               wtext_newf("  %s_INSTANCE_HEADER;\n", id->full.cst),
               wtext_newf("};\n"),
               wtext_newf("\n\n\n"),
               wtext_newf("/* %sClass:\n", id->full.cml),
               wtext_newf(" */\n"),
               wtext_newf("struct _%sClass\n", id->full.cml),
               wtext_newf("{\n"),
               wtext_newf("  %s_CLASS_HEADER;\n", id->full.cst),
               wtext_newf("};\n"),
               wtext_newf("\n\n\n"),
               wtext_newf("G_END_DECLS\n"),
               wtext_newf("\n"),
               wtext_newf("#endif /* ifndef %s */\n", hguard->str),
               NULL);
  g_string_free(hguard, TRUE);
}



/* _write_template_body:
 */
static void _write_template_body ( Dumper *dumper,
                                   CIdent *id )
{
  Context *ctxt = dumper->stack->data; /* check! */
  wsection_add(ctxt->ctmpl->root,
               wtext_newf("#error FIX THIS FILE AND REMOVE ME\n"),
               wtext_newf("/* %s.c -\n", id->fname),
               wtext_newf(" */\n"),
               wtext_newf("\n"),
               wtext_newf("#include \"%s%s.h\"\n", dumper->incsubdir, id->fname),
               wtext_newf("#include \"%s%s.inl\"\n", dumper->incsubdir, id->fname),
               NULL);
}



/* enter_class_decl:
 */
static void enter_class_decl ( Dumper *dumper,
                               AST *ast )
{
  CIdent *id;
  CIdent *base_id;
  Context *ctxt = dumper_push(dumper);
  gchar *hfile, *ifile, *htmpl, *ctmpl;
  if (AST_DECL_EXTERN(ast))
    return;
  id = AST_DECL_CIDENT(ast);
  ASSERT(AST_CLASS_DECL_BASE_CLASS(ast));
  base_id = AST_DECL_CIDENT(AST_CLASS_DECL_BASE_CLASS(ast));
  /* CL_TRACE("%s", AST_DECL(ast)->cident->full.cml); */
  APRINTF(&hfile, "%s-def.h", id->fname);
  APRINTF(&ifile, "%s.inl", id->fname);
  APRINTF(&htmpl, "%s/%s.h", dumper->srcdir, id->fname);
  APRINTF(&ctmpl, "%s/%s.c", dumper->srcdir, id->fname);
  ctxt->hfile = dumper_writer_new(dumper, hfile);
  ctxt->ifile = dumper_writer_new(dumper, ifile);

  ctxt->s_defines = wsection_new(ctxt->hfile->root, "defines");
  ctxt->s_typedefs = wsection_new(ctxt->hfile->root, "typedefs");
  ctxt->s_fndecls = wsection_new(ctxt->hfile->root, "fndecls");

  ctxt->i_vrdefs = wsection_new(ctxt->ifile->root, "vrdefs");
  ctxt->i_fndecls = wsection_new(ctxt->ifile->root, "fndecls");
  ctxt->i_fndefs = wsection_new(ctxt->ifile->root, "fndefs");
  ctxt->i_clsinit = wsection_new(ctxt->i_fndefs, "clsinit");
  ctxt->i_clsinit_head = wsection_new(ctxt->i_clsinit, "head");
  ctxt->i_clsinit_body = wsection_new(ctxt->i_clsinit, "body");
  ctxt->i_clsinit_foot = wsection_new(ctxt->i_clsinit, "foot");
  ctxt->i_init = wsection_new(ctxt->i_fndefs, "init");
  ctxt->i_init_head = wsection_new(ctxt->i_init, "head");
  ctxt->i_init_body = wsection_new(ctxt->i_init, "body");
  ctxt->i_init_foot = wsection_new(ctxt->i_init, "foot");

  _write_class_header(ctxt, id, base_id);
  _write_class_inl(ctxt, id, base_id);
  if (!g_file_test(htmpl, G_FILE_TEST_EXISTS))
    {
      ctxt->htmpl = dumper_writer_new(dumper, htmpl);
      _write_template_header(dumper, id, base_id);
    }
  if (!g_file_test(ctmpl, G_FILE_TEST_EXISTS))
    {
      ctxt->ctmpl = dumper_writer_new(dumper, ctmpl);
      _write_template_body(dumper, id);
    }
}



/* leave_class_decl:
 */
static void leave_class_decl ( Dumper *dumper,
                               AST *ast )
{
  /* CL_TRACE("%p", ast); */
  dumper_pop(dumper);
}



/* _write_class_init_handler:
 */
static void _write_class_init_handler ( Context *ctxt,
                                        CIdent *id )
{
  wsection_add(ctxt->i_fndecls,
               wtext_newf("static void %s_class_init ( LObjectClass *cls );\n", id->full.var),
               NULL);
  wsection_add(ctxt->i_clsinit_body,
               wtext_newf("%s_class_init(cls);\n", id->full.var),
               NULL);
}



/* _write_init_handler:
 */
static void _write_init_handler ( Context *ctxt,
                                  CIdent *id )
{
  wsection_add(ctxt->i_fndecls,
               wtext_newf("static void %s_init ( LObject *obj );\n", id->full.var),
               NULL);
  wsection_add(ctxt->i_init_body,
               wtext_newf("%s_init(obj);\n", id->full.var),
               NULL);
}



/* enter_func_decl:
 */
static void enter_func_decl ( Dumper *dumper,
                              AST *ast )
{
  Context *ctxt = dumper_push(dumper);
  if (AST_DECL_IDENT(ast) == ast_ident_new("class_init"))
    {
      _write_class_init_handler(ctxt, AST_DECL_CIDENT(AST_DECL_CONTEXT(ast)));
    }
  else if (AST_DECL_IDENT(ast) == ast_ident_new("init"))
    {
      _write_init_handler(ctxt, AST_DECL_CIDENT(AST_DECL_CONTEXT(ast)));
    }
  else
    {
      CL_ERROR("[TODO]");
    }
}



/* leave_func_decl:
 */
static void leave_func_decl ( Dumper *dumper,
                              AST *ast )
{
  dumper_pop(dumper);
}
