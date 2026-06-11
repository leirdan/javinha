#include <iostream>
#include "utils.hpp"
#include "ast.hpp"

using namespace jc;
using namespace jc::ast;

void log::debug(const std::string &&msg)
{
  // #ifdef DEBUG
  std::cout << "[DEBUG] " << msg << "\n";
  // #endif
}

void log::ast(const NodePtr node)
{
  // #ifdef AST
  print_ast_rec(node, 0);
  // #endif
}

void log::print_ast_rec(const NodePtr node, u8 depth)
{
  std::string tabs = std::string(depth * 2, ' ');
  if (node == nullptr)
  {
    std::cout << tabs << "[Null]\n";
    return;
  }

  switch (node->kind)
  {
  case Kind::PROGRAM:
  {
    auto *prog = static_cast<ProgramNode *>(node);
    std::cout << tabs << "[ProgramNode]\n";

    // Imprime a classe principal
    print_ast_rec(prog->main_class, depth + 1);

    // Imprime o vetor de classes declaradas
    for (auto *cl : prog->classes)
    {
      print_ast_rec(cl, depth + 1);
    }
    break;
  }

  case Kind::MAIN_CLASS:
  {
    auto *main = static_cast<MainClassNode *>(node);
    std::cout << tabs << std::format("[MainClassNode (name: {}, args: {})]\n", main->name, main->args_param);

    // TODO: imprimir corpo
    break;
  }
  case Kind::CLASS:
  {
    auto *cl = static_cast<ClassNode *>(node);
    std::cout << tabs << std::format("[ClassNode (name: {}, parent: {})]\n", cl->name, cl->parent.value_or("none"));

    // TODO: imprimir corpo
    break;
  }
  }
}