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
      virtual void print() = 0;

    protected:
      explicit Node(Kind k) : kind(k) {}
    };

    using NodePtr = Node *;

    struct TypeNode : Node
    {
      TypeKind kind;
      std::string name;
      TypeNode() : Node(Kind::TYPE) {}
      TypeNode(TypeKind tk) : Node(Kind::TYPE) { kind = tk; }
      TypeNode(TypeKind tk, std::string name) : Node(Kind::TYPE)
      {
        kind = tk;
        name = name;
      }
      void print() override {}
    };

    // Estrutura do programa

    struct ClassNode : Node
    {
      std::string name;
      std::optional<std::string> parent;
      std::vector<NodePtr> fields;
      std::vector<NodePtr> methods;
      ClassNode() : Node(Kind::CLASS) {}
      void print() override
      {
        std::cout << std::format("\t[ClassNode {} | Parent: {}]\n", name, parent.value_or("None"));
        // std::cout << std::format("\t[ClassNode Fields]\n");
        for (auto &field : fields)
        {
          if (field)
            field->print();
        }
        for (auto &method : methods)
        {
          if (method)
            method->print();
        }
      }
    };

    struct MainClassNode : Node
    {
      std::string name;
      std::string args_param;
      NodePtr body;
      MainClassNode() : Node(Kind::MAIN_CLASS) {}
      void print() override
      {
        std::cout << std::format("\t[MainClassNode {}]\n", name);
        body->print();
      }
    };

    struct ParamNode : Node
    {
      TypeNode type;
      std::string name;
      ParamNode() : Node(Kind::PARAM) {}
      ParamNode(NodePtr type, std::string &&name) : Node(Kind::PARAM)
      {
        type = static_cast<TypeNode *>(type);
        name = name;
      }
      void print() override
      {
        std::cout << std::format("[ParamNode {} | Type: {}]\n", name, type.name);
      }
    };

    struct MethodNode : Node
    {
      TypeNode return_type;
      std::string name;
      std::vector<ParamNode *> params;
      std::vector<NodePtr> locals;
      NodePtr body;
      NodePtr return_expr;
      MethodNode() : Node(Kind::METHOD) {}
      MethodNode(NodePtr r, std::string n, std::vector<ParamNode*> &&p, std::vector<NodePtr> &&l, NodePtr b, NodePtr re) : Node(Kind::METHOD)
      {
        return_type = *(static_cast<TypeNode *>(r));
        name = n;
        params = p;
        locals = l;
        body = b;
        return_expr = re;
      }
      void print() override
      {
        // TODO
      }
    };

    struct ProgramNode : Node
    {
      NodePtr main_class;
      std::vector<NodePtr> classes;
      ProgramNode() : Node(Kind::PROGRAM) {}
      void print() override
      {
        std::cout << "[ProgramNode]\n";
        main_class->print();
        for (auto &cl : classes)
        {
          cl->print();
        }
      }
    };

    struct VarDeclNode : Node
    {
      TypeNode type;
      std::string name;
      VarDeclNode() : Node(Kind::VAR_DECL) {}
      VarDeclNode(TypeNode type, std::string name) : Node(Kind::VAR_DECL)
      {
        type = type;
        name = name;
      }
      void print() override
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
      ArrayAssignNode(std::string name, NodePtr index, NodePtr value) : Node(Kind::ARRAY_ASSIGN), name(name), index(index), value(value) {}
      void print() override
      {
        // TODO
      }
    };

    struct AssignNode : Node
    {
      std::string name;
      NodePtr value;
      AssignNode() : Node(Kind::ASSIGN) {}
      AssignNode(std::string name, NodePtr value) : Node(Kind::ASSIGN), name(name), value(value) {}
      void print() override {}
    };

    struct BlockNode : Node
    {
      std::vector<NodePtr> stmt;
      BlockNode() : Node(Kind::BLOCK) {}
      BlockNode(std::vector<NodePtr> &&stmt) : Node(Kind::BLOCK), stmt(stmt) {}
      void print() override
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
      IfNode(NodePtr cond, NodePtr then, NodePtr else_b) : Node(Kind::IF), cond(cond)
      {
        then_branch = then;
        else_branch = else_b;
      }
      void print() override
      {
        // TODO
      }
    };

    struct PrintNode : Node
    {
      NodePtr expr;
      PrintNode() : Node(Kind::PRINT) {}
      PrintNode(NodePtr expr) : Node(Kind::PRINT), expr(expr) {}
      void print() override
      {
        // TODO
      }
    };

    struct WhileNode : Node
    {
      NodePtr cond;
      NodePtr body;
      WhileNode() : Node(Kind::WHILE) {}
      WhileNode(NodePtr cond, NodePtr body) : Node(Kind::WHILE), cond(cond), body(body) {}
      void print() override
      {
        // TODO
      }
    };

    // Expressões

    struct ArrayAccessNode : Node
    {
      NodePtr array;
      NodePtr index;
      ArrayAccessNode() : Node(Kind::ARRAY_ACCESS) {}
      ArrayAccessNode(NodePtr array, NodePtr index) : Node(Kind::ARRAY_ACCESS), array(array), index(index) {}
      void print() override {}
    };

    struct BinOpNode : Node
    {
      BinOp op;
      NodePtr left;
      NodePtr right;
      BinOpNode() : Node(Kind::BIN_OP) {}
      BinOpNode(BinOp op, NodePtr left, NodePtr right) : Node(Kind::BIN_OP), left(left), right(right), op(op) {}
      void print() override {}
    };

    struct BoolNode : Node
    {
      bool value;
      explicit BoolNode(bool v) : Node(Kind::BOOL), value(v) {}
      void print() override {}
    };

    struct IdentifierNode : Node
    {
      std::string name;
      explicit IdentifierNode(std::string n) : Node(Kind::IDENTIFIER), name(std::move(n)) {}
      void print() override {}
    };

    struct LengthNode : Node
    {
      NodePtr obj;
      LengthNode() : Node(Kind::LENGTH) {}
      LengthNode(NodePtr obj) : Node(Kind::LENGTH), obj(obj) {}
      void print() override {}
    };

    struct MethodCallNode : Node
    {
      NodePtr obj;
      std::string method;
      std::vector<NodePtr> args;
      MethodCallNode() : Node(Kind::METHOD_CALL) {}
      MethodCallNode(NodePtr obj, std::string method, std::vector<NodePtr> &&args) : Node(Kind::METHOD_CALL), obj(obj), method(method), args(args) {}
      void print() override {}
    };

    struct NewArrayNode : Node
    {
      NodePtr size;
      NewArrayNode() : Node(Kind::NEW_ARRAY) {}
      NewArrayNode(NodePtr size) : Node(Kind::NEW_ARRAY), size(size) {}
      void print() override {}
    };

    struct NewObjectNode : Node
    {
      std::string class_name;
      NewObjectNode() : Node(Kind::NEW_OBJECT) {}
      NewObjectNode(std::string class_name) : Node(Kind::NEW_OBJECT), class_name(class_name) {}
      void print() override {}
    };

    struct NotNode : Node
    {
      NodePtr expr;
      NotNode() : Node(Kind::NOT) {}
      NotNode(NodePtr expr) : Node(Kind::NOT), expr(expr) {}
      void print() override {}
    };

    struct NumberNode : Node
    {
      i32 value;
      NumberNode(i32 v) : Node(Kind::NUMBER), value(v) {}
      void print() override {}
    };

    struct ThisNode : Node
    {
      ThisNode() : Node(Kind::THIS) {}
      void print() override {}
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
