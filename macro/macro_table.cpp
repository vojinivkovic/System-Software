#include "macro_table.hpp"


std::vector<Macro*> MacroTable::table;
StringTable* MacroTable::tableOfMacroString = new StringTable(StringTable::STType::MacroName);

Macro *MacroTable::findMacro(const std::string &name)
{
  std::string::size_type findIdx = tableOfMacroString->findString(name);
  if(findIdx == std::string::npos)
  {
    return nullptr;
  }

  for(auto iMacro : table)
  {
    if(iMacro->getName() == findIdx)
    {
      return iMacro;
    }
  } 
}

void MacroTable::AddMacro(const std::string &name, Macro *newMacro)
{
  tableOfMacroString->addString(name);
  table.push_back(newMacro);
}
