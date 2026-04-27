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
