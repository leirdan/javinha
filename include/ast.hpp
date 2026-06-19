#pragma once
#include <memory>
#include <iostream>
#include <optional>
#include <string>
#include <format>
#include <vector>
#include "types.hpp"
#include "utils.hpp"
#include "parser.hpp"

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
      STRING_ARRAY,
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
      virtual void print(std::string tabs = "", bool is_last = true) const = 0;

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
      void print(std::string tabs = "", bool is_last = true) const override
      {
        if (kind == TypeKind::CLASS)
        {
          std::cout << tabs << (is_last ? "└── " : "├── ") << "Type [" << name << "]\n";
        }
        else
        {
          std::cout << tabs << (is_last ? "└── " : "├── ") << "Type [" << jc::to_string(kind) << "]\n";
        }
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

      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Class [" << name << " extends " << parent.value_or("None") << "]\n";
        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        for (size_t i = 0; i < fields.size(); ++i)
        {
          bool child_is_last = (i == fields.size() - 1) && methods.empty();
          fields[i]->print(next_tabs, child_is_last);
        }
        for (size_t i = 0; i < methods.size(); ++i)
        {
          bool child_is_last = (i == methods.size() - 1);
          methods[i]->print(next_tabs, child_is_last);
        }
      }
    };

    struct MainClassNode : Node
    {
      std::string name;
      NodePtr main_method;
      MainClassNode() : Node(Kind::MAIN_CLASS) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "MainClass [" << name << "]\n";
        if (main_method)
        {
          main_method->print(tabs + (is_last ? "    " : "│   "), true);
        }
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
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "MainMethod[" << name << ", args: " << param << ", return: " << return_type << "]\n";
        if (body)
          body->print(tabs + (is_last ? "    " : "│   "), true);
      }
    };

    struct ParamNode : Node
    {
      NodePtr type;
      std::string name;
      ParamNode() : Node(Kind::PARAM) {}
      ParamNode(NodePtr type, std::string name) : Node(Kind::PARAM), type(std::move(type)), name(std::move(name)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Param[" << name << "]\n";
        if (type)
          type->print(tabs + (is_last ? "    " : "│   "), true);
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

      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Method[" << name << "" << "]\n";

        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        if (return_type)
          return_type->print(next_tabs);

        for (size_t i = 0; i < params.size(); ++i)
        {
          bool child_is_last = (i == params.size() - 1);
          params[i]->print(next_tabs, child_is_last);
        }

        for (size_t i = 0; i < locals.size(); ++i)
        {
          bool child_is_last = (i == locals.size() - 1);
          locals[i]->print(next_tabs, child_is_last);
        }

        if (body)
          body->print(next_tabs);

        if (return_expr)
          return_expr->print(next_tabs);
      }
    };

    struct ProgramNode : Node
    {
      NodePtr main_class;
      std::vector<NodePtr> classes;
      ProgramNode() : Node(Kind::PROGRAM) {}

      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Program\n";
        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        if (main_class)
        {
          main_class->print(next_tabs, classes.empty());
        }
        for (size_t i = 0; i < classes.size(); ++i)
        {
          bool child_is_last = (i == classes.size() - 1);
          classes[i]->print(next_tabs, child_is_last);
        }
      }
    };

    struct VarDeclNode : Node
    {
      NodePtr type;
      std::string name;
      VarDeclNode() : Node(Kind::VAR_DECL) {}
      VarDeclNode(NodePtr type, std::string name) : Node(Kind::VAR_DECL), type(std::move(type)), name(std::move(name)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Var[" << name << "]\n";
        if (type)
          type->print(tabs + (is_last ? "    " : "|   "), true);
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
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "ArrayAssign[" << name << "]\n";
        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        if (index)
          index->print(next_tabs, false);
        if (value)
          value->print(next_tabs, true);
      }
    };

    struct AssignNode : Node
    {
      std::string name;
      NodePtr value;
      AssignNode() : Node(Kind::ASSIGN) {}
      AssignNode(std::string name, NodePtr value) : Node(Kind::ASSIGN), name(std::move(name)), value(std::move(value)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Assign[" << name << "]\n";
        if (value)
          value->print(tabs + (is_last ? "    " : "│   "), true);
      }
    };

    struct BlockNode : Node
    {
      std::vector<NodePtr> stmt;
      BlockNode() : Node(Kind::BLOCK) {}
      BlockNode(std::vector<NodePtr> stmt) : Node(Kind::BLOCK), stmt(std::move(stmt)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Block\n";
        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        for (size_t i = 0; i < stmt.size(); ++i)
        {
          bool child_is_last = (i == stmt.size() - 1);
          stmt[i]->print(next_tabs, child_is_last);
        }
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
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "If\n";
        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        if (cond)
          cond->print(next_tabs, false);
        if (then_branch)
          then_branch->print(next_tabs, (else_branch == nullptr));
        if (else_branch)
          else_branch->print(next_tabs, true);
      }
    };

    struct PrintNode : Node
    {
      NodePtr expr;
      PrintNode() : Node(Kind::PRINT) {}
      PrintNode(NodePtr expr) : Node(Kind::PRINT), expr(std::move(expr)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Print\n";
        if (expr)
          expr->print(tabs + (is_last ? "    " : "│   "), true);
      }
    };

    struct WhileNode : Node
    {
      NodePtr cond;
      NodePtr body;
      WhileNode() : Node(Kind::WHILE) {}
      WhileNode(NodePtr cond, NodePtr body) : Node(Kind::WHILE), cond(std::move(cond)), body(std::move(body)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "WhileNode\n";
        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        if (cond)
          cond->print(next_tabs, false);
        if (body)
          body->print(next_tabs, true);
      }
    };

    // Expressões

    struct ArrayAccessNode : Node
    {
      NodePtr array;
      NodePtr index;
      ArrayAccessNode() : Node(Kind::ARRAY_ACCESS) {}
      ArrayAccessNode(NodePtr array, NodePtr index) : Node(Kind::ARRAY_ACCESS), array(std::move(array)), index(std::move(index)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "ArrayAccessNode\n";
        std::string next_tabs = tabs + (is_last ? "    " : "│   ");
        if (array)
          array->print(next_tabs, false);
        if (index)
          index->print(next_tabs, true);
      }
    };

    struct BinOpNode : Node
    {
      BinOp op;
      NodePtr left;
      NodePtr right;
      BinOpNode(BinOp op, NodePtr left, NodePtr right) : Node(Kind::BIN_OP), op(op), left(std::move(left)), right(std::move(right)) {}

      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::string op_str = (op == BinOp::ADD) ? "+" : (op == BinOp::SUB) ? "-"
                                                    : (op == BinOp::MULS)  ? "*"
                                                    : (op == BinOp::AND)   ? "&&"
                                                    : (op == BinOp::GT)    ? ">"
                                                                           : "?";

        std::cout << tabs << (is_last ? "└── " : "├── ") << "BinOp [" << op_str << "]\n";

        std::string next_tabs = tabs + (is_last ? "    " : "│   ");

        if (left)
          left->print(next_tabs, false);
        if (right)
          right->print(next_tabs, true);
      }
    };

    struct BoolNode : Node
    {
      bool value;
      explicit BoolNode(bool v) : Node(Kind::BOOL), value(v) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Bool[" << (value ? "true" : "false") << "]\n";
      }
    };

    struct IdentifierNode : Node
    {
      std::string name;
      explicit IdentifierNode(std::string n) : Node(Kind::IDENTIFIER), name(std::move(n)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Id[" << name << "]\n";
      }
    };

    struct LengthNode : Node
    {
      NodePtr obj;
      LengthNode() : Node(Kind::LENGTH) {}
      LengthNode(NodePtr obj) : Node(Kind::LENGTH), obj(std::move(obj)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Length\n";
        if (obj)
          obj->print(tabs + (is_last ? "    " : "│   "), true);
      }
    };

    struct MethodCallNode : Node
    {
      NodePtr obj;
      std::string method;
      std::vector<NodePtr> args;
      MethodCallNode() : Node(Kind::METHOD_CALL) {}
      MethodCallNode(NodePtr obj, std::string method, std::vector<NodePtr> args) : Node(Kind::METHOD_CALL), obj(std::move(obj)), method(std::move(method)), args(std::move(args)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "MethodCall[" << method << "]\n";
        std::string next_tabs = tabs + (is_last ? "    " : "│   ");

        if (obj)
          obj->print(next_tabs, args.empty());

        for (size_t i = 0; i < args.size(); ++i)
        {
          bool child_is_last = (i == args.size() - 1);
          args[i]->print(next_tabs, child_is_last);
        }
      }
    };

    struct NewArrayNode : Node
    {
      NodePtr size;
      NewArrayNode() : Node(Kind::NEW_ARRAY) {}
      NewArrayNode(NodePtr size) : Node(Kind::NEW_ARRAY), size(std::move(size)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "NewIntArray\n";
        if (size)
          size->print(tabs + (is_last ? "    " : "│   "), true);
      }
    };

    struct NewObjectNode : Node
    {
      std::string class_name;
      NewObjectNode() : Node(Kind::NEW_OBJECT) {}
      NewObjectNode(std::string class_name) : Node(Kind::NEW_OBJECT), class_name(std::move(class_name)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "NewObject[" << class_name << "]\n";
      }
    };

    struct NotNode : Node
    {
      NodePtr expr;
      NotNode() : Node(Kind::NOT) {}
      NotNode(NodePtr expr) : Node(Kind::NOT), expr(std::move(expr)) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Not\n";
        if (expr)
          expr->print(tabs + (is_last ? "    " : "│   "), true);
      }
    };

    struct NumberNode : Node
    {
      i32 value;
      NumberNode(i32 v) : Node(Kind::NUMBER), value(v) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "Number[" << value << "]\n";
      }
    };

    struct ThisNode : Node
    {
      ThisNode() : Node(Kind::THIS) {}
      void print(std::string tabs = "", bool is_last = true) const override
      {
        std::cout << tabs << (is_last ? "└── " : "├── ") << "This\n";
      }
    };

    using namespace jc::parser;
    class AST
    {
    private:
      NodePtr root;

      bool has_any_member(const std::vector<std::unique_ptr<PTree>> &children);

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

    public:
      NodePtr create(const PTree &root);
    };
  } // namespace ast
} // namespace jc