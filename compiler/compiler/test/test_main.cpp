#include <Arduino.h>
#include <unity.h>

#include "test_lexer.h"
#include "test_values.h"

void setUp() {}
void tearDown() {}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_lexer_single_tokens);
  RUN_TEST(test_lexer_numbers);
  RUN_TEST(test_lexer_keywords);
  RUN_TEST(test_lexer_operators);
  RUN_TEST(test_lexer_identifiers);

  RUN_TEST(test_values_null_constructor);
  RUN_TEST(test_values_boolean_default_constructor);
  RUN_TEST(test_values_boolean_parameterized_constructor);
  RUN_TEST(test_values_number_default_constructor);
  RUN_TEST(test_values_number_parameterized_constructor);
  RUN_TEST(test_values_string_default_constructor);
  RUN_TEST(test_values_string_parameterized_constructor);
  RUN_TEST(test_values_string_copy_constructor);
  RUN_TEST(test_values_string_move_constructor);
  RUN_TEST(test_values_object_default_constructor);
  RUN_TEST(test_values_object_insertion);
  RUN_TEST(test_values_object_move_constructor);
  UNITY_END();
}

void loop() {}