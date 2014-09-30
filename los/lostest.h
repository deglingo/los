/* lostest.h -
 */

#ifndef _LOSTEST_H_
#define _LOSTEST_H_



#define LOS_CHECK_INT(obj_, val) do {           \
    LObject *obj = (obj_);                      \
    PIF_CHECK(obj);                             \
    PIF_CHECK(L_IS_INT(obj));                   \
    PIF_CHECK_EQ(L_INT_VALUE(obj), (val));      \
  } while (0)



#endif
