#pragma once

#include "lexer/Lexer.h"

class Parser {
public:
    enum class NodeType {
        Undefined,
        Program,
        NumericLiteral,
        Identifier,
        BinaryExpr,
    };

    
    typedef struct Stmt {
        NodeType kind;

        Stmt() {kind = NodeType::Undefined;}
        Stmt(NodeType _kind) : kind(_kind) {}
    } Stmt;

    typedef struct Program : Stmt {
        Stmt body[maxStatements];

        Program() : Stmt(NodeType::Program) {}
    } Program;


    typedef struct Expr : Stmt {
        Expr() {kind = NodeType::Undefined;}
        Expr(NodeType _nodeType) : Stmt(_nodeType) {}
    } Expr;
    
    typedef struct BinaryExpr : Expr {
        Expr left;
        Expr right;
        char op;

        BinaryExpr() : Expr(NodeType::BinaryExpr) {}
    } BinaryExpr;

    typedef struct Identifier : Expr {
        char symbol[tokLen];

        Identifier() : Expr(NodeType::Identifier) {}
    } Identifier;

    typedef struct NumericLiteral : Expr {
        float num;

        NumericLiteral() : Expr(NodeType::NumericLiteral) {}
    } NumericLiteral;

    Program produceAST(char* code, size_t len);
private:
    
};