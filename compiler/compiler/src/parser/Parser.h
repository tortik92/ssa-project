#pragma once

#include <map>
#include <memory>
#include <vector>
#include "lexer/Lexer.h"
#include "ErrorHandler.h"

/**
 * @class Parser
 * 
 * A class responsible for parsing source code into an Abstract Syntax Tree (AST).
 * This class reads a tokenized input from the lexer and produces a hierarchical representation of the program's structure.
 */
class Parser {
public:
  /**
   * @brief Constructor for the Parser class.
   * @param lexer A pointer to the Lexer used for tokenizing the source code.
   */
  Parser(Lexer* lexer) {
    this->lexer = lexer;
  }

  /**
   * @enum NodeType
   * 
   * An enumeration of different node types in the AST.
   */
  enum class NodeType {
    // Statements
    Program,        /**< Represents the program node */
    VarDeclaration, /**< Represents a variable declaration */
    IfStmt,         /**< Represents an if statement */
    WhileStmt,      /**< Represents a while statement */
    BreakStmt,      /**< Represents a break statement */
    BlockStmt,      /**< Represents a block statement */
    // Expressions
    AssignmentExpr, /**< Represents an assignment expression */
    CallExpr,       /**< Represents a function call expression */
    MemberExpr,     /**< Represents a object/array member access */
    // Literals
    NumericLiteral, /**< Represents a numeric literal */
    StringLiteral,  /**< Represents a string literal */
    ObjectLiteral,  /**< Represents an object declaration */
    Identifier,     /**< Represents an identifier */
    BinaryExpr,     /**< Represents a binary expression */
    LogicalExpr     /**< Represents a logical expression */
  };

  /**
   * @struct Stmt
   * 
   * A base structure representing a statement in the AST.
   */
  typedef struct Stmt {
    NodeType kind; /**< The type of the statement */

    Stmt(NodeType _kind)
      : kind(_kind) {}
    virtual ~Stmt() = default;
  } Stmt;

  /**
   * @struct Program
   * 
   * Represents a program statement containing multiple statements in the body.
   */
  typedef struct Program : Stmt {
    std::vector<std::unique_ptr<Stmt>> body; /**< A list of statements within the program */

    Program()
      : Stmt(NodeType::Program) {
      body.reserve(estimatedProgramStatements);
    }

    // Delete copy constructor and copy assignment operator
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;
  } Program;

  /**
   * @struct Expr
   * 
   * A base structure representing an expression in the AST.
   */
  typedef struct Expr : Stmt {
    Expr(NodeType _nodeType)
      : Stmt(_nodeType) {}

    // Delete copy constructor and copy assignment operator
    Expr(const Expr&) = delete;
    Expr& operator=(const Expr&) = delete;

    virtual ~Expr() = default;
  } Expr;

  /**
   * @struct VarDeclaration
   * 
   * Represents a variable declaration with its type, name, and value.
   */
  typedef struct VarDeclaration : Stmt {
    bool constant;               /**< Whether the variable is constant */
    char* ident;                 /**< The identifier (name) of the variable */
    std::unique_ptr<Expr> value; /**< The expression representing the value assigned to the variable */

    VarDeclaration()
      : Stmt(Parser::NodeType::VarDeclaration), constant(false), ident(nullptr), value(nullptr) {}

    // Delete copy constructor and copy assignment operator
    VarDeclaration(const VarDeclaration&) = delete;
    VarDeclaration& operator=(const VarDeclaration&) = delete;

    ~VarDeclaration() {
      delete[] ident;
    }
  } VarDeclaration;

  /**
   * @struct AssignmentExpr
   * 
   * Represents an assignment expression consisting of an assignee and a value.
   */
  typedef struct AssignmentExpr : Expr {
    std::unique_ptr<Expr> assignee; /**< The expression to the left of the assignment */
    std::unique_ptr<Expr> value;    /**< The expression to the right of the assignment */

    AssignmentExpr()
      : Expr(NodeType::AssignmentExpr), assignee(nullptr), value(nullptr) {}

    // Delete copy constructor and copy assignment operator
    AssignmentExpr(const AssignmentExpr&) = delete;
    AssignmentExpr& operator=(const AssignmentExpr&) = delete;
  } AssignmentExpr;

  /**
   * @struct CallExpr
   * 
   * Represents a function call expression consisting of a caller and a list of arguments.
   */
  typedef struct CallExpr : Expr {
    std::unique_ptr<Expr> caller;            /**< The function being called */
    std::vector<std::unique_ptr<Expr>> args; /**< A list of arguments passed to the function */

    CallExpr()
      : Expr(NodeType::CallExpr) {
      args.reserve(estimatedFunctionArgs);
    }
    // Delete copy constructor and copy assignment operator
    CallExpr(const CallExpr&) = delete;
    CallExpr& operator=(const CallExpr&) = delete;
  } CallExpr;

  /**
   * @struct MemberExpr
   * 
   * Represents a object/array member access.
   * Computed means `object[property]`, not computed means `object.property`.
   */
  typedef struct MemberExpr : Expr {
    std::unique_ptr<Expr> object;
    std::unique_ptr<Expr> property;
    bool computed;

    MemberExpr()
      : Expr(NodeType::MemberExpr), object(nullptr), property(nullptr), computed(false) {}

    // Delete copy constructor and copy assignment operator
    MemberExpr(const MemberExpr&) = delete;
    MemberExpr& operator=(const MemberExpr&) = delete;
  } MemberExpr;

  /**
   * @struct BinaryExpr
   * 
   * Represents a binary expression with left and right operands and an operator.
   */
  typedef struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;  /**< The left operand */
    std::unique_ptr<Expr> right; /**< The right operand */
    char* op;                    /**< The operator (e.g., "+", "-", "*", "/") */

    BinaryExpr()
      : Expr(NodeType::BinaryExpr), left(nullptr), right(nullptr), op(nullptr) {}

    // Delete copy constructor and copy assignment operator
    BinaryExpr(const BinaryExpr&) = delete;
    BinaryExpr& operator=(const BinaryExpr&) = delete;

    ~BinaryExpr() {
      delete[] op;
    }
  } BinaryExpr;

  /**
   * @struct Identifier
   * 
   * Represents an identifier in an expression, such as a variable name.
   */
  typedef struct Identifier : Expr {
    char* symbol; /**< The name of the identifier */

    Identifier()
      : Expr(NodeType::Identifier), symbol(nullptr) {}

    // Delete copy constructor and copy assignment operator
    Identifier(const Identifier&) = delete;
    Identifier& operator=(const Identifier&) = delete;

    ~Identifier() {
      delete[] symbol;
    }
  } Identifier;

  /**
   * @struct NumericLiteral
   * 
   * Represents a numeric literal in the AST.
   */
  typedef struct NumericLiteral : Expr {
    int num; /**< The value of the numeric literal */

    NumericLiteral()
      : Expr(NodeType::NumericLiteral), num(0) {}
    // Delete copy constructor and copy assignment operator
    NumericLiteral(const NumericLiteral&) = delete;
    NumericLiteral& operator=(const NumericLiteral&) = delete;
    ~NumericLiteral() = default;
  } NumericLiteral;

  typedef struct StringLiteral : Expr {
    char* value;
    char* raw;

    StringLiteral()
      : Expr(NodeType::StringLiteral), value(nullptr), raw(nullptr) {}

    StringLiteral(char* _raw)
      : Expr(NodeType::StringLiteral) {
      size_t rawStrLen = strlen(_raw);
      size_t valueStrLen = rawStrLen - 2;  // minus the two quotes
      raw = new char[rawStrLen + 1];
      value = new char[valueStrLen + 1];
      strcpy(raw, _raw);
      strncpy(value, _raw + 1, valueStrLen);
      raw[rawStrLen] = '\0';
      value[valueStrLen] = '\0';
    }

    // Copy constructor
    StringLiteral(const StringLiteral& other)
      : Expr(NodeType::StringLiteral) {
      if (other.raw) {
        raw = new char[strlen(other.raw) + 1];
        strcpy(raw, other.raw);
      } else {
        raw = nullptr;
      }
      if (other.value) {
        value = new char[strlen(other.value) + 1];
        strcpy(value, other.value);
      } else {
        value = nullptr;
      }
    }

    // Move constructor
    StringLiteral(StringLiteral&& other) noexcept
      : Expr(NodeType::StringLiteral) {
      raw = other.raw;
      value = other.value;
      other.raw = nullptr;
      other.value = nullptr;
    }

    // Copy assignment operator
    StringLiteral& operator=(const StringLiteral& other) {
      if (this == &other) return *this;

      delete[] raw;
      delete[] value;

      if (other.raw) {
        raw = new char[strlen(other.raw) + 1];
        strcpy(raw, other.raw);
      } else {
        raw = nullptr;
      }
      if (other.value) {
        value = new char[strlen(other.value) + 1];
        strcpy(value, other.value);
      } else {
        value = nullptr;
      }

      return *this;
    }

    // Move assignment operator
    StringLiteral& operator=(StringLiteral&& other) noexcept {
      if (this == &other) return *this;

      delete[] raw;
      delete[] value;

      raw = other.raw;
      value = other.value;
      other.raw = nullptr;
      other.value = nullptr;

      return *this;
    }

    ~StringLiteral() {
      delete[] value;
      delete[] raw;
    }
  } StringLiteral;

  /**
   * @struct ObjectLiteral
   * 
   * Represents a data structure of key/value pairs.
   */
  typedef struct ObjectLiteral : Expr {
    std::map<String, std::unique_ptr<Expr>> properties;

    ObjectLiteral()
      : Expr(NodeType::ObjectLiteral) {}
    // Delete copy constructor and copy assignment operator
    ObjectLiteral(const ObjectLiteral&) = delete;
    ObjectLiteral& operator=(const ObjectLiteral&) = delete;

  } ObjectLiteral;

  /**
   * @struct LogicalExpr
   * 
   * Represents a logical expression with left and right operands and an operator.
   */
  typedef struct LogicalExpr : Expr {
    std::unique_ptr<Expr> left;  /**< The left operand */
    std::unique_ptr<Expr> right; /**< The right operand */
    char* op;                    /**< The logical operator (e.g., "and", "or") */

    LogicalExpr()
      : Expr(NodeType::LogicalExpr), left(nullptr), right(nullptr), op(nullptr) {}

    // Delete copy constructor and copy assignment operator
    LogicalExpr(const LogicalExpr&) = delete;
    LogicalExpr& operator=(const LogicalExpr&) = delete;

    ~LogicalExpr() {
      delete[] op;
    }
  } LogicalExpr;

  /**
   * @struct BlockStmt
   * 
   * Represents a block of statements enclosed in braces.
   */
  typedef struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> body; /**< A list of statements within the block */

    BlockStmt()
      : Stmt(NodeType::BlockStmt) {}
  } BlockStmt;

  /**
   * @struct IfStmt
   * 
   * Represents an if statement with a condition and consequent/alternate blocks.
   */
  typedef struct IfStmt : Stmt {
    std::unique_ptr<Expr> test;            /**< The condition being tested */
    std::unique_ptr<BlockStmt> consequent; /**< The block of statements executed if the condition is true */
    std::unique_ptr<BlockStmt> alternate;  /**< The block of statements executed if the condition is false (optional) */

    IfStmt()
      : Stmt(NodeType::IfStmt), test(nullptr), consequent(nullptr), alternate(nullptr) {}

    // Delete copy constructor and copy assignment operator
    IfStmt(const IfStmt&) = delete;
    IfStmt& operator=(const IfStmt&) = delete;
  } IfStmt;

  /**
   * @struct WhileStmt
   * 
   * Represents a while loop with a condition and body.
   */
  typedef struct WhileStmt : Stmt {
    std::unique_ptr<Expr> test;      /**< The condition being tested */
    std::unique_ptr<BlockStmt> body; /**< The body of the while loop */

    WhileStmt()
      : Stmt(NodeType::WhileStmt), test(nullptr), body(nullptr) {}
    // Delete copy constructor and copy assignment operator
    WhileStmt(const WhileStmt&) = delete;
    WhileStmt& operator=(const WhileStmt&) = delete;
  } WhileStmt;

  /**
   * @struct BreakStmt
   * 
   * Represents a break statement used in loops.
   */
  typedef struct BreakStmt : Stmt {
    BreakStmt()
      : Stmt(NodeType::BreakStmt) {}
  } BreakStmt;

  /**
   * @brief Produces the Abstract Syntax Tree (AST) for the provided source code.
   * @param code A pointer to the source code string.
   * @param len The length of the source code.
   * @return A pointer to the root Program node of the generated AST.
   */
  Program* produceAST(char* code, size_t len);

  /**
   * @brief Prints the AST in a human-readable format.
   * @param program A pointer to the root Program node of the AST.
   */
  void printAST(const Program* program);
private:
  Program program;                 /**< The root program node of the AST */
  Lexer* lexer;                    /**< The lexer used for tokenizing the input */
  std::queue<Lexer::Token> tokens; /**< A queue of tokens to be processed */
  size_t currentStmtIndex;         /**< The current statement index in the program */

  // Parsing functions for different types of statements and expressions
  std::unique_ptr<Stmt> parseStmt();
  std::unique_ptr<Expr> parseExpr();
  std::unique_ptr<VarDeclaration> parseVarDeclaration();
  std::unique_ptr<IfStmt> parseIfStmt();
  std::unique_ptr<WhileStmt> parseWhileStmt();
  std::unique_ptr<BreakStmt> parseBreakStmt();
  std::unique_ptr<BlockStmt> parseBlockStmt();
  std::unique_ptr<Expr> parseLogicalExpr();
  std::unique_ptr<Expr> parseRelationalExpr();
  std::unique_ptr<Expr> parseAssignmentExpr();
  std::unique_ptr<Expr> parseObjectExpr();
  std::unique_ptr<Expr> parseAdditiveExpr();
  std::unique_ptr<Expr> parseMultiplicativeExpr();
  std::unique_ptr<Expr> parseCallMemberExpr();
  std::unique_ptr<CallExpr> parseCallExpr(std::unique_ptr<Expr>&& caller);
  void parseArgs(std::unique_ptr<CallExpr>& callExpr);
  void parseArgsList(std::unique_ptr<CallExpr>& callExpr);
  std::unique_ptr<Expr> parseMemberExpr();
  std::unique_ptr<Expr> parsePrimaryExpr();

  /**
   * @brief Checks if the end of the file (EOF) has been reached.
   * @return True if the EOF has been reached, otherwise false.
   */
  bool endOfFile();

  /**
   * @brief Pushes a statement to the program body.
   */
  void push(std::unique_ptr<Stmt>&& stmt);

  /**
   * @brief Looks up the `Token` at the front of the `tokens` queue.
   * @return The `Token` to be processed next.
   */
  Lexer::Token& at();

  /**
   * @brief Removes the `Token` at the front of the `tokens` queue.
   * @return The `Token` to be processed next.
   */
  Lexer::Token eat();

  /**
   * @brief Checks if the `Token` at the front of the `tokens` queue has the given `TokenType`.
   * @param type The expected `TokenType`.
   * @param expectedVal The value used in the error message if the given types do not equal.
   * @return The expected `Token`.
   * @throws ErrorHandler::restart with `expectedVal` and the actual value.
   */
  Lexer::Token expect(Lexer::TokenType type, const char* expectedVal);

  // various toString functions
  void toStringNumericLiteral(const NumericLiteral* numLit);
  void toStringIdentifier(const Identifier* ident);
  void toStringStringLiteral(const StringLiteral* str);
  void toStringBinaryExpr(const BinaryExpr* binaryExpr);
  void toStringLogicalExpr(const LogicalExpr* logicalExpr);
  void toStringVarDecl(const VarDeclaration* varDecl);
  void toStringBlockStmt(const BlockStmt* blockStmt);
  void toStringIfStmt(const IfStmt* ifStmt);
  void toStringWhileStmt(const WhileStmt* whileStmt);
  void toStringBreakStmt(const BreakStmt* breakStmt);
  void toStringAssignmentExpr(const AssignmentExpr* assignmentExpr);
  void toStringObjectLiteral(const ObjectLiteral* objectLiteral);
  void toStringCallExpr(const CallExpr* callExpr);
  void toStringMemberExpr(const MemberExpr* memberExpr);
  void toString(const Stmt* stmt);
};