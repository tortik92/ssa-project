#pragma once

#include <queue>
#include <Arduino.h>
#include "Constants.h"
#include "ErrorHandler.h"

/**
 * @class Lexer
 * @brief A class for tokenizing source code into various token types.
 *
 * This class is used to convert source code into a sequence of tokens that can be
 * processed by a parser. It supports various token types, including keywords,
 * operators, literals, and more.
 */
class Lexer {
public:
  /**
   * @enum TokenType
   * @brief An enumeration of different types of tokens in the source code.
   *
   * This enum lists all possible types of tokens that can be recognized during
   * the tokenization process, such as keywords, operators, and literals.
   */
  enum class TokenType {
    Let,    ///< Token for the 'let' keyword
    Const,  ///< Token for the 'const' keyword
    If,     ///< Token for the 'if' keyword
    Else,   ///< Token for the 'else' keyword
    While,  ///< Token for the 'while' keyword
    Break,  ///< Token for the 'break' keyword

    Identifier,     ///< Token for identifiers (variable names, etc.)
    Number,         ///< Token for numerical literals
    StringLiteral,  ///< Token for string literals

    Equals,              ///< Token for '=' operator
    ArithmeticOperator,  ///< Token for arithmetic operators (+, -, *, /, %)
    RelationalOperator,  ///< Token for relational operators (<, <=, >, >=, ==, !=)
    LogicalOperator,     ///< Token for logical operators (not, and, or)

    OpenParen,     ///< Token for '(' character
    CloseParen,    ///< Token for ')' character
    OpenBracket,   ///< Token for '[' character
    CloseBracket,  ///< Token for ']' character
    OpenBrace,     ///< Token for '{' character
    CloseBrace,    ///< Token for '}' character

    Semicolon,  ///< Token for ';' character
    Colon,      ///< Token for ':' character
    Comma,      ///< Token for ',' character
    Dot,        ///< Token for '.' character

    EndOfFile,  ///< Token indicating the end of the file
  };

  /**
   * @struct Token
   * @brief A structure representing a token in the source code.
   *
   * A token consists of a string value and a type that indicates what kind of
   * token it is (e.g., identifier, keyword, operator).
   */
  typedef struct Token {
    char* value;     ///< The value of the token (e.g., the string representation of the identifier)
    TokenType type;  ///< The type of the token (e.g., keyword, operator)

    /**
     * @brief Default constructor.
     *
     * Initializes the token with an empty value and an Identifier type.
     */
    Token()
      : value(nullptr), type(TokenType::Identifier) {}

    /**
     * @brief Parameterized constructor.
     * @param _value The value of the token.
     * @param _type The type of the token.
     */
    Token(char* _value, TokenType _type)
      : value(_value), type(_type) {}

    /**
     * @brief Parameterized constructor.
     * @param _value The value of the token (will be copied).
     * @param _type The type of the token.
     */
    Token(const char* _value, TokenType _type)
      : type(_type) {
      if (_value) {
        size_t strLen = strlen(_value) + 1;
        value = new char[strLen];
        strcpy(value, _value);
        value[strLen] = '\0';
      } else {
        value = nullptr;
      }
    }

    /**
     * @brief Copy constructor.
     * @param other The token to copy from.
     */
    Token(const Token& other)
      : type(other.type) {
      if (other.value) {
        size_t strLen = strlen(other.value) + 1;
        value = new char[strLen];
        strcpy(value, other.value);
        value[strLen] = '\0';
      } else {
        value = nullptr;
      }
    }

    /**
     * @brief Move constructor.
     * @param other The token to move from.
     */
    Token(Token&& other) noexcept
      : value(other.value), type(other.type) {
      other.value = nullptr;  // Transfer ownership
    }

    /**
     * @brief Move assignment operator.
     * @param other THe token to move from.
     */
    Token& operator=(Token&& other) noexcept {
      if (this != &other) {
        delete[] value;       // Free existing memory
        value = other.value;  // Transfer ownership
        type = other.type;
        other.value = nullptr;
      }
      return *this;
    }


    /**
     * @brief Destructor.
     * Frees the memory allocated for the token's value.
     */
    ~Token() {
      delete[] value;
    }
  } Token;

  /**
   * @brief Tokenizes the given source code.
   * @param code The source code to tokenize.
   * @param len The length of the source code.
   * @return A queue of tokens parsed from the source code.
   *
   * This method processes the input `code` character by character and generates
   * a sequence of tokens, which are returned as a queue of `Token` objects.
   */
  std::queue<Token> tokenize(char* code, size_t len);

private:
  std::queue<Token> tokens;  ///< A queue to hold the tokens generated during tokenization

  /**
   * @brief Array of known keywords and their corresponding token types.
   */
  const Token keywords[keywordCount] = {
    Token("let", TokenType::Let),
    Token("const", TokenType::Const),
    Token("if", TokenType::If),
    Token("else", TokenType::Else),
    Token("while", TokenType::While),
    Token("break", TokenType::Break),
    Token("and", TokenType::LogicalOperator),
    Token("or", TokenType::LogicalOperator),
  };

  /**
   * @brief Adds a token to the queue.
   * @param src The source string representing the token.
   * @param srcLen The length of the source string.
   * @param tokenType The type of the token.
   *
   * This method creates a token from the given `src` and `tokenType` and adds it
   * to the `tokens` queue. If the queue is full, it triggers an error.
   */
  void addToken(const char* src, size_t srcLen, TokenType tokenType);

  void unrecognizedCharacter(char c);
};
