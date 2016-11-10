#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include "../src/pipeline.h"

static int pipeline_setup(void **state)  {
  *state = pipeline_create();
  return 0;
}

static int pipeline_teardown(void **state)  {
  pipeline_destroy(*state);
  return 0;
}

static void pipeline_add_pipe_test(void **state)  {
  pipeline *pl = *state;

  assert_int_equal(pipeline_size(pl), 0);

  pipeline_insert(pl, "FOURIERTRANSFORM");  //dummy pipe?
  
  assert_int_equal(pipeline_size(pl), 1);
  
  pipeline_insert(pl, "FOURIERTRANSFORM");  //dummy pipe?
  pipeline_insert(pl, "FOURIERTRANSFORM");  //dummy pipe?
  pipeline_insert(pl, "FOURIERTRANSFORM");  //dummy pipe?
  
  assert_int_equal(pipeline_size(pl), 4);
}

int main()  {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(pipeline_add_pipe_test, pipeline_setup, pipeline_teardown),
  };
  cmocka_run_group_tests(tests, NULL, NULL);
  return 0;
}
