/* parsergram.y -
 */

%{
#include "parser.h"

#define TODO() CL_ERROR("[TODO]")

#define PUSH(n)                   (parser_push(parser, (n)))
#define POP()                     (parser_pop(parser))
#define PUSH_NS(id)               (PUSH(parser_get_namespace(parser, (id))))
#define PUSH_CLASS(def, specs, id, base) (PUSH(parser_get_class(parser, (def), (specs), (id), (base))))
%}

%define api.pure
%locations
%parse-param { Parser *parser }
%lex-param   { Parser *parser }

%token TOK_NAMESPACE TOK_CLASS TOK_EXTERN TOK_VOID
%token TOK_IDENT TOK_COLONCOLON

%%

input
  : /* empty */      { $$ = NULL; }
  | ext_decl_list    { $$ = $1; }
  ;

ext_decl_list
  : ext_decl                  { $$ = ast_list_prepend(NULL, $1); }
  | ext_decl ext_decl_list    { $$ = ast_list_prepend($2, $1); }
  ;

ext_decl
  : namespace_decl
  | class_decl
  | func_decl
  ;

namespace_decl
  : namespace_proto namespace_body    { $$ = POP(); }
  ;

namespace_proto
  : TOK_NAMESPACE TOK_IDENT    { $$ = PUSH_NS($2); }
  ;

namespace_body
  : '{' '}'    { TODO(); }
  | '{' ext_decl_list '}'    { $$ = $2; }
  ;

class_decl
  : storage_spec_list TOK_CLASS TOK_IDENT base_clause { PUSH_CLASS(TRUE, $1, $3, $4);    } class_body ';' { $$ = POP(); }
  | storage_spec_list TOK_CLASS TOK_IDENT             { PUSH_CLASS(FALSE, $1, $3, NULL); }            ';' { $$ = POP(); }
  ;

storage_spec_list
  : /* empty */                    { $$ = NULL; }
  | storage_spec storage_spec_list { $$ = ast_list_prepend($2, $1); }
  ;

storage_spec
  : TOK_EXTERN
  ;

base_clause
  : /* empty */          { $$ = NULL; }
  | ':' base_clause_list { $$ = $2; }
  ;

base_clause_list
  : qual_type_name                      { $$ = ast_list_prepend(NULL, $1); }
  | qual_type_name ',' base_clause_list { $$ = ast_list_prepend($3, $1); }
  ;

qual_type_name
  : abs_qual_type_name
  | rel_qual_type_name
  ;

abs_qual_type_name
  : TOK_COLONCOLON qual_type_name_list    { $$ = parser_get_type(parser, TRUE, $2); }
  ;

rel_qual_type_name
  : qual_type_name_list    { $$ = parser_get_type(parser, FALSE, $1); }
  ;

qual_type_name_list
  : TOK_IDENT                                       { $$ = ast_list_prepend(NULL, $1); }
  | TOK_IDENT TOK_COLONCOLON qual_type_name_list    { $$ = ast_list_prepend($3, $1); }
  ;

class_body
  : '{' '}'                  { $$ = NULL; }
  | '{' ext_decl_list '}'    { $$ = $2; }
  ;

func_decl
  : TOK_VOID TOK_IDENT '(' ')' ';' { $$ = parser_get_func(parser, $2); }
  ;

%%
