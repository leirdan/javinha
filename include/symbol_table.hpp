#pragma once
#include <unordered_map>
#include <string>

namespace jc {

struct Symbol {
  std::string name;
  std::string type;   
  size_t line;        // onde foi declarado ?
};

class SymbolTable {
private:
  std::unordered_map<std::string, Symbol> table;

public:
  void insert(const std::string& id, size_t line = 0) {
    if (table.find(id) == table.end()) {
      table[id] = Symbol{id, "", line};
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