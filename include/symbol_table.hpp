#pragma once
#include <unordered_map>
#include <string>

namespace jc {

struct Symbol {
  std::string name;
  std::string category; 
  std::string type;    
  uint32_t line;
};

class SymbolTable {
private:
  std::unordered_map<std::string, Symbol> table;

public:
  void insert(const std::string& id,
              const std::string& category,
              uint32_t line = 0) 
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