#pragma once
#include <unordered_map>
#include <string>
#include <sstream>
#include <ostream>
#include <iostream>
#include "types.hpp"
#include "utils.hpp"

namespace jc
{
  enum class SymbolCategory : u8
  {
    CLASS,
    METHOD,
    LOCAL,
    PARAM,
  };

  struct Symbol
  {
    std::string name;
    std::optional<std::string> type;
    u32 line;
    SymbolCategory category;
    i8 scope;
    Symbol() {}
    Symbol(std::string id, SymbolCategory category, u32 line = 0, i8 scope = -1)
    {
      this->name = id;
      this->type = std::nullopt;
      this->category = category;
      this->line = line;
      this->scope = scope;
    }
    Symbol(std::string id, std::string type, SymbolCategory category, u32 line = 0, i8 scope = -1)
    {
      this->name = id;
      this->type = type;
      this->category = category;
      this->line = line;
      this->scope = scope;
    }
  };

  class SymbolTable
  {
  private:
    std::unordered_map<std::string, std::vector<Symbol>> table; // filosofia - o símbolo mais à esquerda tem um escopo mais restrito que qualquer outro à direita

  public:
    void insert(std::string id, SymbolCategory category = SymbolCategory::LOCAL, u8 scope = -1, u32 line = 0)
    {
      if (table.find(id) == table.end())
      {
        table[id] = {};
      }
      table[id].emplace_back(Symbol(id, category, line, scope));
    }

    void insert(std::string id, std::string type, SymbolCategory category = SymbolCategory::LOCAL, i8 scope = -1, u32 line = 0)
    {
      if (table.find(id) == table.end())
      {
        table[id] = {};
      }
      table[id].emplace_back(Symbol(id, type, category, line, scope));
    }

    const Symbol &get(const std::string &id)
    {
      if (exists(id))
        return table.at(id).front(); // TODO
    }

    bool exists(const std::string &id) const
    {
      return table.find(id) != table.end();
    }

    const std::unordered_map<std::string, std::vector<Symbol>> &get_all() const
    {
      return table;
    }

    void print(std::ostream &stream = std::cout) const
    {
      for (const auto &[k, v] : table)
      {
        std::stringstream str;
        std::for_each(v.begin(), v.end(), [&str](const Symbol &s)
                      {
                        str << "(" << s.name << ", scope " << (int)s.scope << ", category: " << to_string(s.category);
                        if (s.type.has_value())
                        {
                          str << ", type '" << s.type.value() << "')\n";
                        }
                        else
                        {
                          str << ")\n";
                        }
                        // << v.front().name
                        // << " | tipo: " << v.type
                        // << " | escopos: (" << scopes_str.str() << ")"
                        // << " | linha: " << v.line << "\n";
                      });
        stream << str.str();
      }
    };
  };
}