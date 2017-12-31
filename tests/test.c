//TODO test segments, implemented pipes

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <stdint.h>
#include "../src/pipeline.h"
#include "../src/pipebuilder.h"
#include "../src/parameters.h"
#include "../src/linkedlist.h"
#include "../src/hashtable.h"
#include "../src/piperegistry.h"

static int pipe_setup(void **state)  {
  piperegistry_init();
  *state = build_pipe("DUMMYEMOTIV");
  return 0;
}

static int pipe_teardown(void **state)  {
  pipe_destroy(*state);
  piperegistry_deinit();
  return 0;
}

static void pipe_test_init_run(void **state) {
  pipe_ *p = *state;
  assert_int_equal(pipe_get_status(p), -1);
  int status = pipe_init(p, NULL);
  assert_int_equal(status, 1);  //init success
  assert_int_equal(pipe_get_status(p), 0);  //status set to init
  assert_non_null(pipe_get_output(p));  //data structure allocated
  pipe_run(p, NULL);
  assert_int_equal(pipe_get_status(p), 1);  //run success
}

static void pipe_test_parameters(void **state)  {
  pipe_ *p = *state;
  assert_int_equal(pipe_get_params_n(p), 0);
  
  pipe_ *p1 = build_pipe("DUMMYEMOTIV;var1=1");
  assert_int_equal(pipe_get_params_n(p1), 1);
  assert_string_equal(pipe_get_params(p1)[0], "var1=1");  
  pipe_destroy(p1);

  pipe_ *p2 = build_pipe("DUMMYEMOTIV;var1=1,var2=2");
  assert_int_equal(pipe_get_params_n(p2), 2);   
  assert_string_equal(pipe_get_params(p2)[0], "var1=1");
  assert_string_equal(pipe_get_params(p2)[1], "var2=2");
  pipe_destroy(p2);
}

static int pipeline_setup(void **state)  {
  *state = pipeline_create();
  return 0;
}

static int pipeline_teardown(void **state)  {
  pipeline_destroy(*state);
  return 0;
}

static void pipeline_test_add_pipe(void **state)  {
  pipeline *pl = *state;
  int id = -1;

  assert_int_equal(pipeline_size(pl), 0);

  id = pipeline_add(pl, "DUMMYEMOTIV", 0); 
  assert_int_equal(id, 0);
  
  assert_int_equal(pipeline_size(pl), 1);
  
  id = pipeline_add(pl, "DUMMYCOMPUTATION", 0); 
  assert_int_equal(id, 1);
  
  assert_int_equal(pipeline_size(pl), 2);
}

static void pipeline_test_init(void **state)  {
  pipeline *pl = *state;

  int input = pipeline_add(pl, "DUMMYEMOTIV", 0);
  int computation = pipeline_add(pl, "DUMMYCOMPUTATION", 0);
  pipeline_link(pl, input, computation);

  int n = pipeline_size(pl);
  
  //not init
  for (int i = 0; i < n; i++)  {
 //   assert_int_equal(pipeline_get_pipe(pl, i)->status, -1);
  }

  //init
//  pipeline_init(pl);
  for (int i = 0; i < n; i++)  {
   // assert_int_equal(pipeline_get_pipe(pl, i)->status, 0);
  }
}

//pipeline test invalid linking

static int data_setup(void **state)  {
  *state = data_create_from_string("EMOTIV");
  return 0;
}

static int data_teardown(void **state)  {
  data_destroy(*state);
  return 0;
}


static void data_test_create(void ** state)  {
  data *d = *state;

  assert_non_null(d);

  int n = data_get_len(d);
  int count = 0;
  for (int i = 0; i < data_get_shape(d)[0]; i++)  {
    for (int j = 0; j < data_get_shape(d)[1]; j++)  {
      count++;
    }
  }
  assert_int_equal(count, n);
}

static void data_test_create_from(void **state)  {
  data *d = *state;
  
  data *d2 = data_create_from(d);

  assert_int_equal(data_get_len(d), data_get_len(d2));

  int n = data_get_n(d);

  for (int i = 0; i < n; i++)  {
    assert_int_equal(data_get_shape(d)[i], data_get_shape(d2)[i]);
    assert_int_equal(data_get_stride(d)[i], data_get_stride(d2)[i]);
  }

  data_destroy(d2);
}

static void data_test_blocking(void **state)  {
  data *d = *state;
 
  assert_int_equal(data_get_blocking(d), 0);  

  data_make_blocking(d);
  
  assert_int_equal(data_get_blocking(d), 1);  
}

static void data_test_copy(void **state)  {
  data *d = *state;

  int n = data_get_len(d);

  double *input_buffer = (double*)malloc(sizeof(double)*n);

  for (int i = 0; i < n; i++)  {
    input_buffer[i] = i;
  }

  data_copy_to_data(d, input_buffer);

  assert_memory_equal(input_buffer, data_get_buffer(d), data_size(d));  //check internal representation

  double *output_buffer = (double*)malloc(sizeof(double)*n);

  data_copy_from_data(d, output_buffer);

  assert_memory_equal(input_buffer, output_buffer, data_size(d));  //check copying from and to buffer

  free(input_buffer);
  free(output_buffer);
}

static int builder_setup(void **state)  {
  piperegistry_init();
  return 0;
}

static int builder_teardown(void **state)  {
  piperegistry_deinit();
  return 0;
}

static void parameters_test_tokenise(void **state)  {
  char spec[] = "TYPE;var1=val1,var2=val2";
  char *type;
  char **params;
  int params_n;
  tokenise(spec, &type, &params_n, &params);

  assert_string_equal(type, "TYPE");
  assert_int_equal(params_n, 2);
  assert_string_equal(params[0], "var1=val1");
  assert_string_equal(params[1], "var2=val2");

  free(type);
  free(params);
}

static void parameters_test_get_parameter(void **state)  {
  pipe_ *p = build_pipe("EMOTIV;var1=val1,var2=val2");

  assert_string_equal(get_parameter(p, "var1"), "val1");
  assert_string_equal(get_parameter(p, "var2"), "val2");
  assert_null(get_parameter(p, "var3"));
}

static void pipebuilder_test_invalid_spec(void **state)  {
  assert_null(build_pipe("INVALID"));
}

static int linkedlist_setup(void **state)  {
  *state = linkedlist_create();
   return 0;
}

static int linkedlist_teardown(void **state)  {
  *state = linkedlist_create();
  return 0;
}

static void linkedlist_test_insert_remove_clear(void **state)  {
  linkedlist *l = *state;

  assert_int_equal(linkedlist_size(l), 0);

  linkedlist_insert(l, (void*)0);
  linkedlist_insert(l, (void*)1);
  linkedlist_insert(l, (void*)2);
  linkedlist_insert(l, (void*)3);
   
  assert_int_equal(linkedlist_size(l), 4);

  linkedlist_clear(l);

  assert_int_equal(linkedlist_size(l), 0);
}

static void linkedlist_test_head_tail(void **state)  {
  linkedlist *l = *state;
  
  linkedlist_insert(l, (void*)"head");
  linkedlist_insert(l, (void*)"tail");

  assert_string_equal(linkedlist_head(l), "head");
  assert_string_equal(linkedlist_tail(l), "tail");
}

static void linkedlist_test_iterater(void **state)  {
  linkedlist *l = *state;
 
  for (int i = 0; i < 10; i++)  { 
    linkedlist_insert(l, (void*)(intptr_t)i);
  }

  int i = 0;
  char *str = linkedlist_iterate(l);
  while (str != NULL)  {
    assert_int_equal(atoi(str), i++);
  }
}

static int hashtable_setup(void **state)  {
  *state = hashtable_create();
  return 0;
}

static int hashtable_teardown(void **state)  {
  hashtable_destroy(*state);
  return 0;
}

static void hashtable_test_insert(void **state)  {
  hashtable *ht = *state;
  int success = -1;
  success = hashtable_insert(ht, "key", "val1");
  
  assert_int_equal(success, 1);

  success = hashtable_insert(ht, "key", "val2");

  assert_int_equal(success, 0);
}

static void hashtable_test_lookup(void **state)  {  //separate into subtests?
  hashtable *ht = hashtable_create();

  hashtable_insert(ht, "key1", "val1");
  hashtable_insert(ht, "key2", "val2");
 
  assert_string_equal((char*)hashtable_lookup(ht, "key1"), "val1");
  assert_string_equal((char*)hashtable_lookup(ht, "key2"), "val2");

  assert_null(hashtable_lookup(ht, "key3"));
}

int main()  {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(pipeline_test_add_pipe, pipeline_setup, pipeline_teardown),
    cmocka_unit_test_setup_teardown(pipeline_test_init, pipeline_setup, pipeline_teardown),
    cmocka_unit_test_setup_teardown(pipe_test_init_run, pipe_setup, pipe_teardown),
    cmocka_unit_test_setup_teardown(pipe_test_parameters, pipe_setup, pipe_teardown),
    cmocka_unit_test_setup_teardown(data_test_create, data_setup, data_teardown),
    cmocka_unit_test_setup_teardown(data_test_create_from, data_setup, data_teardown),
    cmocka_unit_test_setup_teardown(data_test_blocking, data_setup, data_teardown),
    cmocka_unit_test_setup_teardown(data_test_copy, data_setup, data_teardown),
    cmocka_unit_test_setup_teardown(parameters_test_tokenise, builder_setup, builder_teardown),
    cmocka_unit_test_setup_teardown(parameters_test_get_parameter, builder_setup, builder_teardown),
    cmocka_unit_test_setup_teardown(pipebuilder_test_invalid_spec, builder_setup, builder_teardown),
    cmocka_unit_test_setup_teardown(linkedlist_test_insert_remove_clear, linkedlist_setup, linkedlist_teardown),
    cmocka_unit_test_setup_teardown(linkedlist_test_head_tail, linkedlist_setup, linkedlist_teardown),
    cmocka_unit_test_setup_teardown(linkedlist_test_iterater, linkedlist_setup, linkedlist_teardown),
    cmocka_unit_test_setup_teardown(hashtable_test_insert, hashtable_setup, hashtable_teardown),
    cmocka_unit_test_setup_teardown(hashtable_test_lookup, hashtable_setup, hashtable_teardown),
  };
  cmocka_run_group_tests(tests, NULL, NULL);
  return 0;
}
