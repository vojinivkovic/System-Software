#include "symbol_table.hpp"

StringTable* SymbolTable::tableOfSymbolString = new StringTable(StringTable::STType::SymbolName);
std::vector<Symbol*> SymbolTable::table; 

Symbol* SymbolTable::findSymbol(size_t name)
{
  for(auto iSymbol : table)
  {
    if(iSymbol->getName() == name)
    {
      return iSymbol;
    }
  }
  return nullptr;
}
