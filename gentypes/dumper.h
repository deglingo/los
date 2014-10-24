/* dumper.h -
 */

#ifndef _DUMPER_H_
#define _DUMPER_H_

#include "base.h"
#include "ast.h"



typedef struct _Dumper Dumper;



Dumper *dumper_new ( const gchar *srcdir,
                     const gchar *incsubdir );
void dumper_dump ( Dumper *dumper,
                   AST *ast );
void dumper_write ( Dumper *dumper );



#endif
