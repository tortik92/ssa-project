#pragma once

#include <vector>
#include "lexer/Lexer.h"
#include "ErrorHandler.h"
#include "pool/MemoryPool.h"

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
    // Literals
    NumericLiteral, /**< Represents a numeric literal */
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
  } Stmt;

  /**
   * @struct Program
   * 
   * Represents a program statement containing multiple statements in the body.
   */
  typedef struct Program : Stmt {
    std::vector<Stmt*> body; /**< A list of statements within the program */

    Program()
      : Stmt(NodeType::Program) {
      body.reserve(estimatedProgramStatements);
    }
  } Program;

  /**
   * @struct Expr
   * 
   * A base structure representing an expression in the AST.
   */
  typedef struct Expr : Stmt {
    Expr(NodeType _nodeType)
      : Stmt(_nodeType) {}
  } Expr;

  /**
   * @struct VarDeclaration
   * 
   * Represents a variable declaration with its type, name, and value.
   */
  typedef struct VarDeclaration : Stmt {
    bool constant; /**< Whether the variable is constant */
    char* ident;   /**< The identifier (name) of the variable */
    Expr* value;   /**< The expression representing the value assigned to the variable */

    VarDeclaration()
      : Stmt(Parser::NodeType::VarDeclaration), constant(false), ident(nullptr), value(nullptr) {}
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
    Expr* assignee; /**< The expression to the left of the assignment */
    Expr* value;    /**< The expression to the right of the assignment */

    AssignmentExpr()
      : Expr(NodeType::AssignmentExpr), assignee(nullptr), value(nullptr) {}
  } AssignmentExpr;

  /**
   * @struct CallExpr
   * 
   * Represents a function call expression consisting of a caller and a list of arguments.
   */
  typedef struct CallExpr : Expr {
    Expr* caller;            /**< The function being called */
    std::vector<Expr*> args; /**< A list of arguments passed to the function */

    CallExpr()
      : Expr(NodeType::CallExpr), caller(nullptr) {
      args.reserve(estimatedFunctionArgs);
    }
  } CallExpr;

  /**
   * @struct BinaryExpr
   * 
   * Represents a binary expression with left and right operands and an operator.
   */
  typedef struct BinaryExpr : Expr {
    Expr* left;  /**< The left operand */
    Expr* right; /**< The right operand */
    char* op;    /**< The operator (e.g., "+", "-", "*", "/") */

    BinaryExpr()
      : Expr(NodeType::BinaryExpr), left(nullptr), right(nullptr), op(nullptr) {}
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

    ~Identifier() {
      if (symbol != nullptr) {
        delete[] symbol;
      }
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
      : Expr(NodeType::NumericLiteral) {}
  } NumericLiteral;

  /**
   * @struct LogicalExpr
   * 
   * Represents a logical expression with left and right operands and an operator.
   */
  typedef struct LogicalExpr : Expr {
    Expr* left;  /**< The left operand */
    Expr* right; /**< The right operand */
    char* op;    /**< The logical operator (e.g., "and", "or") */

    LogicalExpr()
      : Expr(NodeType::LogicalExpr), left(nullptr), right(nullptr), op(nullptr) {}
  } LogicalExpr;

  /**
   * @struct BlockStmt
   * 
   * Represents a block of statements enclosed in braces.
   */
  typedef struct BlockStmt : Stmt {
    std::vector<Stmt*> body; /**< A list of statements within the block */

    BlockStmt()
      : Stmt(NodeType::BlockStmt) {}
  } BlockStmt;

  /**
   * @struct IfStmt
   * 
   * Represents an if statement with a condition and consequent/alternate blocks.
   */
  typedef struct IfStmt : Stmt {
    Expr* test;            /**< The condition being tested */
    BlockStmt* consequent; /**< The block of statements executed if the condition is true */
    BlockStmt* alternate;  /**< The block of statements executed if the condition is false (optional) */

    IfStmt()
      : Stmt(NodeType::IfStmt), test(nullptr), consequent(nullptr), alternate(nullptr) {}
  } IfStmt;

  /**
   * @struct WhileStmt
   * 
   * Represents a while loop with a condition and body.
   */
  typedef struct WhileStmt : Stmt {
    Expr* test;      /**< The condition being tested */
    BlockStmt* body; /**< The body of the while loop */

    WhileStmt()
      : Stmt(NodeType::WhileStmt), test(nullptr), body(nullptr) {}
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

  // Memory pools for various AST node types
  MemoryPool<BinaryExpr, poolSize> binaryExprPool;
  MemoryPool<Identifier, poolSize> identifierPool;
  MemoryPool<NumericLiteral, poolSize> numericLiteralPool;
  MemoryPool<VarDeclaration, poolSize> varDeclarationPool;
  MemoryPool<AssignmentExpr, poolSize> assignmentExprPool;
  MemoryPool<CallExpr, poolSize> callExprPool;
  MemoryPool<LogicalExpr, poolSize> logicalExprPool;
  MemoryPool<BlockStmt, poolSize> blockStmtPool;
  MemoryPool<IfStmt, poolSize> ifStmtPool;
  MemoryPool<WhileStmt, poolSize> whileStmtPool;
  MemoryPool<BreakStmt, poolSize> breakStmtPool;

  void cleanup();

  // Parsing functions for different types of statements and expressions
  Stmt* parseStmt();
  Expr* parseExpr();
  VarDeclaration* parseVarDeclaration();
  IfStmt* parseIfStmt();
  WhileStmt* parseWhileStmt();
  BreakStmt* parseBreakStmt();
  BlockStmt* parseBlockStmt();
  Expr* parseLogicalExpr();
  Expr* parseRelationalExpr();
  Expr* parseAssignmentExpr();
  Expr* parseAdditiveExpr();
  Expr* parseMultiplicativeExpr();
  Expr* parseCallMemberExpr();
  Expr* parseCallExpr(Expr* caller);
  void parseArgs(CallExpr* callExpr);
  void parseArgsList(CallExpr* callExpr);
  Expr* parsePrimaryExpr();

  /**
   * @brief Checks if the end of the file (EOF) has been reached.
   * @return True if the EOF has been reached, otherwise false.
   */
  bool endOfFile();

  /**
   * @brief Pushes a statement to the program body.
   */
  void push(Stmt* stmt);

  /**
   * @brief Looks up the `Token` at the front of the `tokens` queue.
   * @return The `Token` to be processed next.
   */
  Lexer::Token at();

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

  /**
   * @brief Copies the string into a new dynamically allocated array.
   * @param src Pointer to the source string.
   * @return A pointer to the destination string. 
   */
  char* strcpyNew(const char* src);

  // various toString functions
  void toStringNumericLiteral(const NumericLiteral* numLit);
  void toStringIdentifier(const Identifier* ident);
  void toStringBinaryExpr(const BinaryExpr* binaryExpr);
  void toStringLogicalExpr(const LogicalExpr* logicalExpr);
  void toStringVarDecl(const VarDeclaration* varDecl);
  void toStringBlockStmt(const BlockStmt* blockStmt);
  void toStringIfStmt(const IfStmt* ifStmt);
  void toStringWhileStmt(const WhileStmt* whileStmt);
  void toStringBreakStmt(const BreakStmt* breakStmt);
  void toStringAssignmentExpr(const AssignmentExpr* assignmentExpr);
  void toStringCallExpr(const CallExpr* callExpr);
  void toString(const Stmt* stmt);
};