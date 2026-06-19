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
    Symbol() {}
    Symbol(std::string id, SymbolCategory category, u32 line = 0)
    {
      this->name = id;
      this->type = std::nullopt;
      this->category = category;
      this->line = line;
    }
    Symbol(std::string id, std::string type, SymbolCategory category, u32 line = 0)
    {
      this->name = id;
      this->type = type;
      this->category = category;
      this->line = line;
    }
  };

  class Scope
  {
  public:
    std::string name;
    std::weak_ptr<Scope> parent; // evitar ref circular
    std::optional<std::string> parent_class;
    std::unordered_map<std::string, Symbol> symbols;
    std::vector<std::shared_ptr<Scope>> children;

    Scope(std::string name, std::shared_ptr<Scope> parent = nullptr)
        : name(name), parent(parent), parent_class(std::nullopt) {}

    bool insert(const Symbol &sym)
    {
      if (symbols.contains(sym.name))
        return false;

      symbols[sym.name] = sym;
      return true;
    }

    void print(std::ostream &stream, int depth) const
    {
      std::string indent(depth * 2, ' ');

      stream << indent << "- scope: " << name;
      if (parent_class)
        stream << " (extends " << *parent_class << ")";
      stream << "\n";

      for (const auto &[id, sym] : symbols)
      {
        stream << indent << "   - [" << to_string(sym.category) << "] " << id;
        if (sym.type)
          stream << ": " << *sym.type;
        stream << "\n";
      }

      for (const auto &child : children)
      {
        child->print(stream, depth + 1);
      }
    }
  };

  class SymbolTable
  {
  private:
    std::shared_ptr<Scope> root;
    std::shared_ptr<Scope> current_scope;
    std::unordered_map<std::string, std::shared_ptr<Scope>> class_scopes; // transitar entre classes por conta da herança

  public:
    SymbolTable()
    {
      root = std::make_shared<Scope>("global");
      current_scope = root;
    }

    void enter_scope(const std::string &scope_name)
    {
      auto new_scope = std::make_shared<Scope>(scope_name, current_scope);
      current_scope->children.push_back(new_scope);
      current_scope = new_scope;
    }

    void enter_class_scope(const std::string &class_name, const std::optional<std::string> &extends_class = std::nullopt)
    {
      auto new_scope = std::make_shared<Scope>(class_name, current_scope);
      new_scope->parent_class = extends_class;
      root->children.push_back(new_scope);
      class_scopes[class_name] = new_scope;
      current_scope = new_scope;
    }

    void exit_scope()
    {
      if (current_scope->parent.lock() != nullptr)
        current_scope = current_scope->parent.lock();
      else
        current_scope = root;
    }

    bool insert(std::string id, SymbolCategory category = SymbolCategory::LOCAL, u32 line = 0)
    {
      return current_scope->insert(Symbol(id, category, line));
    }

    bool insert(const std::string &id, const std::string &type, SymbolCategory category, u32 line = 0)
    {
      return current_scope->insert(Symbol(id, type, category, line));
    }

    // funcao essencial pra analise semantica verificar variaveis nao declaradas por ex.!
    std::optional<Symbol> lookup(const std::string &id)
    {
      auto scope = current_scope;
      while (scope != nullptr)
      {
        if (scope->symbols.contains(id))
          return scope->symbols[id];

        if (scope->parent_class.has_value())
        {
          auto parent = scope->parent_class.value();
          while (!parent.empty() && class_scopes.contains(parent))
          {
            auto base_class_scope = class_scopes[parent];
            if (base_class_scope->symbols.contains(id))
              return base_class_scope->symbols[id];
            parent = base_class_scope->parent_class.value_or("");
          }
        }

        scope = scope->parent.expired() ? nullptr : scope->parent.lock();
      }
      return std::nullopt;
    }

    bool exists(const std::string &id)
    {
      return lookup(id).has_value();
    }

    void print(std::ostream &stream = std::cout) const
    {
      stream << "[SYMBOL TABLE]\n";
      root->print(stream, 0);
    }
  };

}