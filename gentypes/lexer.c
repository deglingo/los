/* lexer.c -
 */

#include "lexer.h"
#include "parser.h"
#include "parsergram.h"

#include <string.h>
#include <stdlib.h>



/* Lexer:
 */
struct _Lexer
{
  FILE *file;
  GQuark qfile;
  gint lineno;
  gint charno;
  GString *buffer;
};



/* LexRule:
 */
typedef struct _LexRule
{
  gint token;
  gchar *pattern;
  GRegex *regex;
}
  LexRule;



/* LEX_RULES:
 */
static LexRule LEX_RULES[] =
  {
    { TOK_IDENT, "^[a-zA-Z_][a-zA-Z0-9_]*", },
    { TOK_COLONCOLON, "^::", },
    { '(', "^\\(", },
    { ')', "^\\)", },
    { '{', "^\\{", },
    { '}', "^\\}", },
    { ';', "^;", },
    { ':', "^:", },
    { 0, },
  };

/* location regex */
static GRegex *REG_LOCATION = NULL;

/* keywords */
static GHashTable *keywords = NULL;



/* lex_rules_init:
 */
static void lex_rules_init ( void )
{
  LexRule *rule;
  static gboolean done = FALSE;
  GError *error = NULL;
  if (done)
    return;
  done = TRUE;
  for (rule = LEX_RULES; rule->token != 0; rule++)
    {
      if (!(rule->regex = g_regex_new(rule->pattern, 0, 0, &error)))
        CL_ERROR("regex error: %s", error->message);
    }
  /* locations */
  if (!(REG_LOCATION = g_regex_new("^#[ \t]+([0-9]+)[ \t]+\"([^\"]+)\".*", 0, 0, &error)))
    CL_ERROR("regex error: %s", error->message);
  /* keywords */
  keywords = g_hash_table_new(g_str_hash, g_str_equal);
  g_hash_table_insert(keywords, "namespace", GINT_TO_POINTER(TOK_NAMESPACE));
  g_hash_table_insert(keywords, "class", GINT_TO_POINTER(TOK_CLASS));
  g_hash_table_insert(keywords, "extern", GINT_TO_POINTER(TOK_EXTERN));
  g_hash_table_insert(keywords, "void", GINT_TO_POINTER(TOK_VOID));
}



/* lex_rule_match:
 */
static LexRule *lex_rule_match ( const gchar *str,
                                 gint start,
                                 gint len,
                                 gint *tok_len )
{
  LexRule *rule;
  GError *error = NULL;
  GMatchInfo *match;
  for (rule = LEX_RULES; rule->token; rule++)
    {
      if (g_regex_match_full(rule->regex, str + start, len, 0, 0, &match, &error))
        {
          gint start;
          ASSERT(!error);
          if (!g_match_info_fetch_pos(match, 0, &start, tok_len))
            CL_ERROR("[FIXME] could not fetch match pos");
          ASSERT(start == 0);
          g_match_info_free(match);
          return rule;
        }
      else
        {
          if (error)
            CL_ERROR("regex error: %s", error->message);
          g_match_info_free(match);
        }
    }
  return NULL;
}



/* lexer_new:
 */
Lexer *lexer_new ( FILE *file )
{
  Lexer *lexer = g_new0(Lexer, 1);
  lex_rules_init();
  lexer->file = file;
  lexer->qfile = 0;
  lexer->buffer = g_string_new("");
  return lexer;
}



/* parse_directive:
 */
static void parse_directive ( Lexer *lexer )
{
  GMatchInfo *match;
  GError *error = NULL;
  if (g_regex_match_full(REG_LOCATION, lexer->buffer->str, lexer->buffer->len, 0, 0, &match, &error))
    {
      gchar *slno, *fname;
      gint lno;
      slno = g_match_info_fetch(match, 1);
      ASSERT(slno);
      lno = strtol(slno, NULL, 10);
      fname = g_match_info_fetch(match, 2);
      ASSERT(fname);
      lexer->qfile = g_quark_from_string(fname);
      lexer->lineno = lno;
      g_free(slno);
      g_free(fname);
      g_match_info_free(match);
    }
  else
    {
      if (error)
        CL_ERROR("match error: %s", error->message);
      CL_ERROR("invalid directive line: '%s'", lexer->buffer->str);
    }
}



/* feed_line:
 */
static gboolean feed_line ( Lexer *lexer )
{
  gint c;
  if (lexer->charno < lexer->buffer->len)
    return TRUE;
  lexer->lineno++;
  lexer->charno = 0;
  g_string_truncate(lexer->buffer, 0);
  while (1)
    {
      c = fgetc(lexer->file);
      if (c == EOF)
        {
          if (lexer->buffer->len == 0)
            return FALSE;
          else
            return TRUE;
        }
      else if (c == '\n')
        {
          /* [FIXME] handle other newline chars */
          if (lexer->buffer->len == 0)
            {
              lexer->lineno++;
            }
          else if (lexer->buffer->str[0] == '#')
            {
              parse_directive(lexer);
              g_string_truncate(lexer->buffer, 0);
            }
          else
            {
              return TRUE;
            }
        }
      else
        {
          g_string_append_c(lexer->buffer, c);
        }
    }
}



/* iswhite:
 */
static inline gboolean iswhite ( gint c )
{
  return c == ' ' || c == '\t';
}



/* feed_buffer:
 */
static gboolean feed_buffer ( Lexer *lexer )
{
  while (1)
    {
      if (lexer->charno < lexer->buffer->len)
        {
          if (iswhite(lexer->buffer->str[lexer->charno]))
            {
              lexer->charno++;
            }
          else
            {
              return TRUE;
            }
        }
      else
        {
          if (!feed_line(lexer))
            return FALSE;
        }
    }
}



/* lexer_lex:
 */
gint lexer_lex ( Lexer *lexer,
                 AST **ast,
                 Location *loc )
{
  LexRule *rule;
  gint tok_len;
  gint token;
  if (!feed_buffer(lexer))
    return 0;
  loc->qfile = lexer->qfile;
  loc->lineno = lexer->lineno;
  loc->charno = lexer->charno;
  if (!(rule = lex_rule_match(lexer->buffer->str, lexer->charno, lexer->buffer->len - lexer->charno, &tok_len)))
    CL_ERROR("no match: `%s'", lexer->buffer->str + lexer->charno);
  if (rule->token == TOK_IDENT)
    {
      gchar *id = g_alloca(tok_len+1);
      gpointer kw;
      memcpy(id, lexer->buffer->str + lexer->charno, tok_len);
      id[tok_len] = 0;
      *ast = ast_ident_new(id);
      if ((kw = g_hash_table_lookup(keywords, id)))
        {
          token = GPOINTER_TO_INT(kw);
        }
      else
        {
          token = TOK_IDENT;
        }
    }
  else
    {
      *ast = NULL;
      token = rule->token;
    }
  lexer->charno += tok_len;
  loc->end_qfile = lexer->qfile;
  loc->end_lineno = lexer->lineno;
  loc->end_charno = lexer->charno;
  return token;
}
