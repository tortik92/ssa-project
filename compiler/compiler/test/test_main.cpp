#include <Arduino.h>
#include <unity.h>

#include "test_lexer.h"
#include "test_parser.h"
#include "test_values.h"
#include "test_environment.h"
#include "test_nativefn.h"
#include "test_interpreter.h"

void setUp() {}
void tearDown() {}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  // Lexer tests
  RUN_TEST(test_lexer_single_tokens);
  RUN_TEST(test_lexer_numbers);
  RUN_TEST(test_lexer_keywords);
  RUN_TEST(test_lexer_operators);
  RUN_TEST(test_lexer_identifiers);

  // Parser tests
  RUN_TEST(test_parser_const_var_decl);
  RUN_TEST(test_parser_number_var_decl);
  RUN_TEST(test_parser_string_var_decl);
  RUN_TEST(test_parser_object_var_decl);
  RUN_TEST(test_parser_array_var_decl);
  RUN_TEST(test_parser_assignment_expr);

  RUN_TEST(test_parser_if_stmt);
  RUN_TEST(test_parser_if_else_stmt);
  RUN_TEST(test_parser_if_stmt_logical_expr);
  RUN_TEST(test_parser_if_stmt_relational_expr);

  RUN_TEST(test_parser_additive_expr);
  RUN_TEST(test_parser_multiplicative_expr);

  RUN_TEST(test_parser_while_stmt);
  RUN_TEST(test_parser_break_stmt);

  RUN_TEST(test_parser_call_expr);
  RUN_TEST(test_parser_call_expr_args);
  RUN_TEST(test_parser_member_expr);
  RUN_TEST(test_parser_member_expr_computed);

  // Values tests
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
  RUN_TEST(test_values_object_clone);

  // Environment tests
  RUN_TEST(test_environment_global_env);
  RUN_TEST(test_environment_nativefn_random);
  RUN_TEST(test_environment_declare_var);
  RUN_TEST(test_environment_assign_var);
  RUN_TEST(test_environment_lookup_var);
  RUN_TEST(test_environment_resolve_var);
  // RUN_TEST(test_environment_reassign_const_var);
  // RUN_TEST(test_environment_var_not_found);

  // NativeFunctions tests
  RUN_TEST(test_nativefn_print);
  RUN_TEST(test_nativefn_rnd_onearg);
  RUN_TEST(test_nativefn_rnd_twoargs);

  // Interpreter tests
  RUN_TEST(test_interpreter_const_var_decl);
  RUN_TEST(test_interpreter_number_var_decl);
  RUN_TEST(test_interpreter_string_var_decl);
  RUN_TEST(test_interpreter_object_var_decl);
  RUN_TEST(test_interpreter_assignment_expr);

  RUN_TEST(test_interpreter_if_stmt);
  RUN_TEST(test_interpreter_if_else_stmt);
  RUN_TEST(test_interpreter_if_stmt_logical_expr);
  RUN_TEST(test_interpreter_if_stmt_relational_expr);

  RUN_TEST(test_interpreter_additive_expr);
  RUN_TEST(test_interpreter_multiplicative_expr);

  RUN_TEST(test_interpreter_while_stmt);
  RUN_TEST(test_interpreter_break_stmt);

  RUN_TEST(test_interpreter_call_expr);
  RUN_TEST(test_interpreter_obj_member_expr);
  RUN_TEST(test_interpreter_obj_member_expr_computed);
  RUN_TEST(test_interpreter_array_member_expr);

  UNITY_END();
}

void loop() {}