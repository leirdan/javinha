#pragma once
#include <vector>
#include <string>
#include <variant>
#include "token.hpp"
#include "symbol_table.hpp"
#include "types.hpp"

namespace jc
{
  /**
   * \brief Lexical analyzer responsible for tokenizing the source code.
   *
   * The Lexer is responsible for scanning the input string and converting it
   * into a sequence of tokens to be consumed by later compiler phases.
   * It also maintains a symbol table for identifiers encountered during
   * the tokenization process.
   */
  class Lexer
  {
  private:
    std::string input;
    u32 pos = 0;
    u32 line = 1;
    u32 col = 1;

    /**
     * \brief Returns the current character without consuming it.
     *
     * @return Current character in the input stream.
     */
    char peek() const;

    /**
     * \brief Consumes the current character and advances the position.
     *
     * Also updates line and column counters accordingly.
     *
     * @return The consumed character.
     */
    char advance();

    /**
     * \brief Skips whitespace characters in the input.
     *
     * This includes spaces, tabs, and newline characters,
     * updating position and line/column tracking.
     */
    void skip_whitespace();

    /**
     * \brief Parses an identifier or keyword token.
     *
     * Reads alphanumeric sequences and determines whether
     * the resulting lexeme is a keyword or a user-defined identifier.
     *
     * @return Token representing an identifier or keyword.
     */
    Token identifier();

    /**
     * \brief Parses a numeric literal token.
     *
     * Reads a sequence of digits and constructs a numeric token.
     *
     * @return Token representing a numeric literal.
     */
    Token number();

    /**
     * \brief Parses operators and delimiters.
     *
     * Handles single and multi-character operators as well as
     * language delimiters such as braces, parentheses, etc.
     *
     * @return Token representing an operator or delimiter.
     */
    Token op_or_delim();

    /**
     * \brief Checks whether a given lexeme is a keyword.
     *
     * @param lexeme String to be checked.
     * @return True if the lexeme is a reserved keyword, false otherwise.
     */
    bool is_keyword(const std::string &) const;

  public:
    /**
     * \brief Constructs a Lexer instance.
     *
     * @param input Source code string to be tokenized.
     */
    Lexer(std::string input);

    ~Lexer() = default;

    /**
     * \brief Tokenizes the entire input source code.
     *
     * Iterates through the input and generates a sequence
     * of tokens representing the program.
     *
     * @return Vector containing all tokens extracted from the input.
     */
    std::variant<std::vector<Token>, std::vector<std::string>> tokenize();
  };

} // namespace jc