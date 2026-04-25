#include "lexer.hpp"
#include "types.hpp"
#include <cctype>
#include <unordered_map>
#include <iostream>

namespace jc {

// List of keywords (NOT DEFINITIVE)
static std::unordered_map<std::string, TokenType> keywords = {
    {"class", TokenType::KEYWORD},
    {"public", TokenType::KEYWORD},
    {"static", TokenType::KEYWORD},
    {"void", TokenType::KEYWORD},
    {"int", TokenType::KEYWORD},
    {"string", TokenType::KEYWORD},
    {"if", TokenType::KEYWORD},
    {"else", TokenType::KEYWORD},
    {"while", TokenType::KEYWORD},
    {"for", TokenType::KEYWORD},
    {"return", TokenType::KEYWORD},
    {"new", TokenType::KEYWORD},
    {"this", TokenType::KEYWORD}
};

Lexer::Lexer(std::string input) : input(std::move(input)) {}

char Lexer::peek() const {
  if (pos >= input.size()) return '\0';
  return input[pos];
}

char Lexer::advance() {
  if (pos >= input.size()) return '\0';

  char c = input[pos++];
  if (c == '\n') {
    line++;
    col = 1;
  } else {
    col++;
  }
  return c;
}

void Lexer::skip_whitespace() {
  while (std::isspace(peek())) {
    advance();
  }
}

bool Lexer::is_keyword(const std::string& str) const {
  return keywords.find(str) != keywords.end();
}

std::string suggest(const std::string& wrong,
                    const SymbolTable& symbols) {
  for (const auto& [k, v] : symbols.get_all()) {
    if (!k.empty() && !wrong.empty() && k[0] == wrong[0]) {
      return k;
    }
  }
  return "";
}

// Identifier: [a-zA-Z_][a-zA-Z0-9_]*
Token Lexer::identifier() {
  u32 start = pos;
  u32 start_col = col;

  while (std::isalnum(peek()) || peek() == '_') {
    advance();
  }

  std::string text = input.substr(start, pos - start);

  if (is_keyword(text)) {
    return {TokenType::KEYWORD, text, line, start_col};
  }

  symbols.insert(text, "IDENTIFIER", line);
  return {TokenType::IDENTIFIER, text, line, start_col};
}

// Number: [0-9]+(\.[0-9]+)?
Token Lexer::number() {
  u32 start = pos;
  u32 start_col = col;

  while (std::isdigit(peek())) advance();

  if (peek() == '.') {
    advance();
    while (std::isdigit(peek())) advance();
  }

  return {
    TokenType::NUMBER,
    input.substr(start, pos - start),
    line,
    start_col
  };
}

// String: "([^"\\]|\\.)*"
Token Lexer::string() {
  u32 start_col = col;
  advance(); // consume "

  u32 start = pos;

  while (peek() != '"' && peek() != '\0') {
    if (peek() == '\\') advance();
    advance();
  }

  std::string value = input.substr(start, pos - start);

  if (peek() == '"') advance();

  return {TokenType::STRING, value, line, start_col};
}

// Operator or Delimiter
Token Lexer::op_or_delim() {
  u32 start_col = col;
  char c = advance();
  std::string lex(1, c);

  if ((c == '=' && peek() == '=') ||
      (c == '!' && peek() == '=') ||
      (c == '<' && peek() == '=') ||
      (c == '>' && peek() == '=') ||
      (c == '+' && peek() == '+') ||
      (c == '-' && peek() == '-') ||
      (c == '+' && peek() == '=') ||
      (c == '-' && peek() == '=')) {
    lex += advance();
    return {TokenType::OPERATOR, lex, line, start_col};
  }

  // delimitadores
  if (c == ';' || c == ',' || c == '(' || c == ')' ||
      c == '{' || c == '}' || c == '[' || c == ']') {
    return {TokenType::DELIMITER, lex, line, start_col};
  }

  return {TokenType::OPERATOR, lex, line, start_col};
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;

  while (pos < input.size()) {
    skip_whitespace();

    char c = peek();
    if (c == '\0') break;

    if (std::isalpha(c) || c == '_') {
      tokens.push_back(identifier());
    }
    else if (std::isdigit(c)) {
      u32 look = pos;
      while (std::isdigit(input[look])) look++;

      if (std::isalpha(input[look]) || input[look] == '_') {
        std::cerr << "Erro lexico: identificador invalido na linha "
                  << line << "\n";
      }

      tokens.push_back(number());
    }
    else if (c == '"') {
      tokens.push_back(string());
    }
    else if (
        c == '+' || c == '-' || c == '*' || c == '/' ||
        c == '=' || c == '!' || c == '<' || c == '>' ||
        c == ';' || c == ',' ||
        c == '(' || c == ')' ||
        c == '{' || c == '}' ||
        c == '[' || c == ']'
    ) {
      tokens.push_back(op_or_delim());
    }
    else {
      std::cerr << "Erro lexico: caractere invalido '"
                << c << "' na linha " << line
                << ", coluna " << col << "\n";
      advance();
    }
  }

  tokens.push_back({TokenType::END_OF_FILE, "", line, col});
  return tokens;
}

SymbolTable&& Lexer::move_symbols() {
  return std::move(symbols);
}

}