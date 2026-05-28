#include <utility>
#include <unordered_set>
#include <exception>
#include <iostream>
#include <stdexcept>
#include "parser.hpp"
#include "utils.hpp"

using namespace jc::parser;
static T map_token(const Token &t);

bool State::is_complete() const
{
  return dot >= rhs->size;
}

const std::optional<grammar::GSymbol> State::next_symbol() const
{
  return dot < rhs->size ? std::make_optional<GSymbol>(rhs->data[dot])
                         : std::nullopt;
}

bool Parser::earley_parse(const std::vector<Token> &&tokens)
{
  u64 n = tokens.size();
  std::vector<StateSet> chart(n + 1);
  GProduction def = grammar::grammar.at(0).rhs.at(0);
  chart.at(0).insert(State(grammar::start_symbol,
                           &def, (u8)0, (u8)0));

  log::debug("Preparado estado inicial para Earley Parser.");

  for (u64 i = 0; i <= n; i++)
  {
    log::debug("=== Coluna " + std::to_string(i) + " do chart ===");
    bool added = true;
    do
    {
      added = false;
      StateSet current_set = chart.at(i);
      for (const State &state : current_set)
      {
        log::debug("  - State atual: " + state.to_string());
        if (state.is_complete())
        {
          if (this->complete(chart, state, i))
            added = true;
        }
        else
        {
          std::optional<GSymbol> sym = state.next_symbol();
          if (!sym.has_value())
            continue;

          if (sym->type == SymbolType::NON_TERMINAL)
          {
            if (this->predict(chart, state, *sym, i))
              added = true;
          }
          else
          {
            if (this->scan(chart, state, *sym, i, tokens))
              added = true;
          }
        }
      }
    } while (added);

    if ((i < n && chart.at(i + 1).empty()))
    {
      log::debug("ERRO SINTÁTICO: Falha ao processar o token '" + tokens.at(i).to_string() + "'");

      std::string expected_msg = "";
      for (const auto &s : chart.at(i))
      {
        auto sym = s.next_symbol();
        if (sym.has_value() && sym->type == SymbolType::TERMINAL)
        {
          expected_msg += sym->to_string() + " ";
        }
      }

      add_error(i, tokens.at(i).line, "Falha ao processar o token", tokens.at(i).to_string(), expected_msg);
      log::debug("ERRO SINTÁTICO adicionado à lista de erros");

      bool recovered = false;

      for (u64 k = i; k < n && !recovered; k++)
      {
        T mapped_t = map_token(tokens.at(k));

        if (mapped_t == T::SEMICOLON || mapped_t == T::RBRACE || mapped_t == T::RBRACKET)
        {
          log::debug("Encontrado possível ponto de sincronização na posição " + std::to_string(k));

          for (i64 c = i; c >= 0; c--)
          {
            bool chart_expects_sync = false;
            for (const auto &s : chart.at(c))
            {
              auto sym = s.next_symbol();
              if (sym.has_value() && sym->type == SymbolType::TERMINAL && sym->value == (u8)mapped_t)
              {
                chart_expects_sync = true;
                break;
              }
            }

            if (chart_expects_sync)
            {
              chart.at(k) = chart.at(c);

              i = k - 1;
              recovered = true;

              log::debug("Recuperado com sucesso! Revertido ao chart " + std::to_string(c) + " e saltando entrada para token " + std::to_string(k));
              break;
            }
          }
        }
      }

      if (!recovered)
      {
        log::debug("Falha crítica na recuperação. Abortando análise.");
        return false;
      }
    }
  }

  return this->has_ended(chart, n);
}

bool Parser::predict(std::vector<StateSet> &chart, const State &state, const GSymbol &symbol, u64 it)
{
  bool added = false;
  i8 idx = grammar::contains_key(symbol);
  if (idx == -1)
    return false;

  for (const auto &prod : this->grammar->at(idx).rhs)
  {
    State new_state(static_cast<NT>(symbol.value), &prod, 0, it);
    if (chart.at(it).insert(new_state).second)
    {
      log::debug("      - Predict: " + new_state.to_string());
      added = true;
    }
  }
  return added;
}

bool Parser::scan(std::vector<StateSet> &chart, const State &state, const GSymbol &symbol, u64 it, const std::vector<Token> &tokens)
{
  if (symbol.type == SymbolType::LAMBDA)
  {
    State new_state = State(state.lhs, state.rhs, state.dot + 1, state.start);
    if (chart.at(it).insert(new_state).second)
      return true;
  }
  else
  {
    if (it < tokens.size() && symbol.value == (u8)map_token(tokens.at(it)))
    {
      State new_state(state.lhs, state.rhs, state.dot + 1, state.start);
      if (chart.at(it + 1).insert(new_state).second)
      {
        log::debug("      - Scan: " + new_state.to_string());
        return true;
      }
    }
  }

  return false;
}

bool Parser::complete(std::vector<StateSet> &chart, const State &state, u64 it)
{
  bool added = false;
  log::debug("    - State completo, retornado aos states da iteração " + std::to_string(state.start));
  StateSet start_chart = chart.at(state.start);
  for (const auto &s : start_chart)
  {
    log::debug("    - State capturado: " + s.to_string());
    if (s.is_complete())
      continue;

    const std::optional<GSymbol> sym = s.next_symbol();
    if (sym.has_value() && sym->type == SymbolType::NON_TERMINAL && sym->value == (u8)state.lhs)
    {
      State new_state = State(s.lhs, s.rhs, s.dot + 1, s.start);
      if (chart.at(it).insert(new_state).second)
      {
        log::debug("      - State avança: " + new_state.to_string());
        added = true;
      }
    }
  }
  return added;
}

bool Parser::has_ended(const std::vector<StateSet> &chart, u64 last)
{
  log::debug("Verificação do último estado do Parser.");
  for (const auto &s : chart.at(last))
  {
    log::debug(s.to_string() + "\n");
    if (s.lhs == start_symbol && s.is_complete() && s.start == 0)
      return true;
  }
  return false;
}

static T map_token(const Token &t)
{
  switch (t.type)
  {
  case TokenType::NUMBER:
    return T::NUMBER;
  case TokenType::IDENTIFIER:
    if (t.value == "main")
      return T::MAIN;
    if (t.value == "System")
      return T::SYSTEM;
    if (t.value == "out")
      return T::OUT;
    if (t.value == "println")
      return T::PRINTLN;
    return T::ID;
  case TokenType::DELIMITER:
    if (t.value == ";")
      return T::SEMICOLON;
    else if (t.value == ",")
      return T::COMMA;
    else if (t.value == ".")
      return T::DOT;
    else if (t.value == "(")
      return T::LPAR;
    else if (t.value == ")")
      return T::RPAR;
    else if (t.value == "{")
      return T::LBRACE;
    else if (t.value == "}")
      return T::RBRACE;
    else if (t.value == "[")
      return T::LBRACKET;
    else if (t.value == "]")
      return T::RBRACKET;
  case TokenType::OPERATOR:
    if (t.value == "+")
      return T::PLUS;
    else if (t.value == "-")
      return T::MINUS;
    else if (t.value == "*")
      return T::MULT;
    else if (t.value == "=")
      return T::ASSIGN;
    else if (t.value == "&&")
      return T::AND;
    else if (t.value == "!")
      return T::NOT;
    else if (t.value == ">")
      return T::GT;
  case TokenType::KEYWORD:
    if (t.value == "class")
      return T::CLASS;
    else if (t.value == "public")
      return T::PUBLIC;
    else if (t.value == "static")
      return T::STATIC;
    else if (t.value == "void")
      return T::VOID;
    else if (t.value == "int")
      return T::INT;
    else if (t.value == "String")
      return T::STRING;
    else if (t.value == "boolean")
      return T::BOOLEAN;
    else if (t.value == "if")
      return T::IF;
    else if (t.value == "else")
      return T::ELSE;
    else if (t.value == "while")
      return T::WHILE;
    else if (t.value == "return")
      return T::RETURN;
    else if (t.value == "new")
      return T::NEW;
    else if (t.value == "this")
      return T::THIS;
    else if (t.value == "extends")
      return T::EXTENDS;
    else if (t.value == "true")
      return T::TRUE;
    else if (t.value == "false")
      return T::FALSE;
  default:
    return T::END;
  }
}