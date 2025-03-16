#pragma once

#include <unity.h>
#include <parser/Parser.h>

void test_parser_const_var_decl() {
  char code[] = "const x = 5;";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(program->body[0].get());
  TEST_ASSERT_EQUAL(true, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
}

void test_parser_number_var_decl() {
  char code[] = "let x = 5;";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(program->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
}

void test_parser_string_var_decl() {
  char code[] = "let x = \"hello\";";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(program->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL_STRING("hello", static_cast<AstNodes::StringLiteral*>(varDecl->value.get())->value);
}

void test_parser_object_var_decl() {
  char code[] = "let x = { a: 5, b: \"hello\" };";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(program->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::ObjectLiteral, varDecl->value->kind);
  AstNodes::ObjectLiteral* obj = static_cast<AstNodes::ObjectLiteral*>(varDecl->value.get());
  TEST_ASSERT_EQUAL(2, obj->properties.size());
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(obj->properties["a"].get())->num);
  TEST_ASSERT_EQUAL_STRING("hello", static_cast<AstNodes::StringLiteral*>(obj->properties["b"].get())->value);
}

void test_parser_assignment_expr() {
  char code[] = "let x = 5; x = 10;";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(2, program->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(program->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
  AstNodes::AssignmentExpr* assignExpr = static_cast<AstNodes::AssignmentExpr*>(program->body[1].get());
  TEST_ASSERT_EQUAL_STRING("x", static_cast<AstNodes::Identifier*>(assignExpr->assignee.get())->symbol);
  TEST_ASSERT_EQUAL(10, static_cast<AstNodes::NumericLiteral*>(assignExpr->value.get())->num);
}

void test_parser_if_stmt() {
  char code[] = "if (true) { let x = 5; }";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::IfStmt* ifStmt = static_cast<AstNodes::IfStmt*>(program->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, ifStmt->test->kind);
  TEST_ASSERT_EQUAL_STRING("true", static_cast<AstNodes::Identifier*>(ifStmt->test.get())->symbol);
  TEST_ASSERT_EQUAL(1, ifStmt->consequent->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(ifStmt->consequent->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
}

void test_parser_if_else_stmt() {
  char code[] = "if (true) { let x = 5; } else { let y = 10; }";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::IfStmt* ifStmt = static_cast<AstNodes::IfStmt*>(program->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, ifStmt->test->kind);
  TEST_ASSERT_EQUAL_STRING("true", static_cast<AstNodes::Identifier*>(ifStmt->test.get())->symbol);
  TEST_ASSERT_EQUAL(1, ifStmt->consequent->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(ifStmt->consequent->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
  TEST_ASSERT_EQUAL(1, ifStmt->alternate->body.size());
  varDecl = static_cast<AstNodes::VarDeclaration*>(ifStmt->alternate->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("y", varDecl->ident);
  TEST_ASSERT_EQUAL(10, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
}
void test_parser_if_stmt_logical_expr() {
  char code[] = "if (true and false or true) { let x = 5; }";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::IfStmt* ifStmt = static_cast<AstNodes::IfStmt*>(program->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::LogicalExpr, ifStmt->test->kind);

  AstNodes::LogicalExpr* logicalExpr = static_cast<AstNodes::LogicalExpr*>(ifStmt->test.get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::LogicalExpr, logicalExpr->left->kind);
  AstNodes::LogicalExpr* andLogicalExpr = static_cast<AstNodes::LogicalExpr*>(logicalExpr->left.get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, andLogicalExpr->left->kind);
  TEST_ASSERT_EQUAL_STRING("true", static_cast<AstNodes::Identifier*>(andLogicalExpr->left.get())->symbol);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, andLogicalExpr->right->kind);
  TEST_ASSERT_EQUAL_STRING("false", static_cast<AstNodes::Identifier*>(andLogicalExpr->right.get())->symbol);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, logicalExpr->right->kind);
  TEST_ASSERT_EQUAL_STRING("true", static_cast<AstNodes::Identifier*>(logicalExpr->right.get())->symbol);

  TEST_ASSERT_EQUAL(1, ifStmt->consequent->body.size());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::VarDeclaration, ifStmt->consequent->body[0]->kind);
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(ifStmt->consequent->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
}

void test_parser_if_stmt_relational_expr() {
  char code[] = "if (5 <= 10) { let x = 5; }";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::IfStmt* ifStmt = static_cast<AstNodes::IfStmt*>(program->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::BinaryExpr, ifStmt->test->kind);
  AstNodes::BinaryExpr* binaryExpr = static_cast<AstNodes::BinaryExpr*>(ifStmt->test.get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, binaryExpr->left->kind);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(binaryExpr->left.get())->num);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, binaryExpr->right->kind);
  TEST_ASSERT_EQUAL(10, static_cast<AstNodes::NumericLiteral*>(binaryExpr->right.get())->num);
  TEST_ASSERT_EQUAL(1, ifStmt->consequent->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(ifStmt->consequent->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
}

void test_parser_additive_expr() {
  char code[] = "x = 5 + 10;";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::AssignmentExpr* assignmentExpr = static_cast<AstNodes::AssignmentExpr*>(program->body[0].get());
  AstNodes::BinaryExpr* binaryExpr = static_cast<AstNodes::BinaryExpr*>(assignmentExpr->value.get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, binaryExpr->left->kind);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(binaryExpr->left.get())->num);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, binaryExpr->right->kind);
  TEST_ASSERT_EQUAL(10, static_cast<AstNodes::NumericLiteral*>(binaryExpr->right.get())->num);
  TEST_ASSERT_EQUAL_STRING("+", binaryExpr->op);
}

void test_parser_multiplicative_expr() {
  char code[] = "x = 5 * 10;";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::AssignmentExpr* assignmentExpr = static_cast<AstNodes::AssignmentExpr*>(program->body[0].get());
  AstNodes::BinaryExpr* binaryExpr = static_cast<AstNodes::BinaryExpr*>(assignmentExpr->value.get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, binaryExpr->left->kind);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(binaryExpr->left.get())->num);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, binaryExpr->right->kind);
  TEST_ASSERT_EQUAL(10, static_cast<AstNodes::NumericLiteral*>(binaryExpr->right.get())->num);
  TEST_ASSERT_EQUAL_STRING("*", binaryExpr->op);
}

void test_parser_complex_expr() {
  char code[] = "x = 5 + 10 * 2;";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::BinaryExpr* additiveExpr = static_cast<AstNodes::BinaryExpr*>(program->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, additiveExpr->left->kind);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(additiveExpr->left.get())->num);
  TEST_ASSERT_EQUAL_STRING("+", additiveExpr->op);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::BinaryExpr, additiveExpr->right->kind);
  AstNodes::BinaryExpr* multiplicativeExpr = static_cast<AstNodes::BinaryExpr*>(additiveExpr->right.get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, multiplicativeExpr->left->kind);
  TEST_ASSERT_EQUAL(10, static_cast<AstNodes::NumericLiteral*>(multiplicativeExpr->left.get())->num);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, multiplicativeExpr->right->kind);
  TEST_ASSERT_EQUAL(2, static_cast<AstNodes::NumericLiteral*>(multiplicativeExpr->right.get())->num);
  TEST_ASSERT_EQUAL_STRING("*", multiplicativeExpr->op);
}
void test_parser_while_stmt() {
  char code[] = "while (true) { let x = 5; }";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::WhileStmt* whileStmt = static_cast<AstNodes::WhileStmt*>(program->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, whileStmt->test->kind);
  TEST_ASSERT_EQUAL_STRING("true", static_cast<AstNodes::Identifier*>(whileStmt->test.get())->symbol);
  TEST_ASSERT_EQUAL(1, whileStmt->body->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(whileStmt->body->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  TEST_ASSERT_EQUAL(5, static_cast<AstNodes::NumericLiteral*>(varDecl->value.get())->num);
}
void test_parser_break_stmt() {
  char code[] = "while (true) { break; }";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::WhileStmt* whileStmt = static_cast<AstNodes::WhileStmt*>(program->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, whileStmt->test->kind);
  TEST_ASSERT_EQUAL_STRING("true", static_cast<AstNodes::Identifier*>(whileStmt->test.get())->symbol);
  TEST_ASSERT_EQUAL(1, whileStmt->body->body.size());
  AstNodes::BreakStmt* breakStmt = static_cast<AstNodes::BreakStmt*>(whileStmt->body->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::BreakStmt, breakStmt->kind);
}

void test_parser_call_expr() {
  char code[] = "foo();";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::CallExpr* callExpr = static_cast<AstNodes::CallExpr*>(program->body[0].get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, callExpr->caller->kind);
  TEST_ASSERT_EQUAL_STRING("foo", static_cast<AstNodes::Identifier*>(callExpr->caller.get())->symbol);
  TEST_ASSERT_EQUAL(0, callExpr->args.size());
}

void test_parser_call_expr_args() {
    char code[] = "foo(bar, \"baz\", 42);";
    Parser parser = Parser();
    AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
    TEST_ASSERT_EQUAL(1, program->body.size());
    AstNodes::CallExpr* callExpr = static_cast<AstNodes::CallExpr*>(program->body[0].get());
    TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, callExpr->caller->kind);
    TEST_ASSERT_EQUAL_STRING("foo", static_cast<AstNodes::Identifier*>(callExpr->caller.get())->symbol);
    TEST_ASSERT_EQUAL(3, callExpr->args.size());
    TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, callExpr->args[0]->kind);
    TEST_ASSERT_EQUAL_STRING("bar", static_cast<AstNodes::Identifier*>(callExpr->args[0].get())->symbol);
    TEST_ASSERT_EQUAL(AstNodes::NodeType::StringLiteral, callExpr->args[1]->kind);
    TEST_ASSERT_EQUAL_STRING("baz", static_cast<AstNodes::StringLiteral*>(callExpr->args[1].get())->value);
    TEST_ASSERT_EQUAL(AstNodes::NodeType::NumericLiteral, callExpr->args[2]->kind);
    TEST_ASSERT_EQUAL(42, static_cast<AstNodes::NumericLiteral*>(callExpr->args[2].get())->num);
  }

void test_parser_member_expr() {
  char code[] = "let x = foo.bar;";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(program->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  AstNodes::MemberExpr* memberExpr = static_cast<AstNodes::MemberExpr*>(varDecl->value.get());
  TEST_ASSERT_EQUAL_MESSAGE(AstNodes::NodeType::Identifier, memberExpr->object->kind, "foo was not an identifier");
  TEST_ASSERT_EQUAL_STRING("foo", static_cast<AstNodes::Identifier*>(memberExpr->object.get())->symbol);
  TEST_ASSERT_EQUAL_MESSAGE(AstNodes::NodeType::Identifier, memberExpr->property->kind, "bar was not an identifier");
  TEST_ASSERT_EQUAL_STRING("bar", static_cast<AstNodes::Identifier*>(memberExpr->property.get())->symbol);
  TEST_ASSERT_EQUAL(false, memberExpr->computed);
}

void test_parser_member_expr_computed() {
  char code[] = "let x = foo[\"bar\"];";
  Parser parser = Parser();
  AstNodes::Program* program = parser.produceAST(code, sizeof(code) - 1);
  TEST_ASSERT_EQUAL(1, program->body.size());
  AstNodes::VarDeclaration* varDecl = static_cast<AstNodes::VarDeclaration*>(program->body[0].get());
  TEST_ASSERT_EQUAL(false, varDecl->constant);
  TEST_ASSERT_EQUAL_STRING("x", varDecl->ident);
  AstNodes::MemberExpr* memberExpr = static_cast<AstNodes::MemberExpr*>(varDecl->value.get());
  TEST_ASSERT_EQUAL(AstNodes::NodeType::Identifier, memberExpr->object->kind);
  TEST_ASSERT_EQUAL_STRING("foo", static_cast<AstNodes::Identifier*>(memberExpr->object.get())->symbol);
  TEST_ASSERT_EQUAL(AstNodes::NodeType::StringLiteral, memberExpr->property->kind);
  TEST_ASSERT_EQUAL_STRING("bar", static_cast<AstNodes::StringLiteral*>(memberExpr->property.get())->value);
  TEST_ASSERT_EQUAL(true, memberExpr->computed);
}