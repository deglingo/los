/* ldict.h -
 */

#ifndef _LDICT_H_
#define _LDICT_H_

#include "los/lobject.h"
#include "los/ldict-def.h"



/* LDict:
 */
struct _LDict
{
  L_DICT_INSTANCE_HEADER;
};



/* LDictClass:
 */
struct _LDictClass
{
  L_DICT_CLASS_HEADER;
};



LDict *l_dict_new ( void );



#endif
