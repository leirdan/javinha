#pragma once
#include "lexer.hpp"
#include <optional>
#include <functional>
#include "grammar.hpp"
#include <unordered_set>

namespace jc
{
  using namespace grammar;

  namespace parser
  {
    struct State
    {
      const GProduction *rhs;
      u64 start; // input position where this state started
      NT lhs;
      u8 dot; // position of the dot in rhs

      State(NT lhs, const GProduction *rhs, u8 dot, u64 start) : rhs(rhs), start(start), lhs(lhs), dot(dot) {}

      bool is_complete() const;

      const std::optional<GSymbol> next_symbol() const;

      bool operator==(const State &other) const
      {
        return lhs == other.lhs &&
               dot == other.dot &&
               start == other.start &&
               *rhs == *other.rhs;
      }

      const std::string to_string() const
      {
        std::string sb = symbol_to_string(lhs) + " -> ";
        for (u8 i = 0; i < rhs->size; i++)
        {
          if (i == dot)
            sb += "• ";
          GSymbol aux = rhs->data.at(i);
          sb += aux.to_string() + ' ';
        }
        if (dot == rhs->size)
          sb += "•";
        sb += " [" + std::to_string(start) + "]";
        return sb;
      }
    };

    using StateSet = std::unordered_set<State>;

    class Parser
    {
      SymbolTable symbols;

    public:
      // Parser(std::vector<Token> &&tokens, const SymbolTable &symbols) : tokens(std::move(tokens)), symbols(symbol_table) {};
      Parser(const SymbolTable &&symbols) : symbols(symbols) {
                                            };

      bool earley_parse(const std::vector<Token> &&tokens);
    };
  }
}

namespace std
{
  template <>
  struct hash<jc::parser::State>
  {
    size_t operator()(const jc::parser::State &s) const noexcept
    {
      size_t h = std::hash<int>{}(static_cast<int>(s.lhs));
      h ^= std::hash<jc::u8>{}(s.dot) + 0x9e3779b9 + (h << 6) + (h >> 2);
      h ^= std::hash<jc::u64>{}(s.start) + 0x9e3779b9 + (h << 6) + (h >> 2);
      h ^= std::hash<const jc::grammar::GProduction *>{}(s.rhs) + 0x9e3779b9 + (h << 6) + (h >> 2);
      return h;
    }
  };
}