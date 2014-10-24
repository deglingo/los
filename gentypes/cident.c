/* cident.h -
 */

#include "cident.h"



static inline gboolean _isupper ( gchar c ) { return c >= 'A' && c <= 'Z'; }
static inline gboolean _islower ( gchar c ) { return c >= 'a' && c <= 'z'; }
static inline gboolean _isdigit ( gchar c ) { return c >= '0' && c <= '9'; }
static inline gchar _tolower ( gchar c ) { return _isupper(c) ? (c + ('a' - 'A')) : c; }
static inline gchar _toupper ( gchar c ) { return _islower(c) ? (c + ('A' - 'a')) : c; }



/* cml_next:
 */
static const gchar *cml_next ( const gchar *name )
{
  const gchar *end = name;
  while (_isupper(*end))
    end++;
  while (_islower(*end) || _isdigit(*end))
    end++;
  return end;
}



/* cml_split:
 */
static void cml_split ( const gchar *name,
                        gchar **cml_,
                        gchar **var_,
                        gchar **cst_ )
{
  const gchar *start, *end, *c;
  gboolean first_word = TRUE;
  GString *cml = g_string_new("");
  GString *var = g_string_new("");
  GString *cst = g_string_new("");
  start = name;
  while (*start)
    {
      end = cml_next(start);
      ASSERTF(end > start, "start='%s'", start);
      if (first_word) {
        first_word = FALSE;
      } else {
        g_string_append_c(var, '_');
        g_string_append_c(cst, '_');
      }
      for (c = start; c < end; c++) {
        g_string_append_c(cml, *c);
        g_string_append_c(var, _tolower(*c));
        g_string_append_c(cst, _toupper(*c));
      }
      start = end;
    }
  *cml_ = g_string_free(cml, FALSE);
  *var_ = g_string_free(var, FALSE);
  *cst_ = g_string_free(cst, FALSE);
}



/* var_split:
 */
static void var_split ( const gchar *name,
                        gchar **cml_,
                        gchar **var_,
                        gchar **cst_ )
{
  GString *cml = g_string_new("");
  GString *var = g_string_new("");
  GString *cst = g_string_new("");
  const gchar *c;
  gboolean start = TRUE;
  for (c = name; *c; c++)
    {
      if ((*c) == '_') {
        start = TRUE;
        g_string_append_c(var, '_');
        g_string_append_c(cst, '_');
        continue;
      }
      if (start) {
        g_string_append_c(cml, _toupper(*c));
        start = FALSE;
      } else {
        g_string_append_c(cml, _tolower(*c));
      }
      start = FALSE;
      g_string_append_c(var, _tolower(*c));
      g_string_append_c(cst, _toupper(*c));
    }
  *cml_ = g_string_free(cml, FALSE);
  *var_ = g_string_free(var, FALSE);
  *cst_ = g_string_free(cst, FALSE);
}



/* c_ident_new:
 */
CIdent *c_ident_new ( CIdentType type,
                      const gchar *orig,
                      CIdent *base )
{
  CIdent *id;
  id = g_new0(CIdent, 1);
  switch (type) {
  case C_IDENT_TYPE_CML:
    cml_split(orig, &id->sfx.cml, &id->sfx.var, &id->sfx.cst);
    break;
  case C_IDENT_TYPE_VAR:
    var_split(orig, &id->sfx.cml, &id->sfx.var, &id->sfx.cst);
    break;
  default:
    CL_ERROR("[TODO] %d", type);
  }
  if (base && base->full.cml[0])
    {
      id->pfx.cml = g_strdup(base->full.cml);
      id->pfx.var = g_strdup_printf("%s_", base->full.var);
      id->pfx.cst = g_strdup_printf("%s_", base->full.cst);
      id->full.cml = g_strdup_printf("%s%s", id->pfx.cml, id->sfx.cml);
      id->full.var = g_strdup_printf("%s%s", id->pfx.var, id->sfx.var);
      id->full.cst = g_strdup_printf("%s%s", id->pfx.cst, id->sfx.cst);
    }
  else
    {
      id->pfx.cml = g_strdup("");
      id->pfx.var = g_strdup("");
      id->pfx.cst = g_strdup("");
      id->full.cml = g_strdup(id->sfx.cml);
      id->full.var = g_strdup(id->sfx.var);
      id->full.cst = g_strdup(id->sfx.cst);
    }
  id->fname = g_ascii_strdown(id->full.cml, -1);
  /* CL_DEBUG("IDENT: '%s' (%s)", orig, (base ? base->full.cml : "NULL")); */
  /* CL_DEBUG("  cml: '%s' + '%s' -> '%s'", id->pfx.cml, id->sfx.cml, id->full.cml); */
  /* CL_DEBUG("  var: '%s' + '%s' -> '%s'", id->pfx.var, id->sfx.var, id->full.var); */
  /* CL_DEBUG("  cst: '%s' + '%s' -> '%s'", id->pfx.cst, id->sfx.cst, id->full.cst); */
  /* CL_DEBUG("  fnm: '%s'", id->fname); */
  return id;
}
