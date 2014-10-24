/* main.c -
 */

#include "base.h"
#include "parser.h"
#include "dumper.h"

#include <string.h>
#include <errno.h>



/* main:
 */
gint main ( gint argc,
            gchar **argv )
{
  Parser *parser;
  FILE *infile;
  gboolean close_infile;
  AST *ast;
  Dumper *dumper;
  /* CL_DEBUG("hello!"); */
  /* command line */
  if (argc == 1)
    {
      infile = stdin;
      close_infile = FALSE;
    }
  else if (argc == 2)
    {
      if (!(infile = fopen(argv[1], "r")))
        CL_ERROR("could not open '%s' : %s", argv[1], strerror(errno));
      close_infile = TRUE;
    }
  else
    {
      CL_ERROR("too many args");
    }
  /* parse */
  parser = parser_new();
  if (!(ast = parser_parse(parser, infile)))
    CL_ERROR("parse error");
  if (close_infile)
    fclose(infile);
  /* dump */
  dumper = dumper_new();
  dumper_dump(dumper, ast);
  dumper_write(dumper);
  return 0;
}
