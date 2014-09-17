/* lpackerbase.h -
 */

#ifndef _LPACKERBASE_H_
#define _LPACKERBASE_H_

#include "los/lbase.h"



#define L_PACK_ERROR (l_pack_error_quark())



/* LPackError:
 */
typedef enum _LPackError
  {
    L_PACK_ERROR_EOF,
  }
  LPackError;



GQuark l_pack_error_quark ( void );



#endif
