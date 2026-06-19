#include <iostream>
#include <fstream>
#include <sstream>

#include "types.hpp"
#include "preprocessor.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

using namespace jc;

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "usage: ./javinha input.ling output.txt\n";
    return EXIT_FAILURE;
  }

  char *input_file = argv[1];
  char *output_file = argv[2];

  std::ifstream f_stream(input_file);
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
    for (const auto &error : errors)
    {
      std::cerr << error;
    }
    return EXIT_FAILURE;
  }

  std::vector<Token> tokens = std::get<std::vector<Token>>(lexer_result);
  SymbolTable symbols = lexer->move_symbols();

  delete lexer;

  std::ofstream o_stream(output_file);

  if (!o_stream.is_open())
  {
    std::cerr << "error creating output file\n";
    return EXIT_FAILURE;
  }

  o_stream << "TOKENS:\n";
  for (const auto &t : tokens)
  {
    o_stream << "(" << jc::to_string(t.type) << ", " << t.value
             << ") [linha " << t.line << "]\n";
  }

  o_stream << "\nSYMBOL TABLE:\n";
  symbols.print(o_stream);

  o_stream.close();

  std::cout << "Analise léxica concluida com sucesso.\n";

  jc::parser::Parser parser(std::move(symbols));
  bool res = parser.earley_parse(std::move(tokens));
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

  return 0;
}