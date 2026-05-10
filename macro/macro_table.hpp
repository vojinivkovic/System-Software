#ifndef _MACRO_TABLE_HPP_
#define _MACRO_TABLE_HPP_
#include <vector>
#include "macro.hpp"
#include "../aux/string_table.hpp"


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

private:
  static std::vector<Macro*> table;
  static StringTable* tableOfMacroString;
};

#endif