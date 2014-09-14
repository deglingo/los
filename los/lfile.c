/* lfile.c -
 */

#include "los/lfile.h"
#include "los/lfile.inl"



/* l_file_fdopen:
 */
LStream *l_file_fdopen ( gint fd,
                         const gchar *mode,
                         GError **error )
{
  LStream *s;
  s = L_STREAM(l_object_new(L_CLASS_FILE, NULL));
  return s;
}
