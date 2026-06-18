#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include "types.hpp"
#include "parser.hpp"
#include "utils.hpp"

namespace jc
{
  namespace ast
  {
    enum class Kind : u8
    {
      // estrutura do programa
      PROGRAM,
      MAIN_CLASS,
      MAIN_METHOD,
      CLASS,
      METHOD,
      PARAM,
      VAR_DECL,
      TYPE,

      // comandos
      ARRAY_ASSIGN,
      ASSIGN,
      BLOCK,
      IF,
      PRINT,
      WHILE,

      // expressões
      ARRAY_ACCESS,
      BOOL,
      BIN_OP,
      IDENTIFIER,
      LENGTH,
      METHOD_CALL,
      NEW_ARRAY,
      NEW_OBJECT,
      NOT,
      NUMBER,
      THIS,
    };

    enum class TypeKind : u8
    {
      BOOLEAN,
      CLASS,
      INT,
      INT_ARRAY,
    };

    enum class BinOp : u8
    {
      ADD,
      AND,
      GT,
      MULS,
      SUB,
    };

    inline std::string to_string(BinOp op)
    {
      switch (op)
      {
      case BinOp::ADD:
        return "+";
      case BinOp::SUB:
        return "-";
      case BinOp::MULS:
        return "*";
      case BinOp::AND:
        return "&&";
      case BinOp::GT:
        return ">";
      }
      return "?";
    }

    struct Node
    {
      Kind kind;
      virtual ~Node() = default;

      virtual std::string label() const = 0;

      virtual std::vector<const Node *> children() const { return {}; }

      void print(std::ostream &os = std::cout, const std::string &tabs = "", bool is_last = true) const
      {
        os << std::format("{}{}{}\n", tabs, is_last ? "└── " : "├── ", label());

        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        auto kids = children();
        for (size_t i = 0; i < kids.size(); ++i)
          kids[i]->print(os, next_tabs, i + 1 == kids.size());
      }

      void print(const std::string &tabs, bool is_last = true) const
      {
        print(std::cout, tabs, is_last);
      }

    protected:
      explicit Node(Kind k) : kind(k) {}
    };

    using NodePtr = ::std::unique_ptr<Node>;

    struct TypeNode : Node
    {
      TypeKind kind;
      std::string name;
      TypeNode() : Node(Kind::TYPE) {}
      TypeNode(TypeKind tk) : Node(Kind::TYPE) { kind = tk; }
      TypeNode(TypeKind tk, std::string name) : Node(Kind::TYPE)
      {
        kind = tk;
        this->name = name;
      }

      std::string label() const override
      {
        if (kind == TypeKind::CLASS)
          return std::format("Type [{}]", name);
        return std::format("Type [{}]", jc::to_string(kind));
      }
    };

    // Estrutura do programa

    struct ClassNode : Node
    {
      std::string name;
      std::optional<std::string> parent;
      std::vector<NodePtr> fields;
      std::vector<NodePtr> methods;
      ClassNode() : Node(Kind::CLASS) {}

      std::string label() const override
      {
        return std::format("Class [{} extends {}]", name, parent.value_or("None"));
      }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        for (auto &f : fields)
          if (f)
            result.push_back(f.get());
        for (auto &m : methods)
          if (m)
            result.push_back(m.get());
        return result;
      }
    };

    struct MainClassNode : Node
    {
      std::string name;
      NodePtr main_method;
      MainClassNode() : Node(Kind::MAIN_CLASS) {}

      std::string label() const override { return std::format("MainClass [{}]", name); }

      std::vector<const Node *> children() const override
      {
        if (main_method)
          return {main_method.get()};
        return {};
      }
    };

    struct MainMethodNode : Node
    {
      std::string name;
      std::string param;
      std::string return_type;
      NodePtr body;
      MainMethodNode() : Node(Kind::MAIN_METHOD) {}
      MainMethodNode(std::string n, std::string p, std::string r, NodePtr b) : Node(Kind::MAIN_METHOD), name(std::move(n)), param(std::move(p)), return_type(std::move(r)), body(std::move(b)) {}

      std::string label() const override
      {
        return std::format("MainMethod[{}, args: {}, return: {}]", name, param, return_type);
      }

      std::vector<const Node *> children() const override
      {
        if (body)
          return {body.get()};
        return {};
      }
    };

    struct ParamNode : Node
    {
      NodePtr type;
      std::string name;
      ParamNode() : Node(Kind::PARAM) {}
      ParamNode(NodePtr type, std::string name) : Node(Kind::PARAM), type(std::move(type)), name(std::move(name)) {}

      std::string label() const override { return std::format("Param[{}]", name); }

      std::vector<const Node *> children() const override
      {
        if (type)
          return {type.get()};
        return {};
      }
    };

    struct MethodNode : Node
    {
      NodePtr return_type;
      std::string name;
      std::vector<ParamNode *> params;
      std::vector<NodePtr> locals;
      NodePtr body;
      NodePtr return_expr;

      MethodNode() : Node(Kind::METHOD) {}

      MethodNode(NodePtr r, std::string n, std::vector<ParamNode *> p, std::vector<NodePtr> l, NodePtr b, NodePtr re) : Node(Kind::METHOD), return_type(std::move(r)), name(std::move(n)), params(std::move(p)), locals(std::move(l)), body(std::move(b)), return_expr(std::move(re)) {}

      std::string label() const override { return std::format("Method[{}]", name); }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        if (return_type)
          result.push_back(return_type.get());
        for (auto *p : params)
          if (p)
            result.push_back(p);
        for (auto &l : locals)
          if (l)
            result.push_back(l.get());
        if (body)
          result.push_back(body.get());
        if (return_expr)
          result.push_back(return_expr.get());
        return result;
      }
    };

    struct ProgramNode : Node
    {
      NodePtr main_class;
      std::vector<NodePtr> classes;
      ProgramNode() : Node(Kind::PROGRAM) {}

      std::string label() const override { return "Program"; }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        if (main_class)
          result.push_back(main_class.get());
        for (auto &c : classes)
          if (c)
            result.push_back(c.get());
        return result;
      }
    };

    struct VarDeclNode : Node
    {
      NodePtr type;
      std::string name;
      VarDeclNode() : Node(Kind::VAR_DECL) {}
      VarDeclNode(NodePtr type, std::string name) : Node(Kind::VAR_DECL), type(std::move(type)), name(std::move(name)) {}

      std::string label() const override { return std::format("Var[{}]", name); }

      std::vector<const Node *> children() const override
      {
        if (type)
          return {type.get()};
        return {};
      }
    };

    // Comandos

    struct ArrayAssignNode : Node
    {
      std::string name;
      NodePtr index;
      NodePtr value;
      ArrayAssignNode() : Node(Kind::ARRAY_ASSIGN) {}
      ArrayAssignNode(std::string name, NodePtr index, NodePtr value)
          : Node(Kind::ARRAY_ASSIGN), name(std::move(name)), index(std::move(index)), value(std::move(value)) {}

      std::string label() const override { return std::format("ArrayAssign[{}]", name); }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        if (index)
          result.push_back(index.get());
        if (value)
          result.push_back(value.get());
        return result;
      }
    };

    struct AssignNode : Node
    {
      std::string name;
      NodePtr value;
      AssignNode() : Node(Kind::ASSIGN) {}
      AssignNode(std::string name, NodePtr value) : Node(Kind::ASSIGN), name(std::move(name)), value(std::move(value)) {}

      std::string label() const override { return std::format("Assign[{}]", name); }

      std::vector<const Node *> children() const override
      {
        if (value)
          return {value.get()};
        return {};
      }
    };

    struct BlockNode : Node
    {
      std::vector<NodePtr> stmt;
      BlockNode() : Node(Kind::BLOCK) {}
      BlockNode(std::vector<NodePtr> stmt) : Node(Kind::BLOCK), stmt(std::move(stmt)) {}

      std::string label() const override { return "Block"; }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        for (auto &s : stmt)
          if (s)
            result.push_back(s.get());
        return result;
      }
    };

    struct IfNode : Node
    {
      NodePtr cond;
      NodePtr then_branch;
      NodePtr else_branch;
      IfNode() : Node(Kind::IF) {}
      IfNode(NodePtr cond, NodePtr then, NodePtr else_b)
          : Node(Kind::IF), cond(std::move(cond)), then_branch(std::move(then)), else_branch(std::move(else_b)) {}

      std::string label() const override { return "If"; }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        if (cond)
          result.push_back(cond.get());
        if (then_branch)
          result.push_back(then_branch.get());
        if (else_branch)
          result.push_back(else_branch.get());
        return result;
      }
    };

    struct PrintNode : Node
    {
      NodePtr expr;
      PrintNode() : Node(Kind::PRINT) {}
      PrintNode(NodePtr expr) : Node(Kind::PRINT), expr(std::move(expr)) {}

      std::string label() const override { return "Print"; }

      std::vector<const Node *> children() const override
      {
        if (expr)
          return {expr.get()};
        return {};
      }
    };

    struct WhileNode : Node
    {
      NodePtr cond;
      NodePtr body;
      WhileNode() : Node(Kind::WHILE) {}
      WhileNode(NodePtr cond, NodePtr body) : Node(Kind::WHILE), cond(std::move(cond)), body(std::move(body)) {}

      // Antes estava "WhileNode" — inconsistente com "If" e "Print".
      std::string label() const override { return "While"; }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        if (cond)
          result.push_back(cond.get());
        if (body)
          result.push_back(body.get());
        return result;
      }
    };

    // Expressões

    struct ArrayAccessNode : Node
    {
      NodePtr array;
      NodePtr index;
      ArrayAccessNode() : Node(Kind::ARRAY_ACCESS) {}
      ArrayAccessNode(NodePtr array, NodePtr index) : Node(Kind::ARRAY_ACCESS), array(std::move(array)), index(std::move(index)) {}

      // Antes estava "ArrayAccessNode" — os outros nós não usam o sufixo "Node".
      std::string label() const override { return "ArrayAccess"; }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        if (array)
          result.push_back(array.get());
        if (index)
          result.push_back(index.get());
        return result;
      }
    };

    struct BinOpNode : Node
    {
      BinOp op;
      NodePtr left;
      NodePtr right;
      BinOpNode(BinOp op, NodePtr left, NodePtr right) : Node(Kind::BIN_OP), op(op), left(std::move(left)), right(std::move(right)) {}

      std::string label() const override { return std::format("BinOp [{}]", to_string(op)); }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        if (left)
          result.push_back(left.get());
        if (right)
          result.push_back(right.get());
        return result;
      }
    };

    struct BoolNode : Node
    {
      bool value;
      explicit BoolNode(bool v) : Node(Kind::BOOL), value(v) {}
      std::string label() const override { return std::format("Bool[{}]", value ? "true" : "false"); }
    };

    struct IdentifierNode : Node
    {
      std::string name;
      explicit IdentifierNode(std::string n) : Node(Kind::IDENTIFIER), name(std::move(n)) {}
      std::string label() const override { return std::format("Id[{}]", name); }
    };

    struct LengthNode : Node
    {
      NodePtr obj;
      LengthNode() : Node(Kind::LENGTH) {}
      LengthNode(NodePtr obj) : Node(Kind::LENGTH), obj(std::move(obj)) {}

      std::string label() const override { return "Length"; }

      std::vector<const Node *> children() const override
      {
        if (obj)
          return {obj.get()};
        return {};
      }
    };

    struct MethodCallNode : Node
    {
      NodePtr obj;
      std::string method;
      std::vector<NodePtr> args;
      MethodCallNode() : Node(Kind::METHOD_CALL) {}
      MethodCallNode(NodePtr obj, std::string method, std::vector<NodePtr> args) : Node(Kind::METHOD_CALL), obj(std::move(obj)), method(std::move(method)), args(std::move(args)) {}

      std::string label() const override { return std::format("MethodCall[{}]", method); }

      std::vector<const Node *> children() const override
      {
        std::vector<const Node *> result;
        if (obj)
          result.push_back(obj.get());
        for (auto &a : args)
          if (a)
            result.push_back(a.get());
        return result;
      }
    };

    struct NewArrayNode : Node
    {
      NodePtr size;
      NewArrayNode() : Node(Kind::NEW_ARRAY) {}
      NewArrayNode(NodePtr size) : Node(Kind::NEW_ARRAY), size(std::move(size)) {}

      std::string label() const override { return "NewIntArray"; }

      std::vector<const Node *> children() const override
      {
        if (size)
          return {size.get()};
        return {};
      }
    };

    struct NewObjectNode : Node
    {
      std::string class_name;
      NewObjectNode() : Node(Kind::NEW_OBJECT) {}
      NewObjectNode(std::string class_name) : Node(Kind::NEW_OBJECT), class_name(std::move(class_name)) {}

      std::string label() const override { return std::format("NewObject[{}]", class_name); }
    };

    struct NotNode : Node
    {
      NodePtr expr;
      NotNode() : Node(Kind::NOT) {}
      NotNode(NodePtr expr) : Node(Kind::NOT), expr(std::move(expr)) {}

      std::string label() const override { return "Not"; }

      std::vector<const Node *> children() const override
      {
        if (expr)
          return {expr.get()};
        return {};
      }
    };

    struct NumberNode : Node
    {
      i32 value;
      NumberNode(i32 v) : Node(Kind::NUMBER), value(v) {}
      std::string label() const override { return std::format("Number[{}]", value); }
    };

    struct ThisNode : Node
    {
      ThisNode() : Node(Kind::THIS) {}
      std::string label() const override { return "This"; }
    };

    inline void to_dot(std::ostream &os, const Node *root, std::string_view graph_name = "AST")
    {
      os << "digraph " << graph_name << " {\n";
      os << "  node [shape=box, fontname=\"monospace\", fontsize=10];\n";

      int counter = 0;
      std::function<int(const Node *)> visit = [&](const Node *node) -> int
      {
        int id = counter++;

        std::string escaped;
        for (char c : node->label())
        {
          if (c == '"' || c == '\\')
            escaped.push_back('\\');
          escaped.push_back(c);
        }
        os << "  n" << id << " [label=\"" << escaped << "\"];\n";

        for (const Node *child : node->children())
          os << "  n" << id << " -> n" << visit(child) << ";\n";

        return id;
      };

      if (root)
        visit(root);

      os << "}\n";
    }

    using namespace jc::parser;
    class AST
    {
    private:
      NodePtr root;

      NodePtr prog(const PTNode &root);
      NodePtr main_c(const PTNode &root);
      std::vector<NodePtr> def_cl(const PTNode &root);
      NodePtr cmd(const PTNode &root);
      NodePtr type(const PTNode &root);
      NodePtr list_cmd(const PTNode &root);
      std::vector<ParamNode *> args(const PTNode &root);
      std::vector<NodePtr> def_var(const PTNode &root);
      std::vector<NodePtr> def_met(const PTNode &root);
      NodePtr exp(const PTNode &root);
      NodePtr exp2(const PTNode &root, NodePtr left);
      NodePtr greater(const PTNode &root);
      NodePtr greater2(const PTNode &root, NodePtr left);
      NodePtr add(const PTNode &root);
      NodePtr add2(const PTNode &root, NodePtr left);
      NodePtr mul(const PTNode &root);
      NodePtr mul2(const PTNode &root, NodePtr left);
      NodePtr neg(const PTNode &root);
      NodePtr obj(const PTNode &root);
      NodePtr obj_met(const PTNode &root, NodePtr left);
      NodePtr obj_atom(const PTNode &root);
      std::vector<NodePtr> list_exp(const PTNode &root);

      bool has_any_member(const std::vector<std::unique_ptr<PTree>> &children)
      {
        for (const auto &child : children)
        {
          if (std::holds_alternative<std::optional<Token>>(child->value))
          {
            const auto &token_opt = std::get<std::optional<Token>>(child->value);
            if (token_opt.has_value())
              return true;
          }
          else
          {
            const auto &rule = std::get<PTNode>(child->value);
            return has_any_member(rule.children);
          }
        }

        return false;
      }

    public:
      NodePtr create(const PTree &root);
    };
  } // namespace ast
} // namespace jc