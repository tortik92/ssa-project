#include <Arduino.h>
#include <unity.h>
#include "lexer/Lexer.h"

Lexer lexer;

void setUp() {}
void tearDown() {}

void test_lexer_single_tokens() {
  char code[] = "{}()[]";
  std::queue<Lexer::Token> tokens = lexer.tokenize(code, sizeof(code));
  TEST_ASSERT_EQUAL_INT(7, tokens.size());
  TEST_ASSERT_EQUAL(Lexer::TokenType::OpenBrace, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("{", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::CloseBrace, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("}", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::OpenParen, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("(", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::CloseParen, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING(")", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::OpenBracket, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("[", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::CloseBracket, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("]", tokens.front().value);
}

void test_lexer_numbers() {
  char code[] = "123 4567";
  std::queue<Lexer::Token> tokens = lexer.tokenize(code, sizeof(code));
  TEST_ASSERT_EQUAL(Lexer::TokenType::Number, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("123", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::Number, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("4567", tokens.front().value);
}

void test_lexer_keywords() {
  char code[] = "let const if else while break";
  std::queue<Lexer::Token> tokens = lexer.tokenize(code, sizeof(code));
  TEST_ASSERT_EQUAL(Lexer::TokenType::Let, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("let", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::Const, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("const", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::If, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("if", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::Else, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("else", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::While, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("while", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::Break, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("break", tokens.front().value);
}

void test_lexer_operators() {
  char code[] = "+ - * / %";
  std::queue<Lexer::Token> tokens = lexer.tokenize(code, sizeof(code));
  TEST_ASSERT_EQUAL(Lexer::TokenType::ArithmeticOperator, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("+", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::ArithmeticOperator, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("-", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::ArithmeticOperator, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("*", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::ArithmeticOperator, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("/", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::ArithmeticOperator, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("%", tokens.front().value);
}

void test_lexer_identifiers() {
  char code[] = "var1 myVariable another_One test123";
  std::queue<Lexer::Token> tokens = lexer.tokenize(code, sizeof(code));
  TEST_ASSERT_EQUAL(Lexer::TokenType::Identifier, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("var1", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::Identifier, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("myVariable", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::Identifier, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("another_One", tokens.front().value);
  tokens.pop();
  TEST_ASSERT_EQUAL(Lexer::TokenType::Identifier, tokens.front().type);
  TEST_ASSERT_EQUAL_STRING("test123", tokens.front().value);
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_lexer_single_tokens);
  RUN_TEST(test_lexer_numbers);
  RUN_TEST(test_lexer_keywords);
  RUN_TEST(test_lexer_operators);
  RUN_TEST(test_lexer_identifiers);
  UNITY_END();
}

void loop() {}
