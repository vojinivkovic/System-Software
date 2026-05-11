#include "symbol_table.hpp"

StringTable* SymbolTable::tableOfSymbolString = new StringTable(StringTable::STType::SymbolName);
std::vector<Symbol*> SymbolTable::table;

void SymbolTable::addSymbol(const std::string &name, Symbol *newSymbol)
{
  tableOfSymbolString->addString(name);
  table.push_back(newSymbol); 
}

Symbol *SymbolTable::findSymbol(const std::string &name)
{
  std::string::size_type findIdx = tableOfSymbolString->findString(name);
  if(findIdx == std::string::npos)
  {
    return nullptr;
  }

  for(auto iSymbol : table)
  {
    if(iSymbol->getName() == findIdx)
    {
      return iSymbol;
    }
  }
}

void SymbolTable::removeSymbolFromTable(const Symbol *tempSymbol)
{
  for(auto it = table.begin(); it != table.end();)
  {
    if(*it == tempSymbol)
    {
      table.erase(it);
      break;
    }
    it++;
  }

  size_t startName = tempSymbol->getName(), endName;

  std::string namesOfSymbols = tableOfSymbolString->getNames();


  for(endName = startName; namesOfSymbols[endName] != '\0'; endName++){}
  endName++;
  tableOfSymbolString->removeName(startName, endName);

}

bool SymbolTable::checkDefinition()
{
  for(auto iSymbol : table)
  {
    if(!iSymbol->getDefined())
    {
      return false;
    }
  }
  return true;
}
