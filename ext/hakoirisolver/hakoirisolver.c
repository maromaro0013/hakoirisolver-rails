#include <ruby.h>
#include "hakoirisolver.h"


VALUE hello(void);

Init_hakoirisolver(){
  VALUE csolver;
  csolver = rb_define_class("HakoiriSolver", rb_cObject);
  rb_define_method(csolver, "hello", hello, 0);
}

VALUE hello(void) {
  printf("Hello\n");
  return INT2FIX(cFIELD_SIZE_LIMIT);
}
