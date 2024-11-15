#include <Arduino.h>
#include <unity.h>

#include "lexer/Lexer.h"

Lexer lexer;

void setUp() {
}

void tearDown() {
}

void test_lexer_tokenize() {
  char code[] = "(100 + 2) * 3 + b";
  Lexer::Token* tokens = lexer.tokenize(code, sizeof(code));
  
  TEST_ASSERT_EQUAL(Lexer::TokenType::OpenParen, tokens[0].type);
  TEST_ASSERT_EQUAL(Lexer::TokenType::Number, tokens[1].type);
}
/*
void test_lexer_doubleParen() {
  char code[] = "a + (basdf * (c-d) / e)";
  Lexer::Token* tokens = lexer.tokenize(code, sizeof(code));
  TEST_ASSERT_EQUAL(Lexer::TokenType::OpenParen, tokens[2].type);
  TEST_ASSERT_EQUAL_STRING("basdf", tokens[4].value);  
  TEST_ASSERT_EQUAL(Lexer::TokenType::OpenParen, tokens[5].type);
  TEST_ASSERT_EQUAL(Lexer::TokenType::CloseParen, tokens[9].type);
  TEST_ASSERT_EQUAL(Lexer::TokenType::CloseParen, tokens[12].type);
}*/


void setup() {
  delay(2000);
   
  UNITY_BEGIN();
  RUN_TEST(test_lexer_tokenize);
  //RUN_TEST(test_lexer_doubleParen);
  UNITY_END();
}

void loop() {

}