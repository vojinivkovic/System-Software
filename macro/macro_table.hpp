#ifndef _MACRO_TABLE_HPP_
#define _MACRO_TABLE_HPP_
#include <vector>
#include "macro.hpp"
#include "../aux/string_table.hpp"
#include <unordered_map>

class MacroTable 
{
public:
  MacroTable() = delete;
  MacroTable(const MacroTable& table) = delete;
  MacroTable& operator=(const MacroTable& table) = delete;
  static size_t getOffsetInTableOfMacroString() { return tableOfMacroString->getOffset(); }
  static Macro* findMacro(const std::string& name);
  static void AddMacro(const std::string& name, Macro* newMacro);
  static void tryToResolveAllMacros(const std::vector<std::string>& definedSymbols);
  static std::string getNameOfMacro(size_t name);
  static bool checkDefinition(Macro*& undefinedMacro);
  static size_t getSizeOfMacroTable() { return table.size(); }
  static void resolveForwardReference();
  static std::string getMacroNames() { return tableOfMacroString->getNames(); }
  static std::vector<std::string> getTextRepresentationOfMacros();
  static void addMapping(const size_t& idxMacro, const size_t& idxSymbol);
  static void setValuesOfSymbols();

private:
  static std::vector<Macro*> table;
  static std::unordered_map<size_t, size_t> mappingMacroToSymbol;
  static StringTable* tableOfMacroString;
};

#endif