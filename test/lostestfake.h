/* lostestfake.h - some fake class to test
 */

#ifndef _LOSTESTFAKE_H_
#define _LOSTESTFAKE_H_

#define CLASS_FAKE (fake_get_class())
#define CLASS_FAKE2 (fake2_get_class())



/* Fake */
typedef struct _FakeClass
{
  LObjectClass super_class;
  int dummy;
}
  FakeClass;

static void _fake_class_init ( LObjectClass *cls )
{
  ((FakeClass *) cls)->dummy = 42;
}

static LObjectClass *fake_get_class ( void )
{
  static LObjectClass *cls = NULL;
  if (!cls) {
    LClassInfo info = { 0, };
    info.class_size = sizeof(FakeClass);
    info.class_init = _fake_class_init;
    cls = l_object_class_register("Fake", L_CLASS_OBJECT, &info);
  }
  return cls;
}

/* Fake2Class */
typedef struct _Fake2Class
{
  FakeClass super_class;
}
  Fake2Class;

static void _fake2_class_init ( LObjectClass *cls )
{
  ((FakeClass *) cls)->dummy *= 2;
}

static LObjectClass *fake2_get_class ( void )
{
  static LObjectClass *cls = NULL;
  if (!cls) {
    LClassInfo info = { 0, };
    info.class_size = sizeof(Fake2Class);
    info.class_init = _fake2_class_init;
    cls = l_object_class_register("Fake2", CLASS_FAKE, &info);
  }
  return cls;
}



#endif
