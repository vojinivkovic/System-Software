#include "macro.hpp"
#include "macro_table.hpp"
#include "../symbol/symbol_table.hpp"
#include "../aux/expression_resolver.hpp"
#include <iostream>


Macro::Macro(const size_t& name_, const std::vector<Token>& expression_, 
      const std::vector<std::string> dependencySymbol_, const std::vector<ForwardReference*>& fReference) : 
    name(name_), expression(expression_), dependencySymbol(dependencySymbol_), tableForwardReference(fReference) 
{
  Macro* tempMacro;
  Symbol* tempSymbol;

  for(auto it = dependencySymbol.begin() ; it != dependencySymbol.end();)
  {
    tempMacro = MacroTable::findMacro(*it);
    if(tempMacro && tempMacro->defined)
    {
      it = dependencySymbol.erase(it);
      continue;
    }
    else
    {
      tempSymbol = SymbolTable::findSymbol(*it);
      if(tempSymbol && tempSymbol->getDefined())
      {
        std::cout << "In .EQU directive symbols are not acceptable" << std::endl;
        return;
      }
    }
    it++;
  }

  if(!dependencySymbol.empty())
  {
    defined = false;
  }
  else
  {
    defined = true;
    this->resolveMacro();
  }
}

void Macro::resolveMacro()
{
  value = ExpressionResolver::evaluteExpression(expression);
}
