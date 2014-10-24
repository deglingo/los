/* cident.h -
 */

#ifndef _CIDENT_H_
#define _CIDENT_H_

#include "base.h"



typedef struct _CName CName;
typedef struct _CIdent CIdent;



/* CIdentType:
 */
typedef enum _CIdentType
  {
    C_IDENT_TYPE_CML,
    C_IDENT_TYPE_VAR,
  }
  CIdentType;



/* CName:
 */
struct _CName
{
  gchar *cml;
  gchar *var;
  gchar *cst;
};



/* CIdent:
 */
struct _CIdent
{
  gchar *orig;
  CName pfx;
  CName sfx;
  CName full;
  gchar *fname;
};



CIdent *c_ident_new ( CIdentType type,
                      const gchar *orig,
                      CIdent *base );



#endif
