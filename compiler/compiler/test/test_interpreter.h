#pragma once

#include <unity.h>
#include "parser/AstNodes.h"
#include "parser/Parser.h"
#include "interpreter/Interpreter.h"

void test_interpreter_const_var_decl() {
  char code[] = "const x = 42;";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(42, static_cast<Values::NumberVal *>(val.get())->value);
}

void test_interpreter_number_var_decl() {
  char code[] = "let x = 42;";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(42, static_cast<Values::NumberVal *>(val.get())->value);
}

void test_interpreter_string_var_decl() {
  char code[] = "let x = \"hello\";";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL_STRING("hello", static_cast<Values::StringVal *>(val.get())->str);
}

void test_interpreter_object_var_decl() {
  char code[] = "let x = {name: \"John\", age: 30};";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  Values::ObjectVal *obj = static_cast<Values::ObjectVal *>(val.get());
  TEST_ASSERT_EQUAL_STRING("John", static_cast<Values::StringVal *>(obj->properties["name"].get())->str);
  TEST_ASSERT_EQUAL(30, static_cast<Values::NumberVal *>(obj->properties["age"].get())->value);
}

void test_interpreter_assignment_expr() {
  char code[] = "let x = 42; x = 100;";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(100, static_cast<Values::NumberVal *>(val.get())->value);
}

void test_interpreter_if_stmt() {
  char code[] = "let x = 42; if (x == 42) { x = 100; }";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(100, static_cast<Values::NumberVal *>(val.get())->value);
}

void test_interpreter_if_else_stmt() {
  char code[] = "let x = 42; if (x != 42) { x = 100; } else { x = 200; }";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(200, static_cast<Values::NumberVal *>(val.get())->value);
}
void test_interpreter_if_stmt_logical_expr() {
  char code[] = "let x = 42; if (x == 42 and x != 100) { x = 200; }";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(200, static_cast<Values::NumberVal *>(val.get())->value);
}
void test_interpreter_if_stmt_relational_expr() {
  char code[] = "let x = 42; if (x > 40) { x = 200; }";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(200, static_cast<Values::NumberVal *>(val.get())->value);
}
void test_interpreter_additive_expr() {
  char code[] = "let x = 42 + 100;";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(142, static_cast<Values::NumberVal *>(val.get())->value);
}
void test_interpreter_multiplicative_expr() {
  char code[] = "let x = 42 * 100;";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(4200, static_cast<Values::NumberVal *>(val.get())->value);
}
void test_interpreter_while_stmt() {
  char code[] = "let x = 0; while (x < 10) { x = x + 1; }";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(10, static_cast<Values::NumberVal *>(val.get())->value);
}
void test_interpreter_break_stmt() {
  char code[] = "let x = 0; while (x < 10) { x = x + 1; if (x == 5) { break; } }";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(5, static_cast<Values::NumberVal *>(val.get())->value);
}
void test_interpreter_call_expr() {
  char code[] = "let x = random(42);";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL(Values::ValueType::Number, val->type);
  int rndVal = static_cast<Values::NumberVal *>(val.get())->value;
  TEST_ASSERT_GREATER_OR_EQUAL(0, rndVal);
  TEST_ASSERT_LESS_THAN(42, rndVal);
}

void test_interpreter_member_expr() {
  char code[] = "let x = {name: \"John\", age: 30}; let y = x.name;";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL_STRING("John", static_cast<Values::StringVal *>(val.get())->str);
}

void test_interpreter_member_expr_computed() {
  char code[] = "let x = {name: \"John\", age: 30}; let y = x[\"name\"];";
  Parser parser;
  Environment env;
  Interpreter interpreter;

  AstNodes::Program *program = parser.produceAST(code, sizeof(code) - 1);

  std::unique_ptr<Values::RuntimeVal> val = interpreter.evaluate(program, &env);
  TEST_ASSERT_EQUAL_STRING("John", static_cast<Values::StringVal *>(val.get())->str);
}