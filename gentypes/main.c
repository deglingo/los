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
  FILE *infile = NULL;;
  gboolean close_infile;
  AST *ast;
  Dumper *dumper;
  gint a;
  gchar *srcdir = ".";
  gchar *incsubdir = "";
  /* CL_DEBUG("hello!"); */
  /* command line */
  for (a = 1; a < argc; a++)
    {
      if (!strcmp(argv[a], "-s"))
        {
          a++;
          ASSERT(a < argc);
          srcdir = argv[a];
        }
      else if (!strcmp(argv[a], "-u"))
        {
          a++;
          ASSERT(a < argc);
          incsubdir = argv[a];
        }
      else
        {
          ASSERT(!infile);
          if (!(infile = fopen(argv[a], "r")))
            CL_ERROR("could not open '%s' : %s", argv[a], strerror(errno));
          close_infile = TRUE;
        }
    }
  if (!infile)
    {
      infile = stdin;
      close_infile = FALSE;
    }
  /* parse */
  parser = parser_new();
  if (!(ast = parser_parse(parser, infile)))
    CL_ERROR("parse error");
  if (close_infile)
    fclose(infile);
  /* dump */
  dumper = dumper_new(srcdir, incsubdir);
  dumper_dump(dumper, ast);
  dumper_write(dumper);
  return 0;
}
