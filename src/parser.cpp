#include <utility>
#include <unordered_set>
#include "parser.hpp"
#include "utils.hpp"

using namespace jc;
static T map_token(const Token &t);

bool parser::State::is_complete() const
{
  return dot >= rhs->size;
}

const std::optional<grammar::GSymbol> parser::State::next_symbol() const
{
  return dot < rhs->size ? std::make_optional<GSymbol>(rhs->data[dot])
                         : std::nullopt;
}

bool parser::Parser::earley_parse(const std::vector<Token> &&tokens)
{
  const auto *g = &grammar::grammar; // evitar ficar usando grammar::grammar;

  u64 n = tokens.size();
  std::vector<StateSet> chart(n + 1);
  GProduction def = grammar::grammar.at(0).rhs.at(0);
  // initial state
  chart.at(0).insert(State(grammar::start_symbol,
                           &def, (u8)0, (u8)0));

  log::debug("Preparado estado inicial para Earley Parser.");

  for (u64 i = 0; i < n; i++)
  {
    log::debug("Iteração " + std::to_string(i) + " do loop externo.");
    bool added = true;
    do
    {
      added = false;
      // copy current set to safely iterate while modifying
      StateSet current_set = chart.at(i);
      for (const State &state : current_set)
      {
        log::debug("- State atual: " + state.to_string());
        if (!state.is_complete())
        {
          log::debug("  - State não completo. ");
          std::optional<GSymbol> sym = state.next_symbol(); // TODO: adaptar para uma verificação pelo type de sym?
          if (sym.has_value())
          {
            i8 idx = grammar::contains_key(*sym);
            if (idx != -1) // non-terminal
            {
              for (const auto &sym_prods : g->at(idx).rhs)
              {
                auto nt = static_cast<NT>(sym->value);
                State newState(nt, &sym_prods, (u8)0, i);
                if (chart.at(i).insert(newState).second)
                {
                  log::debug("  - Adicionado " + sym->to_string() + " no chart " + std::to_string(i) + " com a produção " + sym_prods.to_string());
                  added = true;
                }
              }
            }
            else
            { // terminal
              log::debug("  - Próximo símbolo: " + sym->to_string() + ", terminal");
              if (i < n && sym->type == SymbolType::TERMINAL && sym->value == (u8)map_token(tokens.at(i)))
              {
                State new_state = State(state.lhs, state.rhs, state.dot + 1, state.start + 1);
                if (chart.at(i + 1).insert(new_state).second)
                {
                  log::debug("  - Ponto avançou para '" + sym->to_string() + "' e foi adicionado no próximo chart ");
                  added = true;
                }
              }
            }
          }
        }
        else
        { // complete
          log::debug("  - State completo. ");
          StateSet start_chart = chart.at(state.start);
          for (const auto &s : start_chart)
          {
            if (!s.is_complete())
            {
              const auto sym = s.next_symbol();
              if (sym.has_value() && sym->value == (u8)s.lhs)
              {
                State new_state = State(state.lhs, state.rhs, state.dot + 1, state.start);
                if (chart.at(i).insert(new_state).second)
                  added = true;
              }
            }
          }
        }
      }

    } while (added);
  }

  for (const auto &s : chart.at(n))
  {
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
  case TokenType::STRING:
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
    break;
  }
}