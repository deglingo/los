/* lexer.h -
 */

#ifndef _LEXER_H_
#define _LEXER_H_

#include "base.h"
#include "ast.h"

G_BEGIN_DECLS



typedef struct _Lexer Lexer;



Lexer *lexer_new ( FILE *file );
gint lexer_lex ( Lexer *lexer,
                 AST **value,
                 Location *loc );



G_END_DECLS

#endif
