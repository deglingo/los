/* parser.h -
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "base.h"
#include "ast.h"

G_BEGIN_DECLS



typedef struct _Parser Parser;



Parser *parser_new ( void );
AST *parser_parse ( Parser *parser,
                    FILE *file );

#define YYSTYPE AST *
#define YYLTYPE Location

#define YYLLOC_DEFAULT(cur, rhs, n) do {                \
    if (n) {                                            \
      (cur).qfile = YYRHSLOC(rhs, 1).qfile;             \
      (cur).lineno = YYRHSLOC(rhs, 1).lineno;           \
      (cur).charno = YYRHSLOC(rhs, 1).charno;           \
      (cur).end_qfile = YYRHSLOC(rhs, n).end_qfile;     \
      (cur).end_lineno = YYRHSLOC(rhs, n).end_lineno;   \
      (cur).end_charno = YYRHSLOC(rhs, n).end_charno;   \
    } else {                                            \
      (cur).qfile = (cur).end_qfile =                   \
        YYRHSLOC(rhs, 0).end_qfile;                     \
      (cur).lineno = (cur).end_lineno =                 \
        YYRHSLOC(rhs, 0).end_lineno;                    \
      (cur).charno = (cur).end_charno =                 \
        YYRHSLOC(rhs, 0).end_charno;                    \
    }                                                   \
  } while (0)

int yyparse ( Parser *parser );
int yylex ( AST **lvalp,
            Location *llocp,
            Parser *parser );
void yyerror ( Location *locp,
               Parser *parser,
               char const *msg );

AST *parser_context ( Parser *parser );
AST *parser_push ( Parser *parser,
                   AST *node );
AST *parser_pop ( Parser *parser );
AST *parser_get_namespace ( Parser *parser,
                            AST *ident );
AST *parser_get_class ( Parser *parser,
                        gboolean isdef,
                        AST *type_specs,
                        AST *ident,
                        AST *base_clause );
AST *parser_get_func ( Parser *parser,
                       AST *ident );
AST *parser_get_type ( Parser *parser,
                       gboolean absolute,
                       AST *name_list );



G_END_DECLS

#endif
