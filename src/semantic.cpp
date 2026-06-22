#include "semantic.hpp"
#include <iostream>

using namespace jc;
using namespace jc::semantic;

void SemanticAnalyzer::analyze(const ast::NodePtr &root) {
  if (root)
    analyze_node(*root);
}

void SemanticAnalyzer::analyze_node(ast::Node &node) {

  switch (node.kind) {

    case ast::Kind::CLASS: {
      auto &cls = static_cast<ast::ClassNode &>(node);
      current_class = cls.name;
      if (!table.reenter_class_scope(cls.name)) {
        std::cerr << "[SemanticAnalyzer] escopo de classe '" << cls.name << "' não encontrado\n";
        return;
      }
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      table.exit_scope();
      current_class.clear();
      return;
    }

    case ast::Kind::MAIN_CLASS: {
      auto &mc = static_cast<ast::MainClassNode &>(node);
      current_class = mc.name;
      if (!table.reenter_class_scope(mc.name)) {
        std::cerr << "[SemanticAnalyzer] escopo de classe '" << mc.name << "' não encontrado\n";
        return;
      }
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      table.exit_scope();
      current_class.clear();
      return;
    }

    case ast::Kind::METHOD: {
      auto &method = static_cast<ast::MethodNode &>(node);
      if (!table.reenter_scope(method.name)) {
        std::cerr << "[SemanticAnalyzer] escopo de método '" << method.name << "' não encontrado\n";
        return;
      }
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      table.exit_scope();
      return;
    }

    case ast::Kind::MAIN_METHOD: {
      auto &mm = static_cast<ast::MainMethodNode &>(node);
      if (!table.reenter_scope(mm.name)) {
        std::cerr << "[SemanticAnalyzer] escopo de método '" << mm.name << "' não encontrado\n";
        return;
      }
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      table.exit_scope();
      return;
    }

    case ast::Kind::IDENTIFIER:
      check_identifier(static_cast<const ast::IdentifierNode &>(node));
      break;

    case ast::Kind::METHOD_CALL:
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      check_method_call(static_cast<const ast::MethodCallNode &>(node));
      return;

    case ast::Kind::ASSIGN:
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      check_assign(static_cast<const ast::AssignNode &>(node));
      return;

    case ast::Kind::ARRAY_ASSIGN:
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      check_array_assign(static_cast<const ast::ArrayAssignNode &>(node));
      return;

    case ast::Kind::NEW_OBJECT:
      check_new_object(static_cast<const ast::NewObjectNode &>(node));
      break;

    default:
      break;
  }

  for (const auto *child : node.children())
    analyze_node(*const_cast<ast::Node *>(child));
}

void SemanticAnalyzer::check_identifier(const ast::IdentifierNode &node) {
  if (!table.lookup(node.name).has_value())
    add_error(node.line, std::format("variável '{}' não declarada", node.name));
}

void SemanticAnalyzer::check_method_call(const ast::MethodCallNode &node) {
  if (!node.obj) return;
  if (node.obj->kind == ast::Kind::THIS) {
    if (!table.lookup(node.method).has_value())
      add_error(node.line, std::format("método '{}' não declarado na classe '{}'",
                                       node.method, current_class));
  }
}

void SemanticAnalyzer::check_assign(const ast::AssignNode &node) {
  if (!table.lookup(node.name).has_value())
    add_error(node.line, std::format("variável '{}' não declarada", node.name));
}

void SemanticAnalyzer::check_array_assign(const ast::ArrayAssignNode &node) {
  if (!table.lookup(node.name).has_value())
    add_error(node.line, std::format("variável '{}' não declarada", node.name));
}

void SemanticAnalyzer::check_new_object(const ast::NewObjectNode &node) {
  if (!table.lookup(node.class_name).has_value())
    add_error(node.line, std::format("classe '{}' não declarada", node.class_name));
}