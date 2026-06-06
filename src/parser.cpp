#include <utility>
#include <unordered_set>
#include <variant>
#include <exception>
#include <iostream>
#include <stdexcept>
#include "parser.hpp"
#include "utils.hpp"

using namespace jc::parser;
static T map_token(const Token &t);

struct DFSEdge
{
  State state;
  u64 finish;
};

struct PTNode
{
  // std::string  rule;
  grammar::NT rule;
  std::vector<std::unique_ptr<struct PTree>> children;
};

struct PTree
{
  std::variant<std::optional<Token>, PTNode> value;
};

using DFSNode = std::vector<DFSEdge>;
std::optional<std::vector<std::pair<u64, DFSEdge>>> df_search(
    std::function<std::vector<DFSEdge>(i64, const u64 &)> edges,
    std::function<u64(i64, const DFSEdge &)> child,
    std::function<bool(i64, const u64 &)> pred,
    const u64 &root_node)
{
  // Função lambda recursiva (o "let rec aux" do funcional)
  // Usamos std::function para permitir a autorreferência (recursão) na lambda
  std::function<std::optional<std::vector<std::pair<u64, DFSEdge>>>(i64, const u64 &)> rec_aux =
      [&](i64 depth, const u64 &current_node) -> std::optional<std::vector<std::pair<u64, DFSEdge>>>
  {
    // if pred depth root then Some []
    if (pred(depth, current_node))
    {
      return std::vector<std::pair<u64, DFSEdge>>{}; // Caminho vazio encontrado (sucesso)
    }

    // edges depth root -> Obtém as arestas vizinhas
    auto available_edges = edges(depth, current_node);

    // O bloco "opt_find_mem" está iterando sobre as arestas
    for (const auto &edge : available_edges)
    {
      // child depth edge -> descobre o próximo nó
      u64 next_node = child(depth, edge);
      auto result = rec_aux(depth + 1, next_node);

      if (result.has_value())
      {
        // Insere o nó atual e a aresta usada no início do caminho funcional
        // No C++, como estamos voltando da recursão, podemos inserir no vetor
        result->insert(result->begin(), std::make_pair(current_node, edge));
        return result; // Retorna o caminho completo
      }
    }

    // Se nenhuma aresta levou ao destino: None
    return std::nullopt;
  };

  // in aux -1 root
  return rec_aux(0, root_node);
}

// me dê os estados que começam aqui e me diga em qual coluna do chart eles terminaram!!
std::vector<std::pair<u64, DFSEdge>> top_list(
    const std::vector<Token> &tokens,
    const std::vector<DFSNode> &chart,
    const DFSEdge &edge,
    u64 start)
{
  const GProduction *symbols = edge.state.rhs;
  u8 bottom = symbols->size;

  auto pred = [bottom, finish = edge.finish](i64 depth, u64 current_start) -> bool
  {
    return (depth == (i64)bottom) && (current_start == finish);
  };

  auto child = [](i64 depth, const DFSEdge &edge) -> u64
  {
    return edge.finish;
  };

  auto edges = [&](i64 depth, u64 current_start) -> DFSNode
  {
    // if depth >= DA.length symbols then DA.make_empty ()
    if (depth < 0 || depth >= (i64)bottom)
    {
      return {}; // Retorna vetor vazio (falha na busca)
    }

    // match symbols >: depth with ...
    const GSymbol &current_symbol = symbols->data[depth];

    // Se o símbolo atual for um Terminal:
    if (current_symbol.type == SymbolType::TERMINAL)
    {
      const u8 term = current_symbol.value;

      // if t (input start)
      if (term == (u8)map_token(tokens.at(current_start)))
      {
        std::cout << "[EDGES] terminal ok=" << (int)term
                  << " token='" << tokens.at(current_start).value << "'\n";
        return {DFSEdge{State{}, current_start + 1}};
      }
      else
      {
        return {}; // Token não bateu, falha
      }
    }
    else if (current_symbol.type == SymbolType::LAMBDA)
    {
      // Lambda consome zero tokens: finish = current_start (não avança)
      std::cout << "[EDGES] lambda: current_start=" << current_start << "\n";
      return {DFSEdge{State{}, current_start}};
    }
    else
    {
      // (chart >: start) // filtrar por name
      // Pega todas as arestas que partem de 'current_start' no chart
      const auto &candidates = chart[current_start];

      std::cout << "[EDGES] NT value=" << (int)current_symbol.value
                << " candidates em [" << current_start << "]=" << candidates.size() << "\n";
      for (const auto &e : candidates)
        std::cout << "  candidate lhs=" << (int)e.state.lhs
                  << " finish=" << e.finish << "\n";

      DFSNode filtered_edges;

      for (const auto &edge : candidates)
      {
        // Basta checar se o símbolo do estado bate com o que a gramática espera
        if ((u8)edge.state.lhs == current_symbol.value)
        {
          filtered_edges.push_back(edge); // Ele já vai com o .finish preenchido lá do loop!
        }
      }
      return filtered_edges;
    }
  };

  auto path_opt = df_search(edges, child, pred, start);

  // | None -> failwith "there's always a solution"
  if (!path_opt.has_value())
  {
    throw std::runtime_error("there's always a solution");
  }

  // | Some path -> path
  return path_opt.value();
}

PTree generate_parse_tree(
    const std::vector<Token> &tokens,
    // const std::vector<std::vector<DFSEdge>>& chart,
    const std::vector<StateSet> &chart,
    const std::vector<std::vector<DFSEdge>> &reversed_chart)
{
  std::cout << "[GEN] iniciando, finish=" << (chart.size() - 1) << "\n";

  u64 start = 0;
  u64 finish = chart.size() - 1;

  std::function<PTree(u64, const DFSEdge &)> rec_aux =
      [&](u64 current_start, const DFSEdge &edge) -> PTree
  {
    PTree target_node;

    // Encontrou uma folha na árvore e essa folha é um token!
    if (edge.state.rhs == nullptr)
    {
      target_node.value = std::make_optional(tokens.at(current_start));
      return target_node;
    }
    else
    {
      PTNode internal;
      internal.rule = edge.state.lhs;
      // Obtém a lista de sub-passos necessários para reconstruir essa regra específica
      // top_list retorna um std::vector<std::pair<u64, DFSEdge>>
      auto sub_paths = top_list(tokens, reversed_chart, edge, current_start);

      // Mapeia cada sub-passo chamando a recursão 'aux'
      for (const auto &[step_start, step_edge] : sub_paths)
      {
        PTree child_tree = rec_aux(step_start, step_edge);
        internal.children.push_back(std::make_unique<PTree>(std::move(child_tree)));
      }
      target_node.value = std::move(internal);
      return target_node;
    }
  };

  std::optional<DFSEdge> root_edge = std::nullopt;

  // Varre a primeira coluna do chart (onde start = -1) procurando o estado final completo
  for (const auto &edge : reversed_chart.at(start))
  {
    if (edge.finish == finish && edge.state.lhs == jc::grammar::NT::START && edge.state.is_complete())
    {
      root_edge = edge;
      break;
    }
  }
  if (!root_edge.has_value())
  {
    throw std::runtime_error("Are you sure this parse succeeded?");
  }

  return rec_aux(start, root_edge.value());
};

bool State::is_complete() const
{
  return dot >= rhs->size;
}

const std::optional<grammar::GSymbol> State::next_symbol() const
{
  return dot < rhs->size ? std::make_optional<GSymbol>(rhs->data[dot])
                         : std::nullopt;
}

void print_tree(const PTree &node, int indent = 0)
{
  std::string pad(indent * 2, ' ');

  std::visit([&](auto &&val)
  {
    using T = std::decay_t<decltype(val)>;

    if constexpr (std::is_same_v<T, PTNode>)
    {
      std::cout << pad << "[" << (int)val.rule << "]\n";
      for (const auto &child : val.children)
        print_tree(*child, indent + 1);
    }
    else if constexpr (std::is_same_v<T, std::optional<Token>>)
    {
      if (val.has_value())
        std::cout << pad << "\"" << val->value << "\"\n";
    }
  }, node.value);
}

bool Parser::earley_parse(const std::vector<Token> &&tokens)
{
  u64 n = tokens.size();
  std::vector<StateSet> chart(n + 1);
  const GProduction *def = &grammar::grammar.at(0).rhs.at(0);
  chart.at(0).insert(State(grammar::start_symbol, def, (u8)0, (u8)0));

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

  if (this->has_ended(chart, n))
  {
    using DFSStateSet = std::vector<std::vector<DFSEdge>>;
    DFSStateSet reversed_chart(n + 1);
    for (u64 i = 0; i <= n; i++)
    {
      StateSet state_set = chart.at(i);
      for (auto &state : state_set)
      {
        if (!state.is_complete())
          continue;

        u64 src = state.start;
        DFSEdge edge;
        edge.state = state;
        edge.finish = i;
        // new_state.start = i;
        reversed_chart[src].push_back(edge);
      }
    }

    auto tree = generate_parse_tree(tokens, chart, reversed_chart);
    print_tree(tree);
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