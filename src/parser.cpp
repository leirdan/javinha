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
    log::debug("=== Coluna " + std::to_string(i) + " do chart ===");
    bool added = true;
    do
    {
      added = false;
      // copy current set to safely iterate while modifying
      StateSet current_set = chart.at(i);
      for (const State &state : current_set)
      {
        log::debug("  - State atual: " + state.to_string());
        if (!state.is_complete())
        {
          log::debug("    - State não completo. ");
          std::optional<GSymbol> sym = state.next_symbol(); // TODO: adaptar para uma verificação pelo type de sym?
          if (sym.has_value())
          {
            if (sym->type == SymbolType::LAMBDA)
            {
              log::debug("    - Lambda");
              State new_state = State(state.lhs, state.rhs, state.dot + 1, state.start);

              if (chart.at(i).insert(new_state).second)
              {
                added = true;
              }
            }
            else
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
                    log::debug("    - Adicionado " + sym->to_string() + " no chart " + std::to_string(i) + " com a produção " + sym_prods.to_string());
                    added = true;
                  }
                }
              }
              else
              { // terminal
                log::debug("    - Próximo símbolo: " + sym->to_string() + ", terminal");
                // log::debug("  - Tipo: " + type_to_string(sym->type));
                // t_idx++;
                log::debug("    - Próximo token: " + tokens.at(i).to_string());
                if (i < n && sym->type == SymbolType::TERMINAL && sym->value == (u8)map_token(tokens.at(i)))
                {
                  State new_state = State(state.lhs, state.rhs, state.dot + 1, state.start);
                  if (chart.at(i + 1).insert(new_state).second)
                  {
                    log::debug("    - Ponto avançou para '" + sym->to_string() + "' e foi adicionado no próximo chart ");
                    added = true;
                  }
                }
              }
              // else if (i < n && sym->type == SymbolType::LAMBDA)
              // {
              // }
              // else if (i < n && sym->type == SymbolType::LAMBDA)
              // {
              // }
            }
          }
        }
        else
        { // complete
          log::debug("    - State completo, retornado aos states da iteração " + std::to_string(state.start));
          StateSet start_chart = chart.at(state.start);
          for (const auto &s : start_chart)
          {
            log::debug("    - State capturado: " + s.to_string());
            if (!s.is_complete())
            {
              const std::optional<GSymbol> sym = s.next_symbol();
              // log::debug("    - Comparação: " + sym->to_string() + " e " + symbol_to_string(state.lhs));
              if (sym.has_value() && sym->value == (u8)state.lhs) // state avança se o próximo símbolo for o mesmo que completou por último em outro chart
              {
                State new_state = State(s.lhs, s.rhs, s.dot + 1, s.start);
                if (chart.at(i).insert(new_state).second)
                {
                  log::debug("      - State avança: " + new_state.to_string());
                  added = true;
                }
              }
            }
          }
        }
      }

    } while (added);

    if (i < n && chart.at(i + 1).empty())
    {
      log::debug("ERRO SINTÁTICO: Falha ao processar o token '" + tokens.at(i).to_string() + "'");

      std::string expected_msg = "Esperava-se os terminais (IDs): ";
      for (const auto &s : chart.at(i))
      {
        auto sym = s.next_symbol();
        if (sym.has_value() && sym->type == SymbolType::TERMINAL)
        {
           expected_msg += std::to_string(sym->value) + " ";
        }
      }
      log::debug(std::move(expected_msg));

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
    if (t.value == "main")
      return T::MAIN;
    if (t.value == "System")
      return T::SYSTEM;
    if (t.value == "out")
      return T::OUT;
    if (t.value == "println")
      return T::PRINTLN;
    return T::ID;
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