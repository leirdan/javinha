#pragma once
#include <unordered_map>
#include <string>
#include <sstream>
#include "types.hpp"

namespace jc
{
  struct Symbol
  {
    std::vector<i8> scopes{};
    std::string name;
    std::string type;
    u32 line;
    Symbol() {}
    Symbol(std::string id, std::string type, u32 line = 0, i8 scope = -1)
    {
      this->name = id;
      this->type = type;
      this->line = line;
      if (std::find(scopes.begin(), scopes.end(), scope) == scopes.end() && scope != -1)
        scopes.push_back(scope);
    }
  };

  class SymbolTable
  {
  private:
    std::unordered_map<std::string, Symbol> table;

  public:
    void insert(std::string id, u32 line = 0, i8 scope = -1)
    {
      if (table.find(id) == table.end())
      {
        table[id] = Symbol(id, "", line, scope);
      }
    }

    void add_scope_to(std::string id, u8 scope)
    {
      if (table.find(id) != table.end())
      {
        auto &scopes = table[id].scopes;
        if (std::find(scopes.begin(), scopes.end(), scope) == scopes.end())
          scopes.push_back(scope);
      }
    }

    const Symbol &get(const std::string &id)
    {
      if (exists(id))
        return table.at(id);
    }

    bool exists(const std::string &id) const
    {
      return table.find(id) != table.end();
    }

    const std::unordered_map<std::string, Symbol> &get_all() const
    {
      return table;
    }

    void print() const
    {
      for (const auto &[k, v] : table)
      {
        std::stringstream scopes_str;
        std::for_each(v.scopes.begin(), v.scopes.end(), [&scopes_str](const u8 &s)
                      { scopes_str << (int)s << ", "; });
        std::cout
            << v.name
            << " | tipo: " << v.type
            << " | escopos: (" << scopes_str.str() << ")"
            << " | linha: " << v.line << "\n";
      }
    }
  };

}