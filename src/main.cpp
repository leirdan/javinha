#include <iostream>
#include <fstream>
#include <sstream>

#include "types.hpp"
#include "preprocessor.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "semantic.hpp"
#include "3AC.hpp"

using namespace jc;

int main(int argc, char *argv[])
{
  if (!config.parse(argc, argv))
  {
    std::cerr << "usage: ./javinha input.ling output.txt [--flags]\n";
    return EXIT_FAILURE;
  }

  std::ifstream f_stream(config.inputFile);
  if (!f_stream.is_open())
  {
    std::cerr << "file not found\n";
    return EXIT_FAILURE;
  }

  std::stringstream stream;
  stream << f_stream.rdbuf();
  std::string program_buff = stream.str();
  f_stream.close();

  std::string buffer1 = pre::remove_comments(std::move(program_buff));
  std::string clean_code = pre::minify(std::move(buffer1));

  Lexer *lexer = new Lexer(clean_code);

  auto lexer_result = lexer->tokenize();

  if (std::holds_alternative<std::vector<std::string>>(lexer_result))
  {
    auto &errors = std::get<std::vector<std::string>>(lexer_result);
    log::lexer_errors(errors);
    return EXIT_FAILURE;
  }

  std::vector<Token> tokens = std::get<std::vector<Token>>(lexer_result);

  delete lexer;
  log::tokens(tokens);

  auto *parser = new parser::Parser();
  bool res = parser->earley_parse(std::move(tokens));

  if (parser->has_errors())
  {
    std::cout << "Programa sintaticamente inválido!\n";
    log::parser_errors(parser->get_errors());
    return EXIT_FAILURE;
  }

  ast::NodePtr ast = parser->release_ast();
  SymbolTable table = parser->release_symbol_table();
  delete parser;

  semantic::SemanticAnalyzer analyzer(table);
  analyzer.analyze(ast);

  if (analyzer.has_errors())
  {
    std::cout << "Erros semânticos encontrados:\n";
    for (const auto &e : analyzer.get_errors())
      std::cout << e.to_string() << "\n";
    return EXIT_FAILURE;
  }

  log::ast(ast);
  log::symbol_table(table);

  backend::TACGenerator generator;
  auto [_, tac_code] = ast->generate_tac(generator, table);
  log::tac_code(tac_code);

  return 0;
}