#pragma once
#include "types.hpp"
#include <memory>

namespace jc
{
  namespace grammar
  {
    enum class SymbolType : u8;
    enum class NT : u8;
    enum class T : u8;
  }
  namespace ast
  {
    struct Node;
    using NodePtr = std::unique_ptr<Node>;
    enum class TypeKind : u8;
  }
  namespace parser
  {
    struct PTree;
    struct PTNode;
  }

  enum class TokenType : u8;
  class SymbolTable;
}

namespace jc
{
  std::string to_string(const jc::TokenType t);
  std::string to_string(const jc::ast::TypeKind tk);
  std::string to_string(const jc::grammar::SymbolType symbol);
  std::string to_string(const jc::grammar::NT nt);
  std::string to_string(const jc::grammar::T t);
}

namespace jc
{
  namespace log
  {
    void debug(const std::string &msg);
    void ast(const jc::ast::NodePtr &node);
    void symbol_table(const jc::SymbolTable &table);
  }
}