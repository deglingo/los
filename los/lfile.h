/* lfile.h -
 */

#ifndef _LFILE_H_
#define _LFILE_H_

#include "los/lstream.h"
#include "los/lfile-def.h"



/* LFile:
 */
struct _LFile
{
  L_FILE_INSTANCE_HEADER;

  gint fd;
};



/* LFileClass:
 */
struct _LFileClass
{
  L_FILE_CLASS_HEADER;
};



LStream *l_file_fdopen ( gint fd,
                         const gchar *mode,
                         GError **error );



#endif
