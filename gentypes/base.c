/* base.c -
 */

#include "base.h"



/* printf_len:
 */
gsize printf_len ( const gchar *format,
                   ... )
{
  va_list args;
  gsize size;
  va_start(args, format);
  size = g_printf_string_upper_bound(format, args);
  va_end(args);
  return size;
}
