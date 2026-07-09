#include "semantic.hpp"
#include <iostream>
#include <format>

using namespace jc;
using namespace jc::semantic;
using namespace jc::ast;

bool SemanticAnalyzer::types_compatible(const std::string &actual, const std::string &expected) {
  if (actual == expected)
    return true;
  return false;
}

std::optional<std::string> SemanticAnalyzer::infer_type(const Node &node) {
  switch (node.kind) {
    case Kind::NUMBER:
      return "int";
    case Kind::BOOL:
      return "boolean";
    case Kind::THIS:
      return current_class.empty() ? std::nullopt : std::make_optional(current_class);
    case Kind::IDENTIFIER: {
      const auto &id = static_cast<const IdentifierNode &>(node);
      auto sym = table.lookup(id.name);
      if (sym.has_value() && sym->type.has_value())
        return sym->type;
      return std::nullopt;
    }

    case Kind::BIN_OP: {
      const auto &bin = static_cast<const BinOpNode &>(node);
      switch (bin.op) {
        case BinOp::ADD:
        case BinOp::SUB:
        case BinOp::MULS:
          return "int";
        case BinOp::AND:
          return "boolean";
        case BinOp::GT:
          return "boolean";
      }
      return std::nullopt;
    }

    case Kind::NOT:
      return "boolean";

    case Kind::NEW_ARRAY:
      return "int[]";

    case Kind::NEW_OBJECT: {
      const auto &no = static_cast<const NewObjectNode &>(node);
      return no.class_name;
    }

    case Kind::ARRAY_ACCESS:
      return "int";

    case Kind::LENGTH:
      return "int";

    case Kind::METHOD_CALL: {
      const auto &mc = static_cast<const MethodCallNode &>(node);
      if (!mc.obj) return std::nullopt;
      std::optional<std::string> obj_type;
      if (mc.obj->kind == Kind::THIS)
        obj_type = current_class.empty() ? std::nullopt : std::make_optional(current_class);
      else
        obj_type = infer_type(*mc.obj);
      if (!obj_type.has_value()) return std::nullopt;
      return table.get_method_return_type(*obj_type, mc.method);
    }

    default:
      return std::nullopt;
  }
}

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
      current_method = method.name;
      if (!table.reenter_scope(method.name)) {
        std::cerr << "[SemanticAnalyzer] escopo de método '" << method.name << "' não encontrado\n";
        return;
      }
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      if (method.return_expr)
        check_return(method, *method.return_expr);
      table.exit_scope();
      current_method.clear();
      return;
    }

    case ast::Kind::MAIN_METHOD: {
      auto &mm = static_cast<ast::MainMethodNode &>(node);
      current_method = mm.name;
      if (!table.reenter_scope(mm.name)) {
        std::cerr << "[SemanticAnalyzer] escopo de método '" << mm.name << "' não encontrado\n";
        return;
      }
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      table.exit_scope();
      current_method.clear();
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
    
    case ast::Kind::BIN_OP:
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      check_bin_op(static_cast<const ast::BinOpNode &>(node));
      return;

    case ast::Kind::NOT:
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      check_not(static_cast<const ast::NotNode &>(node));
      return;

    case ast::Kind::IF:
      check_if(static_cast<const ast::IfNode &>(node));
      return;

    case ast::Kind::WHILE:
      check_while(static_cast<const ast::WhileNode &>(node));
      return;

    case ast::Kind::PRINT:
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      check_print(static_cast<const ast::PrintNode &>(node));
      return;

    case ast::Kind::ARRAY_ACCESS:
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      check_array_access(static_cast<const ast::ArrayAccessNode &>(node));
      return;

    case ast::Kind::LENGTH:
      for (const auto *child : node.children())
        analyze_node(*const_cast<ast::Node *>(child));
      check_length(static_cast<const ast::LengthNode &>(node));
      return;

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

  std::optional<std::string> obj_type;
  if (node.obj->kind == ast::Kind::THIS)
    obj_type = current_class.empty() ? std::nullopt : std::make_optional(current_class);
  else
    obj_type = infer_type(*node.obj);

  if (!obj_type.has_value()) return;

  auto return_type = table.get_method_return_type(*obj_type, node.method);
  if (!return_type.has_value()) {
    add_error(node.line, "método '" + node.method + "' não declarado na classe '" + *obj_type + "'");
    return;
  }

  auto param_types = table.get_method_param_types(*obj_type, node.method);
  if (param_types.size() != node.args.size()) {
    add_error(node.line,
      "método '" + node.method + "' esperava " + std::to_string(param_types.size()) +
      " argumento(s), mas recebeu " + std::to_string(node.args.size()));
    return;
  }

  for (size_t i = 0; i < param_types.size(); ++i) {
    if (!node.args[i]) continue;
    auto arg = infer_type(*node.args[i]);
    if (arg.has_value() && !types_compatible(*arg, param_types[i]))
      add_error(node.line,
        "argumento " + std::to_string(i + 1) + " de '" + node.method +
        "': esperado '" + param_types[i] + "', encontrou '" + *arg + "'");
  }
}

void SemanticAnalyzer::check_assign(const ast::AssignNode &node) {
  auto sym = table.lookup(node.name);
  if (!sym.has_value()) {
    add_error(node.line, "variável '" + node.name + "' não declarada");
    return;
  }
  if (!sym->type.has_value() || !node.value) return;
  auto rhs = infer_type(*node.value);
  if (rhs.has_value() && !types_compatible(*rhs, *sym->type))
    add_error(node.line,
      "atribuição inválida: variável '" + node.name + "' é do tipo '" + *sym->type + "', mas recebeu '" + *rhs + "'");
}

void SemanticAnalyzer::check_array_assign(const ast::ArrayAssignNode &node) {
  auto sym = table.lookup(node.name);
  if (!sym.has_value()) {
    add_error(node.line, "variável '" + node.name + "' não declarada");
    return;
  }
  if (sym->type.has_value() && *sym->type != "int[]")
    add_error(node.line, "'" + node.name + "' não é um array (tipo: '" + *sym->type + "')");

  if (node.index) {
    auto idx = infer_type(*node.index);
    if (idx.has_value() && *idx != "int")
      add_error(node.line, "índice de array deve ser 'int', mas encontrou '" + *idx + "'");
  }
  if (node.value) {
    auto val = infer_type(*node.value);
    if (val.has_value() && *val != "int")
      add_error(node.line, "valor de atribuição em array deve ser 'int', mas encontrou '" + *val + "'");
  }
}

void SemanticAnalyzer::check_new_object(const ast::NewObjectNode &node) {
  if (!table.class_exists(node.class_name))
    add_error(node.line, std::format("classe '{}' não declarada", node.class_name));
}

void SemanticAnalyzer::check_bin_op(const BinOpNode &node) {
  if (!node.left || !node.right) return;
  auto lhs = infer_type(*node.left);
  auto rhs = infer_type(*node.right);
  switch (node.op) {
    case BinOp::ADD:
    case BinOp::SUB:
    case BinOp::MULS:
    case BinOp::GT:
      if (lhs.has_value() && *lhs != "int")
        add_error(node.line,
          "operador '" + to_string(node.op) + "' requer 'int', mas encontrou '" + *lhs + "' no lado esquerdo");
      if (rhs.has_value() && *rhs != "int")
        add_error(node.line,
          "operador '" + to_string(node.op) + "' requer 'int', mas encontrou '" + *rhs + "' no lado direito");
      break;
    case BinOp::AND:
      if (lhs.has_value() && *lhs != "boolean")
        add_error(node.line,
          "operador '&&' requer 'boolean', mas encontrou '" + *lhs + "' no lado esquerdo");
      if (rhs.has_value() && *rhs != "boolean")
        add_error(node.line,
          "operador '&&' requer 'boolean', mas encontrou '" + *rhs + "' no lado direito");
      break;
  }
}

void SemanticAnalyzer::check_not(const NotNode &node) {
  if (!node.expr) return;
  auto t = infer_type(*node.expr);
  if (t.has_value() && *t != "boolean")
    add_error(node.line, "operador '!' requer 'boolean', mas encontrou '" + *t + "'");
}

void SemanticAnalyzer::check_if(const IfNode &node) {
  if (node.cond) {
    analyze_node(*const_cast<Node *>(static_cast<const Node *>(node.cond.get())));
    auto t = infer_type(*node.cond);
    if (t.has_value() && *t != "boolean")
      add_error(node.line, "condição do 'if' deve ser 'boolean', mas encontrou '" + *t + "'");
  }
  if (node.then_branch)
    analyze_node(*const_cast<Node *>(static_cast<const Node *>(node.then_branch.get())));
  if (node.else_branch)
    analyze_node(*const_cast<Node *>(static_cast<const Node *>(node.else_branch.get())));
}

void SemanticAnalyzer::check_while(const WhileNode &node) {
  if (node.cond) {
    analyze_node(*const_cast<Node *>(static_cast<const Node *>(node.cond.get())));
    auto t = infer_type(*node.cond);
    if (t.has_value() && *t != "boolean")
      add_error(node.line, "condição do 'while' deve ser 'boolean', mas encontrou '" + *t + "'");
  }
  if (node.body)
    analyze_node(*const_cast<Node *>(static_cast<const Node *>(node.body.get())));
}

void SemanticAnalyzer::check_print(const PrintNode &node) {
  if (!node.expr) return;
  auto t = infer_type(*node.expr);
  if (t.has_value() && *t != "int")
    add_error(node.line, "'System.out.println' requer 'int', mas encontrou '" + *t + "'");
}

void SemanticAnalyzer::check_array_access(const ArrayAccessNode &node) {
  if (node.array) {
    auto arr = infer_type(*node.array);
    if (arr.has_value() && *arr != "int[]")
      add_error(node.line, "acesso por índice requer 'int[]', mas encontrou '" + *arr + "'");
  }
  if (node.index) {
    auto idx = infer_type(*node.index);
    if (idx.has_value() && *idx != "int")
      add_error(node.line, "índice de array deve ser 'int', mas encontrou '" + *idx + "'");
  }
}

void SemanticAnalyzer::check_length(const LengthNode &node) {
  if (!node.obj) return;
  auto t = infer_type(*node.obj);
  if (t.has_value() && *t != "int[]")
    add_error(node.line, "'.length' requer 'int[]', mas encontrou '" + *t + "'");
}

void SemanticAnalyzer::check_return(const MethodNode &method_node, const Node &return_expr) {
  if (!method_node.return_type) return;
  const auto *type_node = dynamic_cast<const TypeNode *>(method_node.return_type.get());
  if (!type_node) return;

  std::string expected = type_node->name;
  if (expected.empty()) return;

  auto actual = infer_type(return_expr);
  if (!actual.has_value()) return;

  if (!types_compatible(*actual, expected))
    add_error(method_node.line,
      "método '" + method_node.name + "' declara retorno '" + expected + "', mas retorna '" + *actual + "'");
}