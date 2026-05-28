#pragma once
#include <vector>
#include <array>
#include <string>
#include "types.hpp"

namespace jc
{
  namespace grammar
  {
    enum class SymbolType : u8
    {
      TERMINAL,
      NON_TERMINAL,
      LAMBDA
    };

    enum class NT : u8
    {
      START = 1,
      PROG,
      MAINC,
      DEFCL,
      DEFVAR,
      DEFMET,
      TYPE,
      ARGS,
      LISTCMD,
      CMD,
      EXP,
      EXP2,
      GREATER,
      GREATER2,
      ADD,
      ADD2,
      MUL,
      MUL2,
      NEG,
      OBJ,
      OBJMET,
      OBJATOM,
      LISTEXP,
    };

    enum class T : u8
    {
      CLASS = 1,
      PUBLIC,
      STATIC,
      VOID,
      MAIN,
      STRING,
      IF,
      ELSE,
      WHILE,
      SYSTEM,
      OUT,
      PRINTLN,
      RETURN,
      EXTENDS,
      INT,
      BOOLEAN,
      TRUE,
      FALSE,
      THIS,
      NEW,
      PLUS,
      MINUS,
      MULT,
      GT,
      AND,
      NOT,
      ASSIGN,
      LPAR,
      RPAR,
      LBRACE,
      RBRACE,
      LBRACKET,
      RBRACKET,
      DOT,
      COMMA,
      SEMICOLON,
      LENGTH,
      NUMBER,
      ID,
      END
    };

    const static std::string type_to_string(SymbolType t);
    const static std::string symbol_to_string(NT nt);
    const static std::string symbol_to_string(T t);

    struct GSymbol
    {
      SymbolType type;
      u8 value;

      constexpr static GSymbol T(T s)
      {
        return GSymbol{.type = SymbolType::TERMINAL, .value = (u8)s};
      }
      constexpr static GSymbol N(NT s)
      {
        return GSymbol{.type = SymbolType::NON_TERMINAL, .value = (u8)s};
      }
      constexpr static GSymbol L()
      {
        return GSymbol{.type = SymbolType::LAMBDA, .value = 0};
      }

      bool operator==(const GSymbol &other) const
      {
        return type == other.type && value == other.value;
      }
      bool operator!=(const GSymbol &other) const
      {
        return !(*this == other);
      }

      [[nodiscard]]
      std::string to_string() const
      {
        switch (type)
        {
        case SymbolType::TERMINAL:
          return symbol_to_string(static_cast<jc::grammar::T>(value));

        case SymbolType::NON_TERMINAL:
          return symbol_to_string(static_cast<jc::grammar::NT>(value));

        case SymbolType::LAMBDA:
          return "λ";
        }

        return "?";
      }
    };

    struct GProduction
    {
      std::array<GSymbol, 24> data{};
      u8 size = 0;

      constexpr GProduction(std::initializer_list<GSymbol> init)
      {
        if (init.size() < data.size())
        {
          u8 i = 0;
          for (auto &s : init)
          {
            data[i++] = s;
          }
          size = i;
        }
      }

      [[nodiscard]]
      std::string to_string() const
      {
        std::string out;
        for (u8 i = 0; i < size; i++)
        {
          out += data[i].to_string();
          if (i + 1 < size)
            out += " ";
        }

        if (size == 0)
          out = "λ";

        return out;
      }

      inline bool operator==(const GProduction &other) const
      {
        if (size != other.size)
          return false;

        for (u8 i = 0; i < size; i++)
          if (data[i] != other.data[i])
            return false;

        return true;
      }
    };

    struct GRule
    {
      std::vector<GProduction> rhs;
      NT lhs;

      const std::string to_string()
      {
        std::string out = symbol_to_string(lhs) + " -> ";
        for (size_t i = 0; i < rhs.size(); i++)
        {
          out += rhs[i].to_string();
          if (i + 1 < rhs.size())
            out += " | ";
        }
        return out;
      }
    };

    const static NT start_symbol = NT::START;
    const u8 rules = 28;
    using Grammar = std::array<GRule, rules>;

    const static Grammar grammar = {{GRule{.rhs = {GProduction{GSymbol::N(NT::PROG), GSymbol::T(T::END)}}, .lhs = NT::START},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::MAINC), GSymbol::N(NT::DEFCL)}}, .lhs = NT::PROG},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::CLASS), GSymbol::T(T::ID), GSymbol::T(T::LBRACE), GSymbol::T(T::PUBLIC), GSymbol::T(T::STATIC), GSymbol::T(T::VOID), GSymbol::T(T::MAIN), GSymbol::T(T::LPAR), GSymbol::T(T::STRING), GSymbol::T(T::LBRACKET), GSymbol::T(T::RBRACKET), GSymbol::T(T::ID), GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::LISTCMD), GSymbol::T(T::RBRACE), GSymbol::T(T::RBRACE)}}, .lhs = NT::MAINC},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::CLASS), GSymbol::T(T::ID), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFVAR), GSymbol::N(NT::DEFMET), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFCL)}, GProduction{GSymbol::T(T::CLASS), GSymbol::T(T::ID), GSymbol::T(T::EXTENDS), GSymbol::T(T::ID), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFVAR), GSymbol::N(NT::DEFMET), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFCL)}, GProduction{GSymbol::L()}}, .lhs = NT::DEFCL},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::TYPE), GSymbol::T(T::ID), GSymbol::T(T::SEMICOLON), GSymbol::N(NT::DEFVAR)}, GProduction{GSymbol::L()}}, .lhs = NT::DEFVAR},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::PUBLIC), GSymbol::N(NT::TYPE), GSymbol::T(T::ID), GSymbol::T(T::LPAR), GSymbol::N(NT::ARGS), GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFVAR), GSymbol::N(NT::LISTCMD), GSymbol::T(T::RETURN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFMET)}, GProduction{GSymbol::T(T::PUBLIC), GSymbol::N(NT::TYPE), GSymbol::T(T::ID), GSymbol::T(T::LPAR), GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFVAR), GSymbol::N(NT::LISTCMD), GSymbol::T(T::RETURN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFMET)}, GProduction{GSymbol::L()}}, .lhs = NT::DEFMET},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::INT), GSymbol::T(T::LBRACKET), GSymbol::T(T::RBRACKET)}, GProduction{GSymbol::T(T::BOOLEAN)}, GProduction{GSymbol::T(T::INT)}, GProduction{GSymbol::T(T::ID)}}, .lhs = NT::TYPE},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::TYPE), GSymbol::T(T::ID)}, GProduction{GSymbol::N(NT::TYPE), GSymbol::T(T::ID), GSymbol::T(T::COMMA), GSymbol::N(NT::ARGS)}}, .lhs = NT::ARGS},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::CMD), GSymbol::N(NT::LISTCMD)}, GProduction{GSymbol::L()}}, .lhs = NT::LISTCMD},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::LBRACE), GSymbol::N(NT::LISTCMD), GSymbol::T(T::RBRACE)}, GProduction{GSymbol::T(T::IF), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::N(NT::CMD), GSymbol::T(T::ELSE), GSymbol::N(NT::CMD)}, GProduction{GSymbol::T(T::WHILE), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::N(NT::CMD)}, GProduction{GSymbol::T(T::SYSTEM), GSymbol::T(T::DOT), GSymbol::T(T::OUT), GSymbol::T(T::DOT), GSymbol::T(T::PRINTLN), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::T(T::SEMICOLON)}, GProduction{GSymbol::T(T::ID), GSymbol::T(T::ASSIGN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON)}, GProduction{GSymbol::T(T::ID), GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET), GSymbol::T(T::ASSIGN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON)}}, .lhs = NT::CMD},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::GREATER), GSymbol::N(NT::EXP2)}}, .lhs = NT::EXP},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::AND), GSymbol::N(NT::GREATER), GSymbol::N(NT::EXP2)}, GProduction{GSymbol::L()}}, .lhs = NT::EXP2},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::ADD), GSymbol::N(NT::GREATER2)}}, .lhs = NT::GREATER},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::GT), GSymbol::N(NT::ADD), GSymbol::N(NT::GREATER2)}, GProduction{GSymbol::L()}}, .lhs = NT::GREATER2},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::MUL), GSymbol::N(NT::ADD2)}}, .lhs = NT::ADD},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::PLUS), GSymbol::N(NT::MUL), GSymbol::N(NT::ADD2)}, GProduction{GSymbol::T(T::MINUS), GSymbol::N(NT::MUL), GSymbol::N(NT::ADD2)}, GProduction{GSymbol::L()}}, .lhs = NT::ADD2},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::NEG), GSymbol::N(NT::MUL2)}}, .lhs = NT::MUL},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::MULT), GSymbol::N(NT::NEG), GSymbol::N(NT::MUL2)}, GProduction{GSymbol::L()}}, .lhs = NT::MUL2},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::NOT), GSymbol::N(NT::NEG)}, GProduction{GSymbol::N(NT::OBJ)}}, .lhs = NT::NEG},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::OBJATOM), GSymbol::N(NT::OBJMET)}}, .lhs = NT::OBJ},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::DOT), GSymbol::T(T::LENGTH), GSymbol::N(NT::OBJMET)}, GProduction{GSymbol::T(T::DOT), GSymbol::T(T::ID), GSymbol::T(T::LPAR), GSymbol::N(NT::LISTEXP), GSymbol::T(T::RPAR), GSymbol::N(NT::OBJMET)}, GProduction{GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET), GSymbol::N(NT::OBJMET)}, GProduction{GSymbol::L()}}, .lhs = NT::OBJMET},

                                     GRule{.rhs = {GProduction{GSymbol::T(T::NEW), GSymbol::T(T::INT), GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET)}, GProduction{GSymbol::T(T::NEW), GSymbol::T(T::ID), GSymbol::T(T::LPAR), GSymbol::T(T::RPAR)}, GProduction{GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR)}, GProduction{GSymbol::T(T::TRUE)}, GProduction{GSymbol::T(T::FALSE)}, GProduction{GSymbol::T(T::ID)}, GProduction{GSymbol::T(T::NUMBER)}, GProduction{GSymbol::T(T::THIS)}}, .lhs = NT::OBJATOM},

                                     GRule{.rhs = {GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::COMMA), GSymbol::N(NT::LISTEXP)}, GProduction{GSymbol::N(NT::EXP)}, GProduction{GSymbol::L()}}, .lhs = NT::LISTEXP}}};

    static i8 contains_key(const GSymbol &sym)
    {
      if (sym.type != SymbolType::NON_TERMINAL)
        return -1;

      for (u8 i = 0; i < rules; i++)
      {
        if ((u8)grammar::grammar.at(i).lhs == sym.value)
        {
          return i;
        }
      }
      return -1;
    }

    const static std::string type_to_string(SymbolType t)
    {
      switch (t)
      {
      case SymbolType::LAMBDA:
        return "LAMBDA";
      case SymbolType::NON_TERMINAL:
        return "NON TERMINAL";
      case SymbolType::TERMINAL:
        return "TERMINAL";
      }
      return "UNKNOWN";
    }

    const static std::string symbol_to_string(NT nt)
    {
      switch (nt)
      {
      case NT::START:
        return "START";
      case NT::PROG:
        return "PROG";
      case NT::MAINC:
        return "MAINC";
      case NT::DEFCL:
        return "DEFCL";
      case NT::DEFVAR:
        return "DEFVAR";
      case NT::DEFMET:
        return "DEFMET";
      case NT::TYPE:
        return "TYPE";
      case NT::ARGS:
        return "ARGS";
      case NT::LISTCMD:
        return "LISTCMD";
      case NT::CMD:
        return "CMD";
      case NT::EXP:
        return "EXP";
      case NT::EXP2:
        return "EXP2";
      case NT::GREATER:
        return "GREATER";
      case NT::GREATER2:
        return "GREATER2";
      case NT::ADD:
        return "ADD";
      case NT::ADD2:
        return "ADD2";
      case NT::MUL:
        return "MUL";
      case NT::MUL2:
        return "MUL2";
      case NT::NEG:
        return "NEG";
      case NT::OBJ:
        return "OBJ";
      case NT::OBJATOM:
        return "OBJATOM";
      case NT::OBJMET:
        return "OBJMET";
      case NT::LISTEXP:
        return "LISTEXP";
      default:
        break;
      }
      return "UNKNOWN_NT";
    }

    const static std::string symbol_to_string(T t)
    {
      switch (t)
      {
      case T::CLASS:
        return "class";
      case T::PUBLIC:
        return "public";
      case T::STATIC:
        return "static";
      case T::VOID:
        return "void";
      case T::MAIN:
        return "main";
      case T::STRING:
        return "String";
      case T::IF:
        return "if";
      case T::ELSE:
        return "else";
      case T::WHILE:
        return "while";
      case T::SYSTEM:
        return "System";
      case T::OUT:
        return "out";
      case T::PRINTLN:
        return "println";
      case T::RETURN:
        return "return";
      case T::EXTENDS:
        return "extends";
      case T::INT:
        return "int";
      case T::BOOLEAN:
        return "boolean";
      case T::TRUE:
        return "true";
      case T::FALSE:
        return "false";
      case T::THIS:
        return "this";
      case T::NEW:
        return "new";
      case T::PLUS:
        return "+";
      case T::MINUS:
        return "-";
      case T::MULT:
        return "*";
      case T::GT:
        return ">";
      case T::AND:
        return "&&";
      case T::NOT:
        return "!";
      case T::ASSIGN:
        return "=";
      case T::LPAR:
        return "(";
      case T::RPAR:
        return ")";
      case T::LBRACE:
        return "{";
      case T::RBRACE:
        return "}";
      case T::LBRACKET:
        return "[";
      case T::RBRACKET:
        return "]";
      case T::DOT:
        return ".";
      case T::COMMA:
        return ",";
      case T::SEMICOLON:
        return ";";
      case T::ID:
        return "id";
      case T::NUMBER:
        return "number";
      case T::LENGTH:
        return "length";
      case T::END:
        return "EOF";
      default:
        break;
      }
      return "UNKNOWN_T";
    }
  } // namespace grammar
} // namespace jc
