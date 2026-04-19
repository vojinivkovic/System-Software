#ifndef _SYMBOL_TABLE_HPP_
#define _SYMBOL_TABLE_HPP_

#include <cstdlib>
#include <vector>
#include "symbol.hpp"

class SymbolTable
{
public:
  SymbolTable() = default;
private:
  static size_t currentSection;
  std::vector<Symbol*> table;
};

#endif