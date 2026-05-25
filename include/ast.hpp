#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "types.hpp"

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

      protected:
        explicit Node(Kind k) : kind(k) {}
    };

    struct TypeNode
    {
      TypeKind kind;
      std::string name;
    };

    // Estrutura do programa

    struct ClassNode : Node
    {
      std::string name;
      std::optional<std::string> parent;
      std::vector<NodePtr> fields;
      std::vector<NodePtr> methods;
      ClassNode() : Node(Kind::CLASS) {}
    };

    struct MainClassNode : Node
    {
      std::string name;
      std::string args_param;
      NodePtr body;
      MainClassNode() : Node(Kind::MAIN_CLASS) {}
    };

    struct MethodNode : Node
    {
      TypeNode return_type;
      std::string name;
      std::vector<ParamNode> params;
      std::vector<NodePtr> locals;
      NodePtr body;
      NodePtr return_expr;
      MethodNode() : Node(Kind::METHOD) {}
    };

    struct ParamNode : Node
    {
      TypeNode type;
      std::string name;
      ParamNode() : Node(Kind::PARAM) {}
    };

    struct ProgramNode : Node
    {
      NodePtr main_class;
      std::vector<NodePtr> classes;
      ProgramNode() : Node(Kind::PROGRAM) {}
    };

    struct VarDeclNode : Node
    {
      TypeNode type;
      std::string name;
      VarDeclNode() : Node(Kind::VAR_DECL) {}
    };

    // Comandos

    struct ArrayAssignNode : Node
    {
      std::string name;
      NodePtr index;
      NodePtr value;
      ArrayAssignNode() : Node(Kind::ARRAY_ASSIGN) {}
    };

    struct AssignNode : Node
    {
      std::string name;
      NodePtr value;
      AssignNode() : Node(Kind::ASSIGN) {}
    };

    struct BlockNode : Node
    {
      NodePtr stmt;
      BlockNode() : Node(Kind::BLOCK) {}
    };

    struct IfNode : Node
    {
      NodePtr cond;
      NodePtr then_branch;
      NodePtr else_branch;
      IfNode() : Node(Kind::IF) {}
    };

    struct PrintNode : Node
    {
      NodePtr expr;
      PrintNode() : Node(Kind::PRINT) {}
    };

    struct WhileNode : Node
    {
      NodePtr cond;
      NodePtr body;
      WhileNode() : Node(Kind::WHILE) {}
    };

    // Expressões

    struct ArrayAccessNode : Node
    {
      NodePtr array;
      NodePtr index;
      ArrayAccessNode() : Node(Kind::ARRAY_ACCESS) {}
    };

    struct BinOpNode : Node
    {
      BinOp op;
      NodePtr left;
      NodePtr right;
      BinOpNode() : Node(Kind::BIN_OP) {}
    };

    struct BoolNode : Node
    {
      bool value;
      explicit BoolNode(bool v) : Node(Kind::BOOL), value(v) {}
    };

    struct IdentifierNode : Node
    {
      std::string name;
      explicit IdentifierNode(std::string n) : Node(Kind::IDENTIFIER), name(std::move(n)) {}
    };

    struct LengthNode : Node
    {
      NodePtr obj;
      LengthNode() : Node(Kind::LENGTH) {}
    };

    struct MethodCallNode : Node
    {
      NodePtr obj;
      std::string method;
      std::vector<NodePtr> args;
      MethodCallNode() : Node(Kind::METHOD_CALL) {}
    };

    struct NewArrayNode : Node
    {
      NodePtr size;
      NewArrayNode() : Node(Kind::NEW_ARRAY) {}
    };

    struct NewObjectNode : Node
    {
      std::string class_name;
      NewObjectNode() : Node(Kind::NEW_OBJECT) {}
    };

    struct NotNode : Node
    {
      NodePtr expr;
      NotNode() : Node(Kind::NOT) {}
    };

    struct NumberNode : Node
    {
      i32 value;
      explicit NumberNode(i32 v) : Node(Kind::NUMBER), value(v) {}
    };

    struct ThisNode : Node
    {
      ThisNode() : Node(Kind::THIS) {}
    };
  } // namespace ast
} // namespace jc
