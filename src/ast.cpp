#include "ast.hpp"
#include "utils.hpp"
#include <variant>

using namespace jc::ast;
using namespace jc::grammar;

std::string jc::ast::type_kind_to_string(TypeKind tk)
{
  switch (tk)
  {
  case TypeKind::BOOLEAN:
    return "boolean";
  case TypeKind::INT:
    return "int";
  case TypeKind::INT_ARRAY:
    return "int[]";
  default:
    return "";
  }
}

NodePtr AST::create(const PTree &root)
{
  if (std::holds_alternative<std::optional<Token>>(root.value))
  {
    log::debug("AST vazia.");
    return nullptr;
  }
  const PTNode &root_node = std::get<PTNode>(root.value);
  if (root_node.children.size() == 2 && root_node.rule == start_symbol)
  {
    auto &ptr = root_node.children.front();
    auto &prog_node = std::get<PTNode>(ptr->value);
    return prog(prog_node);
  }
  return nullptr;
}

NodePtr AST::prog(const PTNode &root)
{
  if (root.rule != NT::PROG)
  {
    log::debug(std::format("esperava prog, recebeu {}", symbol_to_string(root.rule)));
    return nullptr;
  }

  NodePtr program_ptr = std::make_unique<ProgramNode>();
  ProgramNode *prog_raw = static_cast<ProgramNode *>(program_ptr.get());

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      log::debug(std::format("filho: {}", symbol_to_string(child.rule)));
      if (child.rule == NT::MAINC)
      {
        prog_raw->main_class = main_c(child);
      }
      else if (child.rule == NT::DEFCL)
      {
        prog_raw->classes = std::move(def_cl(child));
      }
      else
      {
        log::debug(std::format("Prog não deveria ter nenhum estado diferente...: {}", symbol_to_string(child.rule)));
      }
    }
  }

  program_ptr->print();

  return program_ptr;
}

NodePtr AST::main_c(const PTNode &root)
{
  if (root.rule != NT::MAINC)
  {
    log::debug(std::format("esperava main_c, recebeu {}", symbol_to_string(root.rule)));
    return nullptr;
  }

  NodePtr main_class_ptr = std::make_unique<MainClassNode>();
  MainClassNode *main_raw = static_cast<MainClassNode *>(main_class_ptr.get());

  std::string main_param_name = "";
  NodePtr main_body = nullptr;

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);

      if (child.rule == NT::LISTCMD)
      {
        main_body = list_cmd(child);
      }
    }
    else
    {
      const auto &child_opt = std::get<std::optional<Token>>(ptr->value);
      if (child_opt.has_value())
      {
        const auto &token = child_opt.value();

        if (token.type == TokenType::IDENTIFIER)
        {
          if (main_raw->name.empty())
          {
            main_raw->name = token.value;
          }
          else if (main_param_name.empty() && token.value.compare("main") != 0)
          {
            main_param_name = token.value;
          }
        }
      }
    }
  }

  main_raw->main_method = std::make_unique<MainMethodNode>("main", main_param_name, "void", std::move(main_body));

  return main_class_ptr;
}

std::vector<NodePtr> AST::def_cl(const PTNode &root)
{
  std::vector<NodePtr> classes;
  if (root.rule != NT::DEFCL)
  {
    log::debug(std::format("esperava def_cl, recebeu {}", symbol_to_string(root.rule)));
    return classes;
  }

  bool search_for_inheritance = false;
  NodePtr class_node_ptr = std::make_unique<ClassNode>();
  ClassNode *class_raw = static_cast<ClassNode *>(class_node_ptr.get());

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::DEFVAR)
      {
        class_raw->fields = def_var(child);
      }
      else if (child.rule == NT::DEFMET)
      {
        class_raw->methods = def_met(child);
      }
      else if (child.rule == NT::DEFCL)
      {
        auto next = def_cl(child);
        // classes.insert(classes.end(), std::move(next));
        std::move(next.begin(), next.end(), std::back_inserter(classes));
      }
    }
    else
    {
      const auto &child = std::get<std::optional<Token>>(ptr->value);
      if (child.has_value())
      {
        auto &token = child.value();
        if (token.type == TokenType::IDENTIFIER && class_raw->name.empty())
        {
          class_raw->name = token.value;
        }
        else if (token.type == TokenType::IDENTIFIER && !class_raw->name.empty() && !class_raw->parent.has_value() && search_for_inheritance)
        {
          class_raw->parent = std::make_optional(token.value);
          search_for_inheritance = false;
        }
        else if (token.type == TokenType::KEYWORD && token.value == "extends" && !class_raw->parent.has_value())
        {
          search_for_inheritance = true;
        }
      }
    }
  }

  classes.insert(classes.begin(), std::move(class_node_ptr));
  return classes;
}

std::vector<NodePtr> AST::def_var(const PTNode &root)
{
  std::vector<NodePtr> v;

  if (root.rule != NT::DEFVAR)
    return v;
  else if (root.children.empty() || !has_any_member(root.children))
    return v;

  NodePtr current_type = nullptr;
  std::string var_name = "";
  const PTNode *next_def_var = nullptr;

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::TYPE)
        current_type = type(child);
      else if (child.rule == NT::DEFVAR)
        next_def_var = &child;
    }
    else
    {
      const auto &token_opt = std::get<std::optional<Token>>(ptr->value);
      if (token_opt.has_value())
      {
        auto token = token_opt.value();
        if (token.type == TokenType::IDENTIFIER && var_name.empty())
          var_name = token.value;
      }
    }
  }

  if (current_type != nullptr && !var_name.empty())
  {
    v.push_back(std::make_unique<VarDeclNode>(std::move(current_type), var_name));
  }

  if (next_def_var != nullptr)
  {
    std::vector<NodePtr> next_vars = def_var(*next_def_var);
    std::move(next_vars.begin(), next_vars.end(), std::back_inserter(v));
  }

  return v;
}

NodePtr AST::type(const PTNode &root)
{
  if (root.rule != NT::TYPE)
    return nullptr;

  if (!root.children.empty())
  {
    const auto &first_child = root.children[0];
    if (std::holds_alternative<std::optional<Token>>(first_child->value))
    {
      const auto &token_opt = std::get<std::optional<Token>>(first_child->value);
      if (token_opt.has_value())
      {
        auto token = token_opt.value();
        if (token.type == TokenType::KEYWORD && token.value == "int")
        {
          if (root.children.size() == 3)
            return std::make_unique<TypeNode>(TypeKind::INT_ARRAY);
          else if (root.children.size() == 1)
            return std::make_unique<TypeNode>(TypeKind::INT);
        }
        else if (token.type == TokenType::KEYWORD && token.value == "boolean")
        {
          return std::make_unique<TypeNode>(TypeKind::BOOLEAN);
        }
        else if (token.type == TokenType::IDENTIFIER)
        {
          return std::make_unique<TypeNode>(TypeKind::CLASS, token.value);
        }
      }
    }
  }
  return nullptr;
}

std::vector<NodePtr> AST::def_met(const PTNode &root)
{
  std::vector<NodePtr> v;

  if (root.rule != NT::DEFMET)
    return v;
  if (root.children.empty())
    return v;

  NodePtr return_type = nullptr;
  std::string method_name = "";
  std::vector<ParamNode *> method_args;
  std::vector<NodePtr> local_vars;
  NodePtr body_cmds = nullptr;
  NodePtr return_expr = nullptr;
  const PTNode *next_def_met = nullptr;

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::TYPE)
        return_type = type(child);
      else if (child.rule == NT::ARGS)
        method_args = args(child);
      else if (child.rule == NT::DEFVAR)
        local_vars = def_var(child);
      else if (child.rule == NT::LISTCMD)
        body_cmds = list_cmd(child);
      else if (child.rule == NT::EXP)
        return_expr = exp(child);
      else if (child.rule == NT::DEFMET)
        next_def_met = &child;
    }
    else
    {
      const auto &token_opt = std::get<std::optional<Token>>(ptr->value);
      if (token_opt.has_value())
      {
        auto token = token_opt.value();
        if (token.type == TokenType::IDENTIFIER && method_name.empty())
          method_name = token.value;
      }
    }
  }

  if (return_type != nullptr && !method_name.empty())
  {
    v.push_back(std::make_unique<MethodNode>(
        std::move(return_type),
        method_name,
        std::move(method_args),
        std::move(local_vars),
        std::move(body_cmds),
        std::move(return_expr)));
  }

  if (next_def_met != nullptr)
  {
    std::vector<NodePtr> next_methods = def_met(*next_def_met);
    v.reserve(v.size() + next_methods.size());
    std::move(next_methods.begin(), next_methods.end(), std::back_inserter(v));
  }

  return v;
}

std::vector<ParamNode *> AST::args(const PTNode &root)
{
  std::vector<ParamNode *> v;

  if (root.rule != NT::ARGS)
    return v;

  NodePtr current_type = nullptr;
  std::string param_name = "";
  const PTNode *next_args = nullptr;

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::TYPE)
        current_type = type(child);
      else if (child.rule == NT::ARGS)
        next_args = &child;
    }
    else
    {
      const auto &token_opt = std::get<std::optional<Token>>(ptr->value);
      if (token_opt.has_value())
      {
        auto token = token_opt.value();
        if (token.type == TokenType::IDENTIFIER && param_name.empty())
          param_name = token.value;
      }
    }
  }

  if (current_type != nullptr && !param_name.empty())
  {
    // TODO: Cuidar com o vazamento aqui no futuro, já que ParamNode* é devolvido em um vector de ponteiros crus
    auto *param_node = new ParamNode(std::move(current_type), param_name);
    v.push_back(param_node);
  }

  if (next_args != nullptr)
  {
    std::vector<ParamNode *> next_v = args(*next_args);
    v.reserve(v.size() + next_v.size());
    std::move(next_v.begin(), next_v.end(), std::back_inserter(v));
  }

  return v;
}

NodePtr AST::list_cmd(const PTNode &root)
{
  if (root.rule != NT::LISTCMD)
    return nullptr;

  if (root.children.empty())
    return std::make_unique<BlockNode>();

  std::vector<NodePtr> cmds;
  NodePtr current_cmd = nullptr;
  const PTNode *next_list = nullptr;

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::CMD)
        current_cmd = cmd(child);
      else if (child.rule == NT::LISTCMD)
        next_list = &child;
    }
  }

  if (current_cmd != nullptr)
    cmds.push_back(std::move(current_cmd));

  if (next_list != nullptr)
  {
    NodePtr next_block_node = list_cmd(*next_list);
    if (next_block_node != nullptr)
    {
      auto *sub_block = static_cast<BlockNode *>(next_block_node.get());
      cmds.reserve(cmds.size() + sub_block->stmt.size());
      std::move(sub_block->stmt.begin(), sub_block->stmt.end(), std::back_inserter(cmds));
    }
  }

  return std::make_unique<BlockNode>(std::move(cmds));
}

NodePtr AST::cmd(const PTNode &root)
{
  if (root.rule != NT::CMD || root.children.empty())
    return nullptr;

  if (std::holds_alternative<std::optional<Token>>(root.children[0]->value))
  {
    const auto &token_opt = std::get<std::optional<Token>>(root.children[0]->value);
    if (token_opt.has_value() && token_opt->value == "{")
    {
      if (root.children.size() > 1 && std::holds_alternative<PTNode>(root.children[1]->value))
      {
        return list_cmd(std::get<PTNode>(root.children[1]->value));
      }
    }
  }

  NodePtr expr1 = nullptr;
  NodePtr expr2 = nullptr;
  NodePtr sub_cmd1 = nullptr;
  NodePtr sub_cmd2 = nullptr;
  std::string identifier_name = "";

  bool is_if = false, is_while = false, is_print = false, is_array_assign = false;

  for (size_t i = 0; i < root.children.size(); ++i)
  {
    const auto &ptr = root.children[i];

    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::EXP)
      {
        if (expr1 == nullptr)
          expr1 = exp(child);
        else
          expr2 = exp(child);
      }
      else if (child.rule == NT::CMD)
      {
        if (sub_cmd1 == nullptr)
          sub_cmd1 = cmd(child);
        else
          sub_cmd2 = cmd(child);
      }
    }
    else
    {
      const auto &token_opt = std::get<std::optional<Token>>(ptr->value);
      if (token_opt.has_value())
      {
        const auto &token = token_opt.value();

        if (token.value == "if")
          is_if = true;
        else if (token.value == "while")
          is_while = true;
        else if (token.value == "System")
          is_print = true;
        else if (token.value == "[")
          is_array_assign = true;
        else if (token.type == TokenType::IDENTIFIER && identifier_name.empty())
        {
          identifier_name = token.value;
        }
      }
    }
  }

  if (is_if)
    return std::make_unique<IfNode>(std::move(expr1), std::move(sub_cmd1), std::move(sub_cmd2));
  if (is_while)
    return std::make_unique<WhileNode>(std::move(expr1), std::move(sub_cmd1));
  if (is_print)
    return std::make_unique<PrintNode>(std::move(expr1));
  if (!identifier_name.empty())
  {
    if (is_array_assign)
      return std::make_unique<ArrayAssignNode>(identifier_name, std::move(expr1), std::move(expr2));
    else
      return std::make_unique<AssignNode>(identifier_name, std::move(expr1));
  }

  return nullptr;
}

NodePtr AST::exp(const PTNode &root)
{
  if (root.rule != NT::EXP)
    return nullptr;

  NodePtr left = greater(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
    return exp2(std::get<PTNode>(root.children[1]->value), std::move(left));
  return left;
}

NodePtr AST::exp2(const PTNode &root, NodePtr left)
{
  if (root.children.empty() || !has_any_member(root.children))
    return left;

  NodePtr right = greater(std::get<PTNode>(root.children[1]->value));
  auto bin_op = std::make_unique<BinOpNode>(BinOp::AND, std::move(left), std::move(right));

  if (root.children.size() > 2)
    return exp2(std::get<PTNode>(root.children[2]->value), std::move(bin_op));
  return bin_op;
}

NodePtr AST::greater(const PTNode &root)
{
  NodePtr left = add(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
    return greater2(std::get<PTNode>(root.children[1]->value), std::move(left));
  return left;
}

NodePtr AST::greater2(const PTNode &root, NodePtr left)
{
  if (root.children.empty() || !has_any_member(root.children))
    return left;

  NodePtr right = add(std::get<PTNode>(root.children[1]->value));
  auto bin_op = std::make_unique<BinOpNode>(BinOp::GT, std::move(left), std::move(right));

  if (root.children.size() > 2)
    return greater2(std::get<PTNode>(root.children[2]->value), std::move(bin_op));
  return bin_op;
}

NodePtr AST::add(const PTNode &root)
{
  NodePtr left = mul(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
    return add2(std::get<PTNode>(root.children[1]->value), std::move(left));
  return left;
}

NodePtr AST::add2(const PTNode &root, NodePtr left)
{
  if (root.children.empty() || !has_any_member(root.children))
    return left;

  auto token = std::get<std::optional<Token>>(root.children[0]->value).value();
  BinOp op = (token.value == "+") ? BinOp::ADD : BinOp::SUB;

  NodePtr right = mul(std::get<PTNode>(root.children[1]->value));
  auto bin_op = std::make_unique<BinOpNode>(op, std::move(left), std::move(right));

  if (root.children.size() > 2)
    return add2(std::get<PTNode>(root.children[2]->value), std::move(bin_op));
  return bin_op;
}

NodePtr AST::mul(const PTNode &root)
{
  NodePtr left = neg(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
    return mul2(std::get<PTNode>(root.children[1]->value), std::move(left));
  return left;
}

NodePtr AST::mul2(const PTNode &root, NodePtr left)
{
  if (root.children.empty() || !has_any_member(root.children))
    return left;

  NodePtr right = neg(std::get<PTNode>(root.children[1]->value));
  auto bin_op = std::make_unique<BinOpNode>(BinOp::MULS, std::move(left), std::move(right));

  if (root.children.size() > 2)
    return mul2(std::get<PTNode>(root.children[2]->value), std::move(bin_op));
  return bin_op;
}

NodePtr AST::neg(const PTNode &root)
{
  if (std::holds_alternative<std::optional<Token>>(root.children[0]->value))
  {
    auto token = std::get<std::optional<Token>>(root.children[0]->value).value();
    if (token.value == "!")
    {
      NodePtr inner_neg = neg(std::get<PTNode>(root.children[1]->value));
      return std::make_unique<NotNode>(std::move(inner_neg));
    }
  }
  return obj(std::get<PTNode>(root.children[0]->value));
}

NodePtr AST::obj(const PTNode &root)
{
  NodePtr base = obj_atom(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
    return obj_met(std::get<PTNode>(root.children[1]->value), std::move(base));
  return base;
}

NodePtr AST::obj_met(const PTNode &root, NodePtr left)
{
  if (root.children.empty() || !has_any_member(root.children))
    return left;

  auto first_token = std::get<std::optional<Token>>(root.children[0]->value).value();

  if (first_token.value == "." && std::get<std::optional<Token>>(root.children[1]->value)->value == "length")
  {
    auto length_node = std::make_unique<LengthNode>(std::move(left));
    if (root.children.size() > 2)
      return obj_met(std::get<PTNode>(root.children[2]->value), std::move(length_node));
    return length_node;
  }

  if (first_token.value == "." && root.children.size() >= 5)
  {
    std::string method_name = std::get<std::optional<Token>>(root.children[1]->value)->value;

    std::vector<NodePtr> arguments;
    if (std::holds_alternative<PTNode>(root.children[3]->value))
    {
      arguments = list_exp(std::get<PTNode>(root.children[3]->value));
    }

    auto call_node = std::make_unique<MethodCallNode>(std::move(left), method_name, std::move(arguments));

    const auto &last_child = root.children.back();
    if (std::holds_alternative<PTNode>(last_child->value) && std::get<PTNode>(last_child->value).rule == ::jc::grammar::NT::OBJMET)
    {
      return obj_met(std::get<PTNode>(last_child->value), std::move(call_node));
    }
    return call_node;
  }

  if (first_token.value == "[")
  {
    NodePtr index_expr = exp(std::get<PTNode>(root.children[1]->value));
    auto array_lookup = std::make_unique<ArrayAccessNode>(std::move(left), std::move(index_expr));

    if (root.children.size() > 3)
      return obj_met(std::get<PTNode>(root.children[3]->value), std::move(array_lookup));
    return array_lookup;
  }

  return left;
}

NodePtr AST::obj_atom(const PTNode &root)
{
  const auto &first_child = root.children[0];

  if (std::holds_alternative<std::optional<Token>>(first_child->value))
  {
    auto token = std::get<std::optional<Token>>(first_child->value).value();

    if (token.value == "new")
    {
      auto second_token = std::get<std::optional<Token>>(root.children[1]->value).value();
      if (second_token.value == "int")
      {
        NodePtr size_expr = exp(std::get<PTNode>(root.children[3]->value));
        return std::make_unique<NewArrayNode>(std::move(size_expr));
      }
      else
      {
        return std::make_unique<NewObjectNode>(second_token.value);
      }
    }
    if (token.value == "(")
      return exp(std::get<PTNode>(root.children[1]->value));
    if (token.value == "true")
      return std::make_unique<BoolNode>(true);
    if (token.value == "false")
      return std::make_unique<BoolNode>(false);
    if (token.value == "this")
      return std::make_unique<ThisNode>();
    if (token.type == TokenType::IDENTIFIER)
      return std::make_unique<IdentifierNode>(token.value);
    if (token.type == TokenType::NUMBER)
      return std::make_unique<NumberNode>(std::stoi(token.value));
  }

  return nullptr;
}

std::vector<NodePtr> AST::list_exp(const PTNode &root)
{
  std::vector<NodePtr> expressions;
  if (root.children.empty() || !has_any_member(root.children))
    return expressions;

  NodePtr current_expr = exp(std::get<PTNode>(root.children[0]->value));
  if (current_expr != nullptr)
  {
    expressions.push_back(std::move(current_expr));
  }

  if (root.children.size() > 2 && std::holds_alternative<PTNode>(root.children[2]->value))
  {
    std::vector<NodePtr> next_exps = list_exp(std::get<PTNode>(root.children[2]->value));
    expressions.reserve(expressions.size() + next_exps.size());
    std::move(next_exps.begin(), next_exps.end(), std::back_inserter(expressions));
  }

  return expressions;
}
