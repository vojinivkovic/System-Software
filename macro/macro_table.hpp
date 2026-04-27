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
  static Macro* findMacro(const std::string& name);

private:
  static std::vector<Macro*> table;
  static StringTable* tableOfMacroString;
};

#endif