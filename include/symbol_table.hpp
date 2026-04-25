#pragma once
#include <unordered_map>
#include <string>
#include "types.hpp"

namespace jc {

struct Symbol {
  std::string name;
  std::string category; 
  std::string type;    
  u32 line;
};

class SymbolTable {
private:
  std::unordered_map<std::string, Symbol> table;

public:
  void insert(const std::string& id, const std::string& category, u32 line = 0) 
  {
    if (table.find(id) == table.end()) {
      table[id] = Symbol{id, category, "", line};
    }
  }

  bool exists(const std::string& id) const {
    return table.find(id) != table.end();
  }

  const std::unordered_map<std::string, Symbol>& get_all() const {
    return table;
  }
};

}