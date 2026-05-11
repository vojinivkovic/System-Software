#include "macro_table.hpp"
#include "../aux/exceptions.hpp"
#include "../assembler.hpp"

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

void MacroTable::tryToResolveAllMacros(const std::vector<std::string>& definedSymbols, bool pureSymbol)
{
  std::vector<std::string> newDefinedSymbols;
  bool ifContains;
  for(auto iMacro : table)
  {
    if(!iMacro->getDefined())
    {
      if(iMacro->checkForResolving(definedSymbols, &ifContains))
      {
        if(pureSymbol && ifContains)
        {
          throw AssemblerErrors(ErrorType::ErrorInvalidSymbolInMacroExpression, "Macro can't symbol that is not macro",
            Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
        }
        iMacro->resolveMacro();
        newDefinedSymbols.push_back(MacroTable::getNameOfMacro(iMacro->getName()));
      }
    }
  }
  if(!newDefinedSymbols.empty())
  {
    tryToResolveAllMacros(newDefinedSymbols, false);
  }
}

std::string MacroTable::getNameOfMacro(size_t name)
{

  return tableOfMacroString->getNameOfElement(name);
}

bool MacroTable::checkDefinition()
{
  for(auto iMacro : table)
  {
    if(!iMacro->getDefined())
    {
      return false;
    }
  }
  return true;
}
