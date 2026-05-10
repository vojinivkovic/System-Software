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

void MacroTable::tryToResolveAllMacros(const std::vector<std::string>& definedSymbols)
{
  std::vector<std::string> newDefinedSymbols;
  for(auto iMacro : table)
  {
    if(!iMacro->getDefined())
    {
      if(iMacro->checkForResolving(definedSymbols))
      {
        iMacro->resolveMacro();
        newDefinedSymbols.push_back(MacroTable::getNameOfMacro(iMacro->getName()));
      }
    }
  }
  if(!newDefinedSymbols.empty())
  {
    tryToResolveAllMacros(newDefinedSymbols);
  }
}

std::string MacroTable::getNameOfMacro(size_t name)
{

  return tableOfMacroString->getNameOfElement(name);
}
