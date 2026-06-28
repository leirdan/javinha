#pragma once
#include "ast.hpp"
#include "symbol_table.hpp"
#include <format>
#include <vector>
#include <string>
#include <optional>

namespace jc::semantic {

  struct SemanticError {
    u32 line;
    std::string message;

    std::string to_string() const {
      return std::format("[ERRO SEMÂNTICO] - Linha {} - {}", line, message);
    }
  };

  class SemanticAnalyzer {
  private:
    SymbolTable &table;
    std::vector<SemanticError> errors;

    std::string current_class;
    std::string current_method;

    void analyze_node(ast::Node &node);

    std::optional<std::string> infer_type(const ast::Node &node);

    void check_identifier(const ast::IdentifierNode &node);
    void check_method_call(const ast::MethodCallNode &node);
    void check_assign(const ast::AssignNode &node);
    void check_array_assign(const ast::ArrayAssignNode &node);
    void check_new_object(const ast::NewObjectNode &node);
    void check_bin_op(const ast::BinOpNode &node);
    void check_not(const ast::NotNode &node);
    void check_if(const ast::IfNode &node);
    void check_while(const ast::WhileNode &node);
    void check_print(const ast::PrintNode &node);
    void check_array_access(const ast::ArrayAccessNode &node);
    void check_length(const ast::LengthNode &node);
    void check_return(const ast::MethodNode &method_node, const ast::Node &return_expr);

    void add_error(u32 line, const std::string &msg) {
      errors.push_back({line, msg});
    }

    bool types_compatible(const std::string &actual, const std::string &expected);

  public:
    explicit SemanticAnalyzer(SymbolTable &table) : table(table) {}

    void analyze(const ast::NodePtr &root);

    bool has_errors() const { return !errors.empty(); }
    const std::vector<SemanticError> &get_errors() const { return errors; }
  };

}