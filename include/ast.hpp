#pragma once
#include <memory>
#include <iostream>
#include <optional>
#include <string>
#include <format>
#include <vector>
#include "types.hpp"
#include "parser.hpp"

namespace jc
{
  namespace ast
  {
    enum class Kind : u8
    {
      // estrutura do programa
      CLASS,
      MAIN_CLASS,
      METHOD,
      PARAM,
      PROGRAM,
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

    struct Node
    {
      Kind kind;
      virtual ~Node() = default;
      virtual void print(std::string prefix = "", bool is_last = true) const = 0;

    protected:
      explicit Node(Kind k) : kind(k) {}
    };

    using NodePtr = std::unique_ptr<Node>;

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
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    // Estrutura do programa

    struct ClassNode : Node
    {
      std::string name;
      std::optional<std::string> parent;
      std::vector<NodePtr> fields;
      std::vector<NodePtr> methods;
      ClassNode() : Node(Kind::CLASS) {}

      void print(std::string prefix = "", bool is_last = true) const override
      {
        std::cout << prefix << (is_last ? "└── " : "├── ") << "ClassNode [" << name << " extends " << parent.value_or("None") << "]\n";
        std::string next_prefix = prefix + (is_last ? "    " : "│   ");
        for (size_t i = 0; i < fields.size(); ++i) {
          bool child_is_last = (i == fields.size() - 1) && methods.empty();
          fields[i]->print(next_prefix, child_is_last);
        }
        for (size_t i = 0; i < methods.size(); ++i) {
          bool child_is_last = (i == methods.size() - 1);
          methods[i]->print(next_prefix, child_is_last);
        }
      }
    };

    struct MainClassNode : Node
    {
      std::string name;
      std::string args_param;
      NodePtr body;
      MainClassNode() : Node(Kind::MAIN_CLASS) {}
      void print(std::string prefix = "", bool is_last = true) const override
      {
        std::cout << prefix << (is_last ? "└── " : "├── ") << "MainClassNode [" << name << "]\n";
        if (body) {
          body->print(prefix + (is_last ? "    " : "│   "), true);
        }
      }
    };

    struct ParamNode : Node
    {
      NodePtr type;
      std::string name;
      ParamNode() : Node(Kind::PARAM) {}
      ParamNode(NodePtr type, std::string name) : Node(Kind::PARAM), type(std::move(type)), name(std::move(name)) {}
      void print(std::string prefix = "", bool is_last = true) const override
      {
        std::cout << prefix << (is_last ? "└── " : "├── ") << "ParamNode [" << name << "]\n";
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

      MethodNode(NodePtr r, std::string n, std::vector<ParamNode*> p, std::vector<NodePtr> l, NodePtr b, NodePtr re): Node(Kind::METHOD), return_type(std::move(r)), name(std::move(n)), params(std::move(p)), locals(std::move(l)), body(std::move(b)), return_expr(std::move(re)) {}

      void print(std::string prefix = "", bool is_last = true) const override
      {
        // TODO
      }
    };

    struct ProgramNode : Node
    {
      NodePtr main_class;
      std::vector<NodePtr> classes;
      ProgramNode() : Node(Kind::PROGRAM) {}

      void print(std::string prefix = "", bool is_last = true) const override
      {
        std::cout << prefix << (is_last ? "└── " : "├── ") << "ProgramNode\n";
        std::string next_prefix = prefix + (is_last ? "    " : "│   ");
        if (main_class) {
          main_class->print(next_prefix, classes.empty());
        }
        for (size_t i = 0; i < classes.size(); ++i) {
          bool child_is_last = (i == classes.size() - 1);
          classes[i]->print(next_prefix, child_is_last);
        }
      }
    };

    struct VarDeclNode : Node
    {
      NodePtr type;
      std::string name;
      VarDeclNode() : Node(Kind::VAR_DECL) {}
      VarDeclNode(NodePtr type, std::string name) : Node(Kind::VAR_DECL), type(std::move(type)), name(std::move(name)) {}
      void print(std::string prefix = "", bool is_last = true) const override
      {
        // TODO
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
      void print(std::string prefix = "", bool is_last = true) const override
      {
        // TODO
      }
    };

    struct AssignNode : Node
    {
      std::string name;
      NodePtr value;
      AssignNode() : Node(Kind::ASSIGN) {}
      AssignNode(std::string name, NodePtr value) : Node(Kind::ASSIGN), name(std::move(name)), value(std::move(value)) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct BlockNode : Node
    {
      std::vector<NodePtr> stmt;
      BlockNode() : Node(Kind::BLOCK) {}
      BlockNode(std::vector<NodePtr> stmt) : Node(Kind::BLOCK), stmt(std::move(stmt)) {}
      void print(std::string prefix = "", bool is_last = true) const override
      {
        // TODO
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
      void print(std::string prefix = "", bool is_last = true) const override
      {
        // TODO
      }
    };

    struct PrintNode : Node
    {
      NodePtr expr;
      PrintNode() : Node(Kind::PRINT) {}
      PrintNode(NodePtr expr) : Node(Kind::PRINT), expr(std::move(expr)) {}
      void print(std::string prefix = "", bool is_last = true) const override
      {
        // TODO
      }
    };

    struct WhileNode : Node
    {
      NodePtr cond;
      NodePtr body;
      WhileNode() : Node(Kind::WHILE) {}
      WhileNode(NodePtr cond, NodePtr body) : Node(Kind::WHILE), cond(std::move(cond)), body(std::move(body)) {}
      void print(std::string prefix = "", bool is_last = true) const override {
        // TODO
      }
    };

    // Expressões

    struct ArrayAccessNode : Node
    {
      NodePtr array;
      NodePtr index;
      ArrayAccessNode() : Node(Kind::ARRAY_ACCESS) {}
      ArrayAccessNode(NodePtr array, NodePtr index) : Node(Kind::ARRAY_ACCESS), array(std::move(array)), index(std::move(index)) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct BinOpNode : Node
    {
      BinOp op;
      NodePtr left;
      NodePtr right;
      BinOpNode(BinOp op, NodePtr left, NodePtr right): Node(Kind::BIN_OP), op(op), left(std::move(left)), right(std::move(right)) {}

      void print(std::string prefix = "", bool is_last = true) const override
      {
        std::string op_str = (op == BinOp::ADD) ? "+" :
                             (op == BinOp::SUB) ? "-" :
                             (op == BinOp::MULS) ? "*" :
                             (op == BinOp::AND) ? "&&" :
                             (op == BinOp::GT) ? ">" : "?";

        std::cout << prefix << (is_last ? "└── " : "├── ") << "BinOpNode [" << op_str << "]\n";

        std::string next_prefix = prefix + (is_last ? "    " : "│   ");

        if (left) left->print(next_prefix, false);
        if (right) right->print(next_prefix, true);
      }
    };

    struct BoolNode : Node
    {
      bool value;
      explicit BoolNode(bool v) : Node(Kind::BOOL), value(v) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct IdentifierNode : Node
    {
      std::string name;
      explicit IdentifierNode(std::string n) : Node(Kind::IDENTIFIER), name(std::move(n)) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct LengthNode : Node
    {
      NodePtr obj;
      LengthNode() : Node(Kind::LENGTH) {}
      LengthNode(NodePtr obj) : Node(Kind::LENGTH), obj(std::move(obj)) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct MethodCallNode : Node
    {
      NodePtr obj;
      std::string method;
      std::vector<NodePtr> args;
      MethodCallNode() : Node(Kind::METHOD_CALL) {}
      MethodCallNode(NodePtr obj, std::string method, std::vector<NodePtr> args) : Node(Kind::METHOD_CALL), obj(std::move(obj)), method(std::move(method)), args(std::move(args)) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct NewArrayNode : Node
    {
      NodePtr size;
      NewArrayNode() : Node(Kind::NEW_ARRAY) {}
      NewArrayNode(NodePtr size) : Node(Kind::NEW_ARRAY), size(std::move(size)) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct NewObjectNode : Node
    {
      std::string class_name;
      NewObjectNode() : Node(Kind::NEW_OBJECT) {}
      NewObjectNode(std::string class_name) : Node(Kind::NEW_OBJECT), class_name(std::move(class_name)) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct NotNode : Node
    {
      NodePtr expr;
      NotNode() : Node(Kind::NOT) {}
      NotNode(NodePtr expr) : Node(Kind::NOT), expr(std::move(expr)) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct NumberNode : Node
    {
      i32 value;
      NumberNode(i32 v) : Node(Kind::NUMBER), value(v) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    struct ThisNode : Node
    {
      ThisNode() : Node(Kind::THIS) {}
      void print(std::string prefix = "", bool is_last = true) const override {}
    };

    using namespace jc::parser;
    class AST
    {
      NodePtr root;

      NodePtr prog(const PTNode &root);
      NodePtr main_c(const PTNode &root);
      NodePtr def_cl(const PTNode &root);
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

    public:
      NodePtr create(const PTree &root);
    };
  } // namespace ast
} // namespace jc