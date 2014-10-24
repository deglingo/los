/* writer.c -
 */

#include "writer.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>



static void wnode_dump ( WNode *node,
                         FILE *f );



/* filecmp:
 */
static gint filecmp ( const gchar *fname1,
                      const gchar *fname2 )
{
  struct stat s1, s2;
  GMappedFile *m1, *m2;
  gint r;
  GError *error = NULL;
  if (stat(fname1, &s1) != 0)
    CL_ERROR("could not stat '%s' : %s", fname1, strerror(errno));
  if (stat(fname2, &s2) != 0)
    CL_ERROR("could not stat '%s' : %s", fname2, strerror(errno));
  if (s1.st_size != s2.st_size)
    return 1;
  if (!(m1 = g_mapped_file_new(fname1, FALSE, &error)))
    CL_ERROR("could not open '%s' : %s", fname1, error->message);
  if (!(m2 = g_mapped_file_new(fname2, FALSE, &error)))
    CL_ERROR("could not open '%s' : %s", fname2, error->message);
  r = memcmp(g_mapped_file_get_contents(m1),
             g_mapped_file_get_contents(m2),
             s1.st_size);
  g_mapped_file_unref(m1);
  g_mapped_file_unref(m2);
  return r;
}



/* writer_new:
 */
Writer *writer_new ( const gchar *fname )
{
  Writer *w = g_new0(Writer, 1);
  w->fname = g_strdup(fname);
  w->root = wsection_new(NULL, "");
  return w;
}



/* writer_write:
 */
void writer_write ( Writer *writer )
{
  gchar *tmpname;
  FILE *f;
  /* CL_DEBUG("WRITE: '%s'", writer->fname); */
  tmpname = g_strdup_printf("%s.tmp", writer->fname);
  if (!(f = fopen(tmpname, "w")))
    CL_ERROR("could not open '%s' : %s", tmpname, strerror(errno));
  wnode_dump(writer->root, f);
  fclose(f);
  if (!g_file_test(writer->fname, G_FILE_TEST_EXISTS)) {
    CL_DEBUG(" C %s", writer->fname);
    rename(tmpname, writer->fname);
  } else if (filecmp(tmpname, writer->fname)) {
    CL_DEBUG(" U %s", writer->fname);
    rename(tmpname, writer->fname);
  } else {
    /* CL_DEBUG(" - %s", writer->fname); */
    unlink(tmpname);
  }
}



/* wnode_dump:
 */
static void wnode_dump ( WNode *node,
                         FILE *f )
{
  switch (node->type)
    {
    case WNODE_TYPE_SECTION:
      {
        guint n;
        for (n = 0; n < node->section.children->len; n++)
          wnode_dump(node->section.children->pdata[n], f);
      }
      break;
    case WNODE_TYPE_TEXT:
      fprintf(f, node->text.text);
      break;
    default:
      CL_ERROR("[TODO] %d", node->type);
    }
}



/* wsection_new:
 */
WNode *wsection_new ( WNode *parent,
                      const gchar *name )
{
  WNode *node = g_new0(WNode, 1);
  node->type = WNODE_TYPE_SECTION;
  node->section.children = g_ptr_array_new();
  if (parent)
    {
      ASSERT(parent->type == WNODE_TYPE_SECTION);
      g_ptr_array_add(parent->section.children, node);
    }
  return node;
}



/* wsection_writef:
 */
void wsection_writef ( WNode *section,
                       const gchar *format,
                       ... )
{
  va_list args;
  ASSERT(section && section->type == WNODE_TYPE_SECTION);
  va_start(args, format);
  wsection_writev(section, format, args);
  va_end(args);
}



/* wsection_writev:
 */
void wsection_writev ( WNode *section,
                       const gchar *format,
                       va_list args )
{
  WNode *node;
  ASSERT(section && section->type == WNODE_TYPE_SECTION);
  node = g_new0(WNode, 1);
  node->type = WNODE_TYPE_TEXT;
  node->text.text = g_strdup_vprintf(format, args);
  g_ptr_array_add(section->section.children, node);
}



/* wsection_add:
 */
void wsection_add ( WNode *section,
                    WNode *first_node,
                    ... )
{
  va_list args;
  WNode *node;
  ASSERT(section->type == WNODE_TYPE_SECTION);
  va_start(args, first_node);
  for (node = first_node; node; node = va_arg(args, WNode *))
    g_ptr_array_add(section->section.children, node);
  va_end(args);
}



/* wtext_newf:
 */
WNode *wtext_newf ( const gchar *format,
                    ... )
{
  WNode *node;
  va_list args;
  va_start(args, format);
  node = g_new0(WNode, 1);
  node->type = WNODE_TYPE_TEXT;
  node->text.text = g_strdup_vprintf(format, args);
  va_end(args);
  return node;
}
