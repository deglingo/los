/* base.h -
 */

#ifndef _BASE_H_
#define _BASE_H_

#include <glib.h>
#include <clog.h>
#include <stdio.h>

G_BEGIN_DECLS



/* ASSERT:
 */
#define ASSERT(expr) do {                           \
    if (!(expr)) {                                  \
      CL_ERROR("ASSERTION FAILED: `" #expr "'");    \
    }                                               \
  } while (0)

#define ASSERTF(expr, fmt, args...) do {                            \
    if (!(expr)) {                                                  \
      CL_ERROR("ASSERTION FAILED: `" #expr "' (" fmt ")", args);    \
    }                                                               \
  } while (0)



/* Location:
 */
typedef struct _Location
{
  GQuark qfile;
  gint lineno;
  gint charno;
  GQuark end_qfile;
  gint end_lineno;
  gint end_charno;
}
  Location;



/* APRINTF:
 */
gsize printf_len ( const gchar *format,
                   ... )
  G_GNUC_PRINTF(1, 2);

#define APRINTF(dest, fmt, args...) do {            \
    gsize _aprintf_size = printf_len(fmt, args);    \
    *dest = g_alloca(_aprintf_size + 1);            \
    sprintf(*dest, fmt, args);                      \
  } while (0)



G_END_DECLS

#endif
