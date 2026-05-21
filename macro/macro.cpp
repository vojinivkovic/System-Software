#include "macro.hpp"
#include "macro_table.hpp"
#include "../symbol/symbol_table.hpp"
#include "../aux/expression_resolver.hpp"
#include "../aux/exceptions.hpp"
#include "../assembler.hpp"
#include <iostream>


Macro::Macro(const size_t& name_, const std::vector<Token>& expression_, 
      const std::vector<std::string> dependencySymbol_, const std::vector<ForwardReference*>& fReference) : 
    name(name_), expression(expression_), dependencySymbol(dependencySymbol_), tableForwardReference(fReference) 
{
  Macro* tempMacro;
  Symbol* tempSymbol;
  std::cout << "Macro constructor" << std::endl;
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
        throw AssemblerErrors(ErrorType::ErrorSymbolAlreadyDefined, "Symbols is already defined",
        Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
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
    this->resolveMacro();
    MacroTable::tryToResolveAllMacros(std::vector<std::string>{MacroTable::getNameOfMacro(name)}, false);
  }
}

bool Macro::checkForResolving(std::vector<std::string> definedSymbols, bool* ifContains)
{

  for(auto itDefinedSymbols : definedSymbols)
  {
    for(auto itDependencySymbols = dependencySymbol.begin(); itDependencySymbols != dependencySymbol.end(); itDependencySymbols++)
    {
      if(itDefinedSymbols == (*itDependencySymbols))
      {
        *ifContains = true;
        dependencySymbol.erase(itDependencySymbols);
        break;
      }
    }
  }
  return dependencySymbol.empty();

}

void Macro::resolveMacro()
{
  defined = true;
  value = ExpressionResolver::evaluteExpression(expression);
}
