#include <unity.h>
#include <ctime>

#include "lexer/Lexer.hpp"

Lexer* lexer;
char code[] = "(1 + 2) * 3 + b";

void setUp() {
  lexer = new Lexer();
}

void tearDown() {
  delete lexer;
}

void test_lexer_tokenize() {
  size_t len = 10;
  Lexer::Token* tokens = lexer->tokenize(code, len);
  TEST_ASSERT_EQUAL(Lexer::TokenType::OpenParen, tokens[0].type);
}

void test_add() {
  TEST_ASSERT_EQUAL(4, 2 + 2);
}

int main() {
  clock_t start_time = clock();
  while(clock() < start_time + 4000 * (CLOCKS_PER_SEC/1000));
   
  UNITY_BEGIN();
  RUN_TEST(test_add);
  UNITY_END();
}