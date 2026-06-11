#pragma once
#include "grammar.hpp"
#include "token.hpp"
#include "types.hpp"
// #include "ast.hpp"

namespace jc
{
  namespace ast
  {
    struct Node;
    using NodePtr = Node *;
  }
}

using namespace jc;
using namespace jc::grammar;

namespace jc
{
  namespace log
  {
    void debug(const std::string &&msg);
    void ast(const ::jc::ast::NodePtr node);
    void print_ast_rec(const ::jc::ast::NodePtr node, u8 depth = 0);
  }
}