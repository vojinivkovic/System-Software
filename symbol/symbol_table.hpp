#ifndef _SYMBOL_TABLE_HPP_
#define _SYMBOL_TABLE_HPP_

#include <cstdlib>
#include <vector>
#include "symbol.hpp"
#include "../aux/string_table.hpp"

class SymbolTable
{
public:
  SymbolTable() = delete;
  SymbolTable(const SymbolTable& table) = delete;
  SymbolTable& operator=(const SymbolTable& table) = delete;
  static void addSymbol(const std::string& name, Symbol* newSymbol);
  static size_t getOffsetInTableOfSymbolString() { return tableOfSymbolString->getOffset(); }
  static size_t getNewIdxInSymbolTable() { return table.size(); }
  static Symbol* findSymbol(const std::string& name);
  static void readSymbolTableInELfFile(const std::string& fileName, std::vector<Symbol*>& symTable, StringTable*& tableOfSymbolString);
  static Symbol* findSymbolInSection(const size_t& sectionIdx, const size_t& offset);
  static void removeSymbolFromTable(const Symbol* tempSymbol);
  static bool checkDefinition(Symbol*& undefinedSymbol);
  static void resolveForwardReference();
  static std::vector<Symbol*> getSymbolTable() { return table; }
  static void adjustSymbolValues(const size_t& idxSection, const size_t& offsetOfSymbol, const size_t& shift);
  static void makeSection();
  static void addContentInSection();
  static std::string getNameOfSymbol(const size_t& name);
  static std::vector<Symbol*> readSymbolsFromElfFile(const std::string& fileName, const Section* symTable);
  static Symbol* getSymbolBasedOnIdx(const size_t& idx) { return table[idx]; }


private:
  static size_t currentSection;
  static std::vector<Symbol*> table;
  static StringTable* tableOfSymbolString;
  static Section* sectionSymbolTable;
};

#endif