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
  static void addSymbol(Symbol* newSymbol) {table.push_back(newSymbol); }
  static StringTable* getTableOfSymbolString() { return tableOfSymbolString; }
  static size_t getSizeOfSymbolTable() { return table.size(); }
  static Symbol* findSymbol(size_t name);

private:
  static size_t currentSection;
  static std::vector<Symbol*> table;
  static StringTable* tableOfSymbolString;
};

#endif