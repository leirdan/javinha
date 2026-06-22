#pragma once
#include "ast.hpp"
#include "symbol_table.hpp"
#include <format>
#include <vector>
#include <string>

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

    void analyze_node(ast::Node &node);

    void check_identifier(const ast::IdentifierNode &node);
    void check_method_call(const ast::MethodCallNode &node);
    void check_assign(const ast::AssignNode &node);
    void check_array_assign(const ast::ArrayAssignNode &node);
    void check_new_object(const ast::NewObjectNode &node);

    void add_error(u32 line, const std::string &msg) {
      errors.push_back({line, msg});
    }

  public:
    explicit SemanticAnalyzer(SymbolTable &table) : table(table) {}

    void analyze(const ast::NodePtr &root);

    bool has_errors() const { return !errors.empty(); }
    const std::vector<SemanticError> &get_errors() const { return errors; }
  };

}