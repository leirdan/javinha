#include <iostream>
#include "utils.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "grammar.hpp"

void jc::log::debug(const std::string &msg)
{
#ifdef DEBUG
  std::cout << "[DEBUG] " << msg << "\n";
#endif
}

void jc::log::ast(const jc::ast::NodePtr &node)
{
  // #ifdef AST
  auto *prog = static_cast<jc::ast::ProgramNode *>(node.get());
  prog->print();
  // #endif
}

void jc::log::symbol_table(const jc::SymbolTable &table)
{
  table.print();
}

std::string jc::to_string(jc::TokenType t)
{
  switch (t)
  {
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::KEYWORD:
    return "KEYWORD";
  case TokenType::NUMBER:
    return "NUMBER";
  case TokenType::OPERATOR:
    return "OPERATOR";
  case TokenType::DELIMITER:
    return "DELIMITER";
  case TokenType::END_OF_FILE:
    return "EOF";
  default:
    return "UNKNOWN";
  }
}

std::string jc::to_string(const jc::ast::TypeKind tk)
{
  switch (tk)
  {
  case jc::ast::TypeKind::BOOLEAN:
    return "boolean";
  case jc::ast::TypeKind::INT:
    return "int";
  case jc::ast::TypeKind::INT_ARRAY:
    return "int[]";
  default:
    return "";
  }
}

std::string jc::to_string(const jc::grammar::SymbolType symbol)
{
  switch (symbol)
  {
  case SymbolType::LAMBDA:
    return "LAMBDA";
  case SymbolType::NON_TERMINAL:
    return "NON TERMINAL";
  case SymbolType::TERMINAL:
    return "TERMINAL";
  }
  return "UNKNOWN";
}

std::string jc::to_string(const jc::grammar::NT nt)
{
  switch (nt)
  {
  case NT::START:
    return "START";
  case NT::PROG:
    return "PROG";
  case NT::MAINC:
    return "MAINC";
  case NT::DEFCL:
    return "DEFCL";
  case NT::DEFVAR:
    return "DEFVAR";
  case NT::DEFMET:
    return "DEFMET";
  case NT::TYPE:
    return "TYPE";
  case NT::ARGS:
    return "ARGS";
  case NT::LISTCMD:
    return "LISTCMD";
  case NT::CMD:
    return "CMD";
  case NT::EXP:
    return "EXP";
  case NT::EXP2:
    return "EXP2";
  case NT::GREATER:
    return "GREATER";
  case NT::GREATER2:
    return "GREATER2";
  case NT::ADD:
    return "ADD";
  case NT::ADD2:
    return "ADD2";
  case NT::MUL:
    return "MUL";
  case NT::MUL2:
    return "MUL2";
  case NT::NEG:
    return "NEG";
  case NT::OBJ:
    return "OBJ";
  case NT::OBJATOM:
    return "OBJATOM";
  case NT::OBJMET:
    return "OBJMET";
  case NT::LISTEXP:
    return "LISTEXP";
  default:
    break;
  }
  return "UNKNOWN_NT";
}

std::string jc::to_string(const jc::grammar::T t)
{
  switch (t)
  {
  case T::CLASS:
    return "class";
  case T::PUBLIC:
    return "public";
  case T::STATIC:
    return "static";
  case T::VOID:
    return "void";
  case T::MAIN:
    return "main";
  case T::STRING:
    return "String";
  case T::IF:
    return "if";
  case T::ELSE:
    return "else";
  case T::WHILE:
    return "while";
  case T::SYSTEM:
    return "System";
  case T::OUT:
    return "out";
  case T::PRINTLN:
    return "println";
  case T::RETURN:
    return "return";
  case T::EXTENDS:
    return "extends";
  case T::INT:
    return "int";
  case T::BOOLEAN:
    return "boolean";
  case T::TRUE:
    return "true";
  case T::FALSE:
    return "false";
  case T::THIS:
    return "this";
  case T::NEW:
    return "new";
  case T::PLUS:
    return "+";
  case T::MINUS:
    return "-";
  case T::MULT:
    return "*";
  case T::GT:
    return ">";
  case T::AND:
    return "&&";
  case T::NOT:
    return "!";
  case T::ASSIGN:
    return "=";
  case T::LPAR:
    return "(";
  case T::RPAR:
    return ")";
  case T::LBRACE:
    return "{";
  case T::RBRACE:
    return "}";
  case T::LBRACKET:
    return "[";
  case T::RBRACKET:
    return "]";
  case T::DOT:
    return ".";
  case T::COMMA:
    return ",";
  case T::SEMICOLON:
    return ";";
  case T::ID:
    return "id";
  case T::NUMBER:
    return "number";
  case T::LENGTH:
    return "length";
  case T::END:
    return "EOF";
  default:
    break;
  }
  return "UNKNOWN_T";
}