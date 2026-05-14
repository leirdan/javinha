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
      PROG = 1,
      MAINC,
      DEFCL,
      DEFCL2,
      DEFCL3,
      DEFCL4,
      DEFCL5,
      DEFCL6,
      DEFVAR,
      DEFVAR2,
      DEFMET,
      DEFMET2,
      DEFMET3,
      DEFMET4,
      TYPE,
      TYPE2,
      ARGS,
      ARGS2,
      CMD,
      CMD2,
      EXP,
      EXP2,
      EXP3,
      LISTEXP,
      LISTEXP2,
      LISTEXP3,
      ID,
      ID2,
      CHAR,
      WORD,
      WORD2,
      NUMBER
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
      LT,
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

    // Até 24 símbolos na regra, mais do que o necessário
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

    const static NT start_symbol = NT::PROG;
    const u8 rules = 10;

    static std::array<GRule, rules> grammar = {
        // PROG
        GRule{
            .rhs = {
                GProduction{GSymbol::N(NT::MAINC), GSymbol::N(NT::DEFCL)}},
            .lhs = NT::PROG},
        // MAINC
        GRule{.rhs = {GProduction{GSymbol::T(T::CLASS), GSymbol::T(T::ID), GSymbol::T(T::LBRACE), GSymbol::T(T::PUBLIC), GSymbol::T(T::STATIC), GSymbol::T(T::VOID), GSymbol::T(T::MAIN), GSymbol::T(T::LPAR), GSymbol::T(T::STRING), GSymbol::T(T::LBRACKET), GSymbol::T(T::RBRACKET), GSymbol::T(T::ID), GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::CMD), GSymbol::T(T::RBRACE), GSymbol::T(T::RBRACE)}}, .lhs = NT::MAINC},
        // DEFCL
        GRule{.rhs = {GProduction{GSymbol::T(T::CLASS), GSymbol::T(T::ID), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFVAR), GSymbol::N(NT::DEFMET), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFCL)}, GProduction{GSymbol::T(T::CLASS), GSymbol::T(T::ID), GSymbol::T(T::EXTENDS), GSymbol::T(T::ID), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFVAR), GSymbol::N(NT::DEFMET), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFCL)}, GProduction{GSymbol::L()}}, .lhs = NT::DEFCL},
        // DEFVAR
        GRule{.rhs = {GProduction{GSymbol::N(NT::TYPE), GSymbol::T(T::ID), GSymbol::T(T::SEMICOLON), GSymbol::N(NT::DEFVAR)}, GProduction{GSymbol::L()}}, .lhs = NT::DEFVAR},
        // DEFMET
        GRule{.rhs = {GProduction{GSymbol::T(T::PUBLIC), GSymbol::N(NT::TYPE), GSymbol::T(T::ID), GSymbol::T(T::LPAR), GSymbol::N(NT::ARGS), GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFVAR), GSymbol::N(NT::CMD), GSymbol::T(T::RETURN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFMET)}, GProduction{GSymbol::T(T::PUBLIC), GSymbol::N(NT::TYPE), GSymbol::T(T::ID), GSymbol::T(T::LPAR), GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFVAR), GSymbol::N(NT::CMD), GSymbol::T(T::RETURN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFMET)}, GProduction{GSymbol::L()}}, .lhs = NT::DEFMET},
        // TYPE
        GRule{.rhs = {GProduction{GSymbol::T(T::INT), GSymbol::T(T::LBRACKET), GSymbol::T(T::RBRACKET)}, GProduction{GSymbol::T(T::BOOLEAN)}, GProduction{GSymbol::T(T::INT)}, GProduction{GSymbol::T(T::ID)}}, .lhs = NT::TYPE},
        // ARGS
        GRule{.rhs = {GProduction{GSymbol::N(NT::TYPE), GSymbol::T(T::ID)}, GProduction{GSymbol::N(NT::TYPE), GSymbol::T(T::ID), GSymbol::T(T::COMMA), GSymbol::N(NT::ARGS)}}, .lhs = NT::ARGS},
        // CMD
        GRule{.rhs = {GProduction{GSymbol::T(T::LBRACE), GSymbol::N(NT::CMD), GSymbol::T(T::RBRACE)}, GProduction{GSymbol::T(T::IF), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::N(NT::CMD), GSymbol::T(T::ELSE), GSymbol::N(NT::CMD)}, GProduction{GSymbol::T(T::WHILE), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::N(NT::CMD)}, GProduction{GSymbol::T(T::SYSTEM), GSymbol::T(T::DOT), GSymbol::T(T::OUT), GSymbol::T(T::DOT), GSymbol::T(T::PRINTLN), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::T(T::SEMICOLON)}, GProduction{GSymbol::T(T::ID), GSymbol::T(T::ASSIGN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON)}, GProduction{GSymbol::T(T::ID), GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET), GSymbol::T(T::ASSIGN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON)}}, .lhs = NT::CMD},
        // EXP
        GRule{.rhs = {GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::AND), GSymbol::N(NT::EXP)}, GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::LT), GSymbol::N(NT::EXP)}, GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::GT), GSymbol::N(NT::EXP)}, GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::PLUS), GSymbol::N(NT::EXP)}, GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::MINUS), GSymbol::N(NT::EXP)}, GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::MULT), GSymbol::N(NT::EXP)}, GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET)}, GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::DOT), GSymbol::T(T::LENGTH)}, GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::DOT), GSymbol::N(NT::EXP), GSymbol::T(T::LPAR), GSymbol::N(NT::LISTEXP), GSymbol::T(T::RPAR)}, GProduction{GSymbol::T(T::NEW), GSymbol::T(T::ID), GSymbol::T(T::LPAR), GSymbol::T(T::RPAR)}, GProduction{GSymbol::T(T::NOT), GSymbol::N(NT::EXP)}, GProduction{GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR)}, GProduction{GSymbol::T(T::TRUE)}, GProduction{GSymbol::T(T::FALSE)}, GProduction{GSymbol::T(T::ID)}, GProduction{GSymbol::T(T::NUMBER)}, GProduction{GSymbol::T(T::THIS)}, GProduction{GSymbol::T(T::NEW), GSymbol::T(T::INT), GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET)}}, .lhs = NT::EXP},
        // LISTEXP
        GRule{.rhs = {GProduction{GSymbol::N(NT::EXP), GSymbol::T(T::COMMA), GSymbol::N(NT::LISTEXP)}, GProduction{GSymbol::N(NT::EXP)}, GProduction{GSymbol::L()}}, .lhs = NT::LISTEXP}};

    // static std::array<GRule, rules> grammar = {
    //     // PROG
    //     GRule{.rhs = {{GSymbol::N(NT::MAINC), GSymbol::N(NT::DEFCL)}}, .lhs = NT::PROG},
    //     // MAINC
    //     {.rhs = {{GSymbol::T(T::CLASS), GSymbol::N(NT::ID), GSymbol::T(T::LBRACE), GSymbol::T(T::PUBLIC), GSymbol::T(T::STATIC), GSymbol::T(T::VOID), GSymbol::T(T::MAIN), GSymbol::T(T::LPAR), GSymbol::T(T::STRING), GSymbol::T(T::LBRACKET), GSymbol::T(T::RBRACKET), GSymbol::N(NT::ID), GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::CMD), GSymbol::T(T::RBRACE), GSymbol::T(T::RBRACE)}}, .lhs = NT::MAINC},
    //     // DEFCL
    //     {.rhs = {{GSymbol::T(T::CLASS), GSymbol::N(NT::ID), GSymbol::N(NT::DEFCL2)}}, .lhs = NT::DEFCL},
    //     // DEFCL2
    //     {.rhs = {{GSymbol::N(NT::DEFCL3)}, {GSymbol::T(T::EXTENDS), GSymbol::N(NT::ID), GSymbol::N(NT::DEFCL3)}}, .lhs = NT::DEFCL2},
    //     // DEFCL3
    //     {.rhs = {{GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFCL4), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFCL6)}}, .lhs = NT::DEFCL3},
    //     // DEFCL4
    //     {
    //         .rhs = {{GSymbol::N(NT::DEFVAR), GSymbol::N(NT::DEFCL5)}, {GSymbol::N(NT::DEFCL5)}},
    //         .lhs = NT::DEFCL4,
    //     },
    //     // DEFCL5
    //     {.rhs = {{GSymbol::N(NT::DEFMET)}, {GSymbol::L()}}, .lhs = NT::DEFCL5},
    //     // DEFCL6
    //     {.rhs = {{GSymbol::N(NT::DEFCL)}, {GSymbol::L()}}, .lhs = NT::DEFCL6},
    //     // DEFVAR
    //     {.rhs = {{GSymbol::N(NT::TYPE), GSymbol::N(NT::ID), GSymbol::T(T::SEMICOLON), GSymbol::N(NT::DEFVAR2)}}, .lhs = NT::DEFVAR},
    //     // DEFVAR2
    //     {.rhs = {{GSymbol::N(NT::DEFVAR)}, {GSymbol::L()}}, .lhs = NT::DEFVAR2},
    //     // DEFMET
    //     {.rhs = {{GSymbol::T(T::PUBLIC), GSymbol::N(NT::TYPE), GSymbol::N(NT::ID), GSymbol::T(T::LPAR), GSymbol::N(NT::DEFMET2)}}, .lhs = NT::DEFMET},
    //     // DEFMET2
    //     {.rhs = {{GSymbol::N(NT::ARGS), GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFMET3)}, {GSymbol::T(T::RPAR), GSymbol::T(T::LBRACE), GSymbol::N(NT::DEFMET3)}}, .lhs = NT::DEFMET2},
    //     // DEFMET3
    //     {.rhs = {{GSymbol::N(NT::DEFVAR), GSymbol::N(NT::CMD), GSymbol::T(T::RETURN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFMET4)}, {GSymbol::N(NT::CMD), GSymbol::T(T::RETURN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON), GSymbol::T(T::RBRACE), GSymbol::N(NT::DEFMET4)}}, .lhs = NT::DEFMET3},
    //     // DEFMET4
    //     {.rhs = {{GSymbol::N(NT::DEFMET)}, {GSymbol::L()}}, .lhs = NT::DEFMET4},
    //     // TYPE
    //     {.rhs = {{GSymbol::T(T::INT), GSymbol::N(NT::TYPE2)}, {GSymbol::T(T::BOOLEAN)}}, .lhs = NT::TYPE},
    //     // TYPE2
    //     {.rhs = {{GSymbol::T(T::LBRACKET), GSymbol::T(T::RBRACKET)}, {GSymbol::L()}}, .lhs = NT::TYPE2},
    //     // ARGS
    //     {.rhs = {{GSymbol::N(NT::TYPE), GSymbol::N(NT::ID), GSymbol::N(NT::ARGS2)}}, .lhs = NT::ARGS},
    //     // ARGS2
    //     {.rhs = {{GSymbol::T(T::DOT), GSymbol::N(NT::ARGS)}, {GSymbol::L()}}, .lhs = NT::ARGS2},
    //     // CMD
    //     {.rhs = {{GSymbol::T(T::LBRACE), GSymbol::N(NT::CMD), GSymbol::T(T::RBRACE)}, {GSymbol::T(T::IF), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::N(NT::CMD), GSymbol::T(T::ELSE), GSymbol::N(NT::CMD)}, {GSymbol::T(T::WHILE), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::N(NT::CMD)}, {GSymbol::T(T::SYSTEM), GSymbol::T(T::DOT), GSymbol::T(T::OUT), GSymbol::T(T::DOT), GSymbol::T(T::PRINTLN), GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::T(T::SEMICOLON)}, {GSymbol::N(NT::ID), GSymbol::N(NT::CMD2)}}, .lhs = NT::CMD},
    //     // CMD2
    //     {.rhs = {{GSymbol::T(T::ASSIGN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON)}, {GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET), GSymbol::T(T::ASSIGN), GSymbol::N(NT::EXP), GSymbol::T(T::SEMICOLON)}}, .lhs = NT::CMD2},
    //     // EXP
    //     {.rhs = {{GSymbol::T(T::NEW), GSymbol::N(NT::ID), GSymbol::T(T::LPAR), GSymbol::T(T::RPAR), GSymbol::N(NT::EXP2)}, {GSymbol::T(T::NOT), GSymbol::N(NT::EXP), GSymbol::N(NT::EXP2)}, {GSymbol::T(T::LPAR), GSymbol::N(NT::EXP), GSymbol::T(T::RPAR), GSymbol::N(NT::EXP2)}, {GSymbol::T(T::TRUE), GSymbol::N(NT::EXP2)}, {GSymbol::T(T::FALSE), GSymbol::N(NT::EXP2)}, {GSymbol::N(NT::ID), GSymbol::N(NT::EXP2)}, {GSymbol::N(NT::NUMBER), GSymbol::N(NT::EXP2)}, {GSymbol::T(T::THIS), GSymbol::N(NT::EXP2)}, {GSymbol::T(T::NEW), GSymbol::T(T::INT), GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET), GSymbol::N(NT::EXP2)}}, .lhs = NT::EXP},
    //     // EXP2
    //     {
    //         .rhs = {{GSymbol::T(T::AND), GSymbol::N(NT::EXP)}, {GSymbol::T(T::GT), GSymbol::N(NT::EXP)}, {GSymbol::T(T::PLUS), GSymbol::N(NT::EXP)}, {GSymbol::T(T::MINUS), GSymbol::N(NT::EXP)}, {GSymbol::T(T::MULT), GSymbol::N(NT::EXP)}, {GSymbol::T(T::LBRACKET), GSymbol::N(NT::EXP), GSymbol::T(T::RBRACKET)}, {GSymbol::T(T::DOT), GSymbol::N(NT::EXP3)}, {GSymbol::L()}},
    //         .lhs = NT::EXP2,
    //     },
    //     // EXP3
    //     {.rhs = {{GSymbol::T(T::LENGTH)}, {GSymbol::N(NT::EXP), GSymbol::T(T::LPAR), GSymbol::N(NT::LISTEXP), GSymbol::T(T::RPAR)}, {GSymbol::N(NT::ID), GSymbol::T(T::LPAR), GSymbol::N(NT::LISTEXP), GSymbol::T(T::RPAR)}}, .lhs = NT::EXP3},
    //     // LISTEXP
    //     {.rhs = {{GSymbol::N(NT::EXP), GSymbol::N(NT::LISTEXP2)}}, .lhs = NT::LISTEXP},
    //     // LISTEXP2
    //     {.rhs = {{GSymbol::T(T::DOT), GSymbol::N(NT::LISTEXP3)}, {GSymbol::L()}}, .lhs = NT::LISTEXP2},
    //     // LISTEXP3
    //     {.rhs = {{GSymbol::N(NT::LISTEXP)}, {GSymbol::L()}}, .lhs = NT::LISTEXP3},
    //     // ID
    //     {.rhs = {{GSymbol::T(T::ID)}}, .lhs = NT::ID},
    //     // NUMBER
    //     {.rhs = {{GSymbol::T(T::NUMBER)}}, .lhs = NT::NUMBER},
    //     // {.rhs = {{GSymbol::N(NT::CHAR), GSymbol::N(NT::ID2)}}, .lhs = NT::ID},
    //     // {.rhs = {{GSymbol::N(NT::WORD)}, {GSymbol::L()}}, .lhs = NT::ID2},
    //     // {.rhs = {{GSymbol::N(NT::CHAR), GSymbol::N(NT::ID2)}}, .lhs = NT::ID},
    //     // {.rhs = {{GSymbol::N(NT::CHAR), GSymbol::N(NT::ID2)}}, .lhs = NT::ID},
    //     // {.rhs = {{GSymbol::N(NT::CHAR), GSymbol::N(NT::ID2)}}, .lhs = NT::ID},
    //     // {.rhs = {{GSymbol::N(NT::CHAR), GSymbol::N(NT::ID2)}}, .lhs = NT::ID}};

    // };

    // retorna o índice
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
    }

    const static std::string symbol_to_string(NT nt)
    {
      switch (nt)
      {
      case NT::PROG:
        return "PROG";
      case NT::MAINC:
        return "MAINC";
      case NT::DEFCL:
        return "DEFCL";
      case NT::DEFCL2:
        return "DEFCL2";
      case NT::DEFCL3:
        return "DEFCL3";
      case NT::DEFCL4:
        return "DEFCL4";
      case NT::DEFCL5:
        return "DEFCL5";
      case NT::DEFCL6:
        return "DEFCL6";
      case NT::DEFVAR:
        return "DEFVAR";
      case NT::DEFVAR2:
        return "DEFVAR2";
      case NT::DEFMET:
        return "DEFMET";
      case NT::DEFMET2:
        return "DEFMET2";
      case NT::DEFMET3:
        return "DEFMET3";
      case NT::DEFMET4:
        return "DEFMET4";
      case NT::TYPE:
        return "TYPE";
      case NT::TYPE2:
        return "TYPE2";
      case NT::ARGS:
        return "ARGS";
      case NT::ARGS2:
        return "ARGS2";
      case NT::CMD:
        return "CMD";
      case NT::CMD2:
        return "CMD2";
      case NT::EXP:
        return "EXP";
      case NT::EXP2:
        return "EXP2";
      case NT::EXP3:
        return "EXP3";
      case NT::LISTEXP:
        return "LISTEXP";
      case NT::LISTEXP2:
        return "LISTEXP2";
      case NT::LISTEXP3:
        return "LISTEXP3";
      case NT::ID:
        return "ID";
      case NT::ID2:
        return "ID2";
      case NT::CHAR:
        return "CHAR";
      case NT::WORD:
        return "WORD";
      case NT::WORD2:
        return "WORD2";
      case NT::NUMBER:
        return "NUMBER";
      default:
        return "UNKNOWN_NT";
      }
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
      default:
        return "UNKNOWN_T";
      }
    }
  } // namespace grammar
} // namespace jc