#include <iostream>
#include <fstream>
#include <sstream>

#include "types.hpp"
#include "preprocessor.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include "config.hpp"

using namespace jc;

int main(int argc, char *argv[])
{
  CompilerConfig config;
  if (!config.parse(argc, argv))
  {
    std::cerr << "usage: ./javinha input.ling output.txt [--flag]\n";
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

    if (config.firstLexicalError)
    {
      std::cerr << errors.front();
      delete lexer;
      return EXIT_FAILURE;
    }

    for (const auto &error : errors)
    {
      std::cerr << error;
    }
    delete lexer;
    return EXIT_FAILURE;
  }

  std::vector<Token> tokens = std::get<std::vector<Token>>(lexer_result);

  delete lexer;

  if (config.printTokens)
  {
    std::cout << "TOKENS:\n";
    for (const auto &t : tokens)
    {
      std::cout << "(" << jc::to_string(t.type) << ", " << t.value
                << ") [linha " << t.line << "]\n";
    }
    return EXIT_SUCCESS;
  }

  jc::parser::Parser parser;
  bool res = parser.earley_parse(std::move(tokens), config.printAst);
  if (res && !parser.has_errors())
  {
    std::cout << "Programa sintaticamente válido. \n";
  }
  else if (parser.has_errors())
  {
    std::cout << "Programa sintaticamente incorreto. \n";
  }

  if (parser.has_errors())
  {
    std::cout << "\n===== ERROS SINTÁTICOS ENCONTRADOS =====\n";
    for (const auto &error : parser.get_errors())
    {
      std::cout << error.to_string() << "\n";
    }
    std::cout << "=========================================\n";
  }

  if (config.printSymbolTable)
  {
    // std::cout << "\nSYMBOL TABLE:\n";
    // for (const auto &[k, v] : parser.get_all())
    // {
    //   std::cout << v.name
    //             << " | categoria: " << v.category
    //             << " | tipo: " << v.type
    //             << " | linha: " << v.line << "\n";
    // }
  }

  return 0;
}