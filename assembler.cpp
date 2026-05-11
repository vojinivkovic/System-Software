#include "assembler.hpp"
#include "symbol/symbol_table.hpp"
#include "macro/macro_table.hpp"
#include "aux/exceptions.hpp"
#include "relocation/relocation_table.hpp"

Section* Assembler::currentSection = nullptr;
std::vector<Section*> Assembler::arrayOfSections;

void Assembler::initializeAssembler()
{
  Section* newSection = new Section("UND");
  Assembler::addSection(newSection);
  Assembler::setCurrentSection(newSection);
}

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
  SymbolTable::resolveForwardReference();
  MacroTable::resolveForwardReference();

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

void Assembler::fixRelocationEntries()
{
  std::vector<RelocationEntry*> tableOfRelocationEntries = RelocationTable::getTable();
  std::vector<Symbol*> tableOfSymbols = SymbolTable::getSymbolTable();
  for(size_t i = 0; i < tableOfRelocationEntries.size(); i++)
  {
    Symbol* tempSymbol = tableOfSymbols[tableOfRelocationEntries[i]->getIdxSymbol()];
    if(tempSymbol->getScope() == Symbol::Scope::Local)
    {
      Section* tempSection = Assembler::getSections()[tempSymbol->getSection()];
      std::string nameOfSection = Section::getTableOfSectionStrings()->getNameOfElement(tempSection->getSectionName());
      Symbol* tempSectionSymbol = SymbolTable::findSymbol(nameOfSection);
      RelocationTable::fixRelocationEntry(i, tempSectionSymbol->getIdx(), tempSymbol->getValue());
    }
  }
}
