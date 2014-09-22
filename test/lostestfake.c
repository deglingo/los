
#include "test/lostestfake.h"


gint fake_dispose_count = 0;
gint fake_finalize_count = 0;


static void _fake_dispose ( LObject *obj )
{
  fake_dispose_count++;
}

static void _fake_finalize ( LObject *obj )
{
  fake_finalize_count++;
}

static void _fake_class_init ( LObjectClass *cls )
{
  cls->dispose = _fake_dispose;
  cls->finalize = _fake_finalize;
  ((FakeClass *) cls)->dummy = 42;
}

static void _fake_init ( LObject *obj )
{
  ((Fake *) obj)->instval = 3;
}

LObjectClass *fake_get_class ( void )
{
  static LObjectClass *cls = NULL;
  if (!cls) {
    LClassInfo info = { 0, };
    info.class_size = sizeof(FakeClass);
    info.class_init = _fake_class_init;
    info.instance_size = sizeof(Fake);
    info.init = _fake_init;
    cls = l_object_class_register("Fake", L_CLASS_OBJECT, &info);
  }
  return cls;
}

static void _fake2_class_init ( LObjectClass *cls )
{
  ((FakeClass *) cls)->dummy *= 2;
}

static void _fake2_init ( LObject *obj )
{
  ((Fake *) obj)->instval *= 2;
}

LObjectClass *fake2_get_class ( void )
{
  static LObjectClass *cls = NULL;
  if (!cls) {
    LClassInfo info = { 0, };
    info.class_size = sizeof(Fake2Class);
    info.class_init = _fake2_class_init;
    info.instance_size = sizeof(Fake2);
    info.init = _fake2_init;
    cls = l_object_class_register("Fake2", CLASS_FAKE, &info);
  }
  return cls;
}

