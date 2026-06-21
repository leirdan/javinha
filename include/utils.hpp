#pragma once
#include "config.hpp"
#include "types.hpp"
#include <memory>
#include <ostream>

inline CompilerConfig config;

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
    struct ParserError;
  }

  enum class TokenType : u8;
  enum class SymbolCategory : u8;
  class SymbolTable;
  class Token;
}

namespace jc
{
  std::string to_string(const jc::TokenType t);
  std::string to_string(const jc::ast::TypeKind tk);
  std::string to_string(const jc::grammar::SymbolType symbol);
  std::string to_string(const jc::SymbolCategory symbol);
  std::string to_string(const jc::grammar::NT nt);
  std::string to_string(const jc::grammar::T t);
}

namespace jc
{
  namespace log
  {
    inline void debug(const std::string &msg, std::ostream &stream = std::cout)
    {
      if (config.debug)
        stream << "[DEBUG] " << msg << "\n";
    }
    void lexer_errors(const std::vector<std::string> &errors, std::ostream &stream = std::cout);
    void parser_errors(const std::vector<jc::parser::ParserError> &errors, std::ostream &stream = std::cout);
    void tokens(const std::vector<jc::Token> &tokens, std::ostream &stream = std::cout);
    void ast(const jc::ast::NodePtr &node, std::ostream &stream = std::cout);
    void symbol_table(const jc::SymbolTable &table, std::ostream &stream = std::cout);
  }
}