#include "assembler.hpp"
#include "symbol/symbol_table.hpp"
#include "macro/macro_table.hpp"
#include "aux/exceptions.hpp"

Section* Assembler::currentSection = nullptr;
std::vector<Section*> Assembler::arrayOfSections;

Section *Assembler::findSection(size_t nameOfSection)
{
  for(auto iSection: arrayOfSections)
  {
    if(iSection->getSectionName() == nameOfSection)
    {
      return iSection;
    }
  }  
  return nullptr;
}

void Assembler::afterFirstPass()
{
  checkIfSymbolsDefined();
  checkIfMacrosDefined();
  
}

void Assembler::checkIfSymbolsDefined()
{
  if(!SymbolTable::checkDefinition())
  {
    throw AssemblerErrors(ErrorType::ErrorUndefinedSymbol, "Symbol is not defined",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
  }
}

void Assembler::checkIfMacrosDefined()
{
  if(!MacroTable::checkDefinition())
  {
    throw AssemblerErrors(ErrorType::ErrorUndefinedMacro, "Macro is not defined",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  
  }
}
