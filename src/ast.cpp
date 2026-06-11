#include "ast.hpp"
#include "utils.hpp"
#include <variant>

using namespace jc::ast;
using namespace jc::grammar;

NodePtr AST::create(const PTree &root)
{
  if (std::holds_alternative<std::optional<Token>>(root.value))
  {
    log::debug("AST vazia.");
    return nullptr;
  }
  const PTNode &root_node = std::get<PTNode>(root.value);
  if (root_node.children.size() == 1 && root_node.rule == start_symbol)
  {
    auto &ptr = root_node.children.front();
    auto &prog_node = std::get<PTNode>(ptr->value);
    return prog(prog_node);
  }
}

NodePtr AST::prog(const PTNode &root)
{
  if (root.rule != NT::PROG)
  {
    log::debug(
        std::format("esperava prog, recebeu {}", symbol_to_string(root.rule)));
    return nullptr;
  }

  ProgramNode *program_ptr = new ProgramNode();
  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      log::debug(std::format("filho: {}", symbol_to_string(child.rule)));
      if (child.rule == NT::MAINC)
      {
        program_ptr->main_class = main_c(child);
      }
      else if (child.rule == NT::DEFCL)
      {
        program_ptr->classes.push_back(def_cl(child));
      }
      else
      {
        log::debug(
            std::format("Prog não deveria ter nenhum estado diferente...: {}",
                        symbol_to_string(child.rule)));
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
    log::debug(std::format("esperava main_c, recebeu {}",
                           symbol_to_string(root.rule)));
    return nullptr;
  }

  MainClassNode *main_class_ptr = new MainClassNode();
  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      log::debug("filho de mainc: " +
                 symbol_to_string(child.rule)); // pq tá dando problema aqui??
      if (child.rule == NT::LISTCMD)
      {
        // log::debug("entrei!");
        main_class_ptr->body = list_cmd(child);
      }
      else
      {
        log::debug(
            std::format("MAINC não deveria ter nenhum estado diferente...: {}",
                        symbol_to_string(child.rule)));
      }
    }
    else
    {
      const auto &child = std::get<std::optional<Token>>(ptr->value);
      if (child.has_value())
      {
        const auto &token = child.value();
        if (token.type == TokenType::IDENTIFIER &&
            main_class_ptr->name.empty())
        {
          main_class_ptr->name = token.value;
        }
        else if (token.type == TokenType::IDENTIFIER &&
                 !main_class_ptr->name.empty() &&
                 main_class_ptr->args_param.empty())
        {
          main_class_ptr->args_param = token.value;
        }
      }
      else
        log::debug("Nó vazio detectado em MAINC");
    }
  }

  return main_class_ptr;
}

NodePtr AST::def_cl(const PTNode &root)
{
  if (root.rule != NT::DEFCL)
  {
    log::debug(std::format("esperava def_cl, recebeu {}",
                           symbol_to_string(root.rule)));
    return nullptr;
  }

  bool search_for_inheritance = false;
  ClassNode *class_node_ptr = new ClassNode();
  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      log::debug(std::format("child de def_cl: {} ({})", symbol_to_string(child.rule), (int)child.rule));
      if (child.rule == NT::DEFVAR)
      {
        class_node_ptr->fields = def_var(child);
      }
      else if (child.rule == NT::DEFMET)
      {
        class_node_ptr->methods = def_met(child);
      }
      else
      {
        log::debug(
            std::format("DEFCL não deveria ter nenhum estado diferente...: {}",
                        symbol_to_string(child.rule)));
      }
    }
    else
    {
      const auto &child = std::get<std::optional<Token>>(ptr->value);
      if (child.has_value())
      {
        auto &token = child.value();
        if (token.type == TokenType::IDENTIFIER &&
            class_node_ptr->name.empty())
        {
          class_node_ptr->name = token.value;
        }
        else if (token.type == TokenType::IDENTIFIER &&
                 !class_node_ptr->name.empty() &&
                 !class_node_ptr->parent.has_value() &&
                 search_for_inheritance)
        {
          class_node_ptr->parent = std::make_optional(token.value);
          search_for_inheritance = false;
        }
        else if (token.type == TokenType::KEYWORD &&
                 token.value == "EXTENDS" &&
                 !class_node_ptr->parent.has_value())
        {
          // não temos como olhar pro próximo nó (eu acho!!) então vamos marcar
          // que estamos procurando!
          search_for_inheritance = true;
        }
      }
      else
        log::debug("Nó vazio detectado em DEFCL");
    }
  }

  return class_node_ptr;
}

std::vector<NodePtr> AST::def_var(const PTNode &root)
{
  std::vector<NodePtr> v;

  if (root.rule != NT::DEFVAR)
  {
    log::debug("esperava def_var, recebeu " + symbol_to_string(root.rule));
    return v;
  }

  if (root.children.empty())
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
      {
        current_type = type(child);
      }
      else if (child.rule == NT::DEFVAR)
      {
        next_def_var = &child;
      }
    }
    else
    {
      const auto &token_opt = std::get<std::optional<Token>>(ptr->value);
      if (token_opt.has_value())
      {
        auto token = token_opt.value();
        if (token.type == TokenType::IDENTIFIER && var_name.empty())
        {
          var_name = token.value;
        }
      }
    }
  }

  if (current_type != nullptr && !var_name.empty())
  {
    TypeNode *type = static_cast<TypeNode *>(current_type);
    auto *var_decl = new VarDeclNode(*type, var_name);
    v.push_back(var_decl);
  }

  if (next_def_var != nullptr)
  {
    std::vector<NodePtr> next_vars = def_var(*next_def_var);
    v.insert(v.end(), next_vars.begin(), next_vars.end());
  }

  return v;
}

NodePtr AST::type(const PTNode &root)
{
  if (root.rule != NT::TYPE)
  {
    log::debug("esperava type, recebeu " + symbol_to_string(root.rule));
    return nullptr;
  }

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
          {
            return new TypeNode(TypeKind::INT_ARRAY);
          }
          else if (root.children.size() == 1)
            return new TypeNode(TypeKind::INT);
          else
          {
            log::debug("que tipo int é esse?");
            return nullptr;
          }
        }
        else if (token.type == TokenType::KEYWORD && token.value == "boolean")
        {
          return new TypeNode(TypeKind::BOOLEAN);
        }
        else if (token.type == TokenType::IDENTIFIER)
        {
          return new TypeNode(TypeKind::CLASS, token.value);
        }
      }
    }
  }

  log::debug("não foi possível reconhecer o tipo na regra TYPE");
  return nullptr;
}

std::vector<NodePtr>
AST::def_met(const PTNode &root)
{
  std::vector<NodePtr> v;

  if (root.rule != NT::DEFMET)
  {
    log::debug("esperava def_met, recebeu " + symbol_to_string(root.rule));
    return v;
  }
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
      {
        return_type = type(child);
      }
      else if (child.rule == NT::ARGS)
      {
        method_args = args(child);
      }
      else if (child.rule == NT::DEFVAR)
      {
        local_vars = def_var(child);
      }
      else if (child.rule == NT::LISTCMD)
      {
        body_cmds = list_cmd(child);
        // body_cmds = list_cmd(child);
      }
      else if (child.rule == NT::EXP)
      {
        return_expr = exp(child);
        // TODO: chamar exp aqui
      }
      else if (child.rule == NT::DEFMET)
      {
        next_def_met = &child;
      }
    }
    else
    {
      const auto &token_opt = std::get<std::optional<Token>>(ptr->value);
      if (token_opt.has_value())
      {
        auto token = token_opt.value();

        if (token.type == TokenType::IDENTIFIER && method_name.empty())
        {
          method_name = token.value;
        }
      }
    }
  }

  if (return_type != nullptr && !method_name.empty())
  {
    auto *method_decl = new MethodNode(
        return_type,
        method_name,
        std::move(method_args),
        std::move(local_vars),
        body_cmds,
        return_expr);
    v.push_back(method_decl);
  }

  if (next_def_met != nullptr)
  {
    std::vector<NodePtr> next_methods = def_met(*next_def_met);
    v.reserve(v.size() + next_methods.size());
    // v.insert(v.end(), next_methods.begin(), next_methods.end());
    // eficiência né pai
    std::move(next_methods.begin(), next_methods.end(), std::back_inserter(v));

    // delete &next_methods;
  }

  return v;
}

std::vector<ParamNode *> AST::args(const PTNode &root)
{
  std::vector<ParamNode *> v;

  if (root.rule != NT::ARGS)
  {
    log::debug("esperava args, recebeu " + symbol_to_string(root.rule));
    return v;
  }

  NodePtr current_type = nullptr;
  std::string param_name = "";
  const PTNode *next_args = nullptr;

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::TYPE)
      {
        current_type = type(child);
      }
      else if (child.rule == NT::ARGS)
      {
        next_args = &child;
      }
    }
    else
    {
      const auto &token_opt = std::get<std::optional<Token>>(ptr->value);
      if (token_opt.has_value())
      {
        auto token = token_opt.value();
        if (token.type == TokenType::IDENTIFIER && param_name.empty())
        {
          param_name = token.value;
        }
      }
    }
  }

  if (current_type != nullptr && !param_name.empty())
  {
    auto *param_node = new ParamNode(current_type, std::move(param_name));
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
  {
    log::debug("esperava list_cmd, recebeu " + symbol_to_string(root.rule));
    return nullptr;
  }

  if (root.children.empty())
  {
    return new BlockNode();
  }

  log::debug("entrei em listcmd");
  std::vector<NodePtr> cmds;
  NodePtr current_cmd = nullptr;
  const PTNode *next_list = nullptr;

  for (const auto &ptr : root.children)
  {
    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::CMD)
      {
        current_cmd = cmd(child);
      }
      else if (child.rule == NT::LISTCMD)
      {
        next_list = &child;
      }
    }
  }

  if (current_cmd != nullptr)
  {
    cmds.push_back(current_cmd);
  }

  if (next_list != nullptr)
  {
    NodePtr next_block_node = list_cmd(*next_list);
    if (next_block_node != nullptr)
    {
      auto *sub_block = static_cast<BlockNode *>(next_block_node);

      cmds.reserve(cmds.size() + sub_block->stmt.size());
      std::move(sub_block->stmt.begin(), sub_block->stmt.end(), std::back_inserter(cmds));

      delete sub_block;
    }
  }

  return new BlockNode(std::move(cmds));
}

NodePtr AST::cmd(const PTNode &root)
{
  if (root.rule != NT::CMD)
  {
    log::debug("esperava cmd, recebeu " + symbol_to_string(root.rule));
    return nullptr;
  }

  if (root.children.empty())
    return nullptr;

  log::debug("entrei em cmd");
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

  bool is_if = false;
  bool is_while = false;
  bool is_print = false;
  bool is_array_assign = false;

  for (size_t i = 0; i < root.children.size(); ++i)
  {
    const auto &ptr = root.children[i];

    if (std::holds_alternative<PTNode>(ptr->value))
    {
      const auto &child = std::get<PTNode>(ptr->value);
      if (child.rule == NT::EXP)
      {
        if (expr1 == nullptr)
          expr1 = exp(child); // Primeiro nó de expressão encontrado
        else
          expr2 = exp(child); // Segundo nó (ex: index de array ou valor atribuído)
      }
      else if (child.rule == NT::CMD)
      {
        if (sub_cmd1 == nullptr)
          sub_cmd1 = cmd(child); // Then branch do IF / Corpo do WHILE
        else
          sub_cmd2 = cmd(child); // Else branch do IF
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
          is_print = true; // Identifica o System.out.println
        else if (token.value == "[")
          is_array_assign = true; // Identifica que é atribuição de array: id '[' exp ']' '=' exp ';'
        else if (token.type == TokenType::IDENTIFIER && identifier_name.empty())
        {
          identifier_name = token.value;
        }
      }
    }
  }

  // 4. Montagem dos nós da AST baseada nas flags ativadas
  if (is_if)
  {
    return new IfNode(expr1, sub_cmd1, sub_cmd2);
  }
  if (is_while)
  {
    return new WhileNode(expr1, sub_cmd1);
  }
  if (is_print)
  {
    return new PrintNode(expr1);
  }
  if (!identifier_name.empty())
  {
    if (is_array_assign)
    {
      // Id '[' Exp ']' '=' Exp ';'
      // expr1 é o índice do array, expr2 é o valor a ser guardado
      return new ArrayAssignNode(identifier_name, expr1, expr2);
    }
    else
    {
      // Id '=' Exp ';'
      // expr1 é o valor sendo atribuído
      return new AssignNode(identifier_name, expr1);
    }
  }

  log::debug("Comando não reconhecido ou mal estruturado");
  return nullptr;
}

NodePtr AST::exp(const PTNode &root)
{
  if (root.rule != ::jc::grammar::NT::EXP)
    return nullptr;

  // Exp -> Greater Exp2
  NodePtr left = greater(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
  {
    return exp2(std::get<PTNode>(root.children[1]->value), left);
  }
  return left;
}

NodePtr AST::exp2(const PTNode &root, NodePtr left)
{
  if (root.children.empty())
    return left; // Caso Lambda: retorna o que já temos

  // Exp2 -> '&&' Greater Exp2
  NodePtr right = greater(std::get<PTNode>(root.children[1]->value));
  auto *bin_op = new BinOpNode(BinOp::AND, left, right);

  if (root.children.size() > 2)
  {
    return exp2(std::get<PTNode>(root.children[2]->value), bin_op);
  }
  return bin_op;
}

NodePtr AST::greater(const PTNode &root)
{
  // Greater -> Add Greater2
  NodePtr left = add(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
  {
    return greater2(std::get<PTNode>(root.children[1]->value), left);
  }
  return left;
}

NodePtr AST::greater2(const PTNode &root, NodePtr left)
{
  if (root.children.empty())
    return left;

  // Greater2 -> '>' Add Greater2
  NodePtr right = add(std::get<PTNode>(root.children[1]->value));
  auto *bin_op = new BinOpNode(BinOp::GT, left, right);

  if (root.children.size() > 2)
  {
    return greater2(std::get<PTNode>(root.children[2]->value), bin_op);
  }
  return bin_op;
}

NodePtr AST::add(const PTNode &root)
{
  // Add -> Mul Add2
  NodePtr left = mul(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
  {
    return add2(std::get<PTNode>(root.children[1]->value), left);
  }
  return left;
}

NodePtr AST::add2(const PTNode &root, NodePtr left)
{
  if (root.children.empty())
    return left;

  // Add2 -> '+' Mul Add2 | '-' Mul Add2
  auto token = std::get<std::optional<Token>>(root.children[0]->value).value();
  BinOp op = (token.value == "+") ? BinOp::ADD : BinOp::SUB;

  NodePtr right = mul(std::get<PTNode>(root.children[1]->value));
  auto *bin_op = new BinOpNode(op, left, right);

  if (root.children.size() > 2)
  {
    return add2(std::get<PTNode>(root.children[2]->value), bin_op);
  }
  return bin_op;
}

NodePtr AST::mul(const PTNode &root)
{
  // Mul -> Neg Mul2
  NodePtr left = neg(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
  {
    return mul2(std::get<PTNode>(root.children[1]->value), left);
  }
  return left;
}

NodePtr AST::mul2(const PTNode &root, NodePtr left)
{
  if (root.children.empty())
    return left;

  // Mul2 -> '*' Neg Mul2
  NodePtr right = neg(std::get<PTNode>(root.children[1]->value));
  auto *bin_op = new BinOpNode(BinOp::MULS, left, right);

  if (root.children.size() > 2)
  {
    return mul2(std::get<PTNode>(root.children[2]->value), bin_op);
  }
  return bin_op;
}

NodePtr AST::neg(const PTNode &root)
{
  // Neg -> '!' Neg
  if (std::holds_alternative<std::optional<Token>>(root.children[0]->value))
  {
    auto token = std::get<std::optional<Token>>(root.children[0]->value).value();
    if (token.value == "!")
    {
      NodePtr inner_neg = neg(std::get<PTNode>(root.children[1]->value));
      return new NotNode(inner_neg);
    }
  }
  // Neg -> Obj
  return obj(std::get<PTNode>(root.children[0]->value));
}

NodePtr AST::obj(const PTNode &root)
{
  // Obj -> ObjAtom ObjMet
  NodePtr base = obj_atom(std::get<PTNode>(root.children[0]->value));
  if (root.children.size() > 1)
  {
    return obj_met(std::get<PTNode>(root.children[1]->value), base);
  }
  return base;
}

NodePtr AST::obj_met(const PTNode &root, NodePtr left)
{
  if (root.children.empty())
    return left;

  auto first_token = std::get<std::optional<Token>>(root.children[0]->value).value();

  // Caso 1: '.' 'length' ObjMet
  if (first_token.value == "." && std::get<std::optional<Token>>(root.children[1]->value)->value == "length")
  {
    auto *length_node = new LengthNode(left);
    if (root.children.size() > 2)
    {
      return obj_met(std::get<PTNode>(root.children[2]->value), length_node);
    }
    return length_node;
  }

  // Caso 2: '.' Id '(' ListExp ')' ObjMet (Chamada de Método)
  if (first_token.value == "." && root.children.size() >= 5)
  {
    std::string method_name = std::get<std::optional<Token>>(root.children[1]->value)->value;

    // ListExp pode estar na posição 3
    std::vector<NodePtr> arguments;
    if (std::holds_alternative<PTNode>(root.children[3]->value))
    {
      arguments = list_exp(std::get<PTNode>(root.children[3]->value));
    }

    auto *call_node = new MethodCallNode(left, method_name, std::move(arguments));

    // Captura o último elemento se ele for um ObjMet subsequente
    const auto &last_child = root.children.back();
    if (std::holds_alternative<PTNode>(last_child->value) && std::get<PTNode>(last_child->value).rule == ::jc::grammar::NT::OBJMET)
    {
      return obj_met(std::get<PTNode>(last_child->value), call_node);
    }
    return call_node;
  }

  // Caso 3: '[' Exp ']' ObjMet (Indexação de Array)
  if (first_token.value == "[")
  {
    NodePtr index_expr = exp(std::get<PTNode>(root.children[1]->value));
    auto *array_lookup = new ArrayAccessNode(left, index_expr);

    if (root.children.size() > 3)
    {
      return obj_met(std::get<PTNode>(root.children[3]->value), array_lookup);
    }
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
        // new int [ Exp ]
        NodePtr size_expr = exp(std::get<PTNode>(root.children[3]->value));
        return new NewArrayNode(size_expr);
      }
      else
      {
        // new Id ( )
        return new NewObjectNode(second_token.value);
      }
    }
    if (token.value == "(")
    {
      // ( Exp ) -> Retorna apenas a expressão interna limpa
      return exp(std::get<PTNode>(root.children[1]->value));
    }
    if (token.value == "true")
      return new BoolNode(true);
    if (token.value == "false")
      return new BoolNode(false);
    if (token.value == "this")
      return new ThisNode();

    if (token.type == TokenType::IDENTIFIER)
      return new IdentifierNode(token.value);
    if (token.type == TokenType::NUMBER)
      return new NumberNode(std::stoi(token.value));
  }

  log::debug("ObjAtom inválido capturado");
  return nullptr;
}

std::vector<NodePtr> AST::list_exp(const PTNode &root)
{
  std::vector<NodePtr> expressions;
  if (root.children.empty())
    return expressions;

  // Caso 1: Exp ',' ListExp ou apenas Exp
  NodePtr current_expr = exp(std::get<PTNode>(root.children[0]->value));
  if (current_expr != nullptr)
  {
    expressions.push_back(current_expr);
  }

  // Se houver mais elementos (tamanho > 2 significa que tem a vírgula e a sublista)
  if (root.children.size() > 2 && std::holds_alternative<PTNode>(root.children[2]->value))
  {
    std::vector<NodePtr> next_exps = list_exp(std::get<PTNode>(root.children[2]->value));
    expressions.reserve(expressions.size() + next_exps.size());
    std::move(next_exps.begin(), next_exps.end(), std::back_inserter(expressions));
  }

  return expressions;
}