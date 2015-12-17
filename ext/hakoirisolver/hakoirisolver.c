#include <stdio.h>
#include <ruby.h>

init_HakoiriSolver(){
  VALUE csolver;
  csolver = rb_define_class("HakoiriSolver", rb_cObject);
  rb_define_method(csolver, "hello", hello, 0);
}

VALUE hello(void) {
  printf("Hello\n");
  return Qnil;
}
