#ifndef _SYMBOL_TABLE_HPP_
#define _SYMBOL_TABLE_HPP_

#include <cstdlib>
#include <vector>
#include "symbol.hpp"
#include "string_table.hpp"

class SymbolTable
{
public:
  SymbolTable() = delete;
  SymbolTable(const SymbolTable& table) = delete;
  SymbolTable& operator=(const SymbolTable& table) = delete;

  static int addSymbol();
private:
  static size_t currentSection;
  static std::vector<Symbol*> table;
  static StringTable* tableOfSymbolString;
};

#endif