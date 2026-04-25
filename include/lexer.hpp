#pragma once
#include <vector>
#include <string>
#include "token.hpp"
#include "symbol_table.hpp"
#include "types.hpp"

namespace jc {
class Lexer {
private:
  std::string input;
  u32 pos = 0;
  u32 line = 1;
  u32 col = 1;

  SymbolTable symbols;

  char peek() const;
  char advance();
  void skip_whitespace();

  Token identifier();
  Token number();
  Token string();
  Token op_or_delim(); // operator or delimiter (didn't know how to name it)

  bool is_keyword(const std::string&) const;

public:
  Lexer(std::string input);

  std::vector<Token> tokenize();
  SymbolTable&& move_symbols();
};

}