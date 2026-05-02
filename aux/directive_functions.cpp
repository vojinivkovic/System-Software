#include "directive_functions.hpp"
#include "../section.hpp"
#include "../assembler.hpp"
#include "../symbol/symbol_table.hpp"
#include <iostream>
#include "auxiliary_func.hpp"
#include "../macro/macro_table.hpp"



void directiveSection(const std::vector<MacroParameter>& parameters)
{
  std::string::size_type findName = Section::findSectionInStringTable(parameters[0].stringValue);
  if(findName == std::string::npos)
  {
    Section* newSection = new Section(parameters[0].stringValue);
    Assembler::addSection(newSection);
    Assembler::setCurrentSection(newSection);
    return;
  }
  else
  {
    Section* oldSection = Assembler::findSection(findName);
    Assembler::setCurrentSection(oldSection);
  }
  return;
}

void directiveGlobal(const std::vector<MacroParameter>& parameters)
{
  //std::vector<std::string> listOfArguments = parseArguments(arguments);
  for(auto iParameter : parameters)
  {
    Symbol* tempSymbol = SymbolTable::findSymbol(iParameter.stringValue);
    if(tempSymbol == nullptr)
    {
      Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, 0, Symbol::Binding::Export, Symbol::Type::NoType, Symbol::Scope::Global, false);
      SymbolTable::addSymbol(iParameter.stringValue,newSymbol);
    }
    else
    {
      tempSymbol->setBinding(Symbol::Binding::Export);
      tempSymbol->setScope(Symbol::Scope::Global);
    }
  }
}

void directiveExtern(const std::vector<MacroParameter>& parameters)
{
  //std::vector<std::string> listOfArguments = parseArguments(arguments);
  for(auto iParameter : parameters)
  {
    Symbol* tempSymbol = SymbolTable::findSymbol(iParameter.stringValue);
    if(tempSymbol == nullptr)
    {
      Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, 0, Symbol::Binding::Import, Symbol::Type::NoType, Symbol::Scope::Global, false);
      SymbolTable::addSymbol(iParameter.stringValue, newSymbol);
    }
    else
    {
      if(tempSymbol->getDefined() || tempSymbol->getScope() == Symbol::Scope::Local || 
          (tempSymbol->getScope() == Symbol::Scope::Global && tempSymbol->getBinding() == Symbol::Binding::Export))
      {
        std::cout << "Error: Symbol is already defined." << "\n";
      }
    }
  }
}

void directiveWord(const std::vector<MacroParameter> &parameters)
{

  uint32_t value;
  Section* currSection = Assembler::getCurrentSection();
  for(auto iParameter: parameters)
  {
    if(iParameter.type == MacroParameterType::Symbol)
    {
      if(Instructions::resolveSymbol(iParameter.stringValue, &value))
      {
        currSection->insertContent(value);
      }
      else
      {
        currSection->insertContent(0);
      }
    }
    else
    {
      currSection->insertContent(iParameter.integerValue);
    }
  }
}

void directiveEqu(const std::vector<MacroParameter> &parameters)
{
  std::string nameOfMacro = parameters[0].stringValue;
  Symbol* tempSymbol = SymbolTable::findSymbol(nameOfMacro);
  std::vector<std::string> dependencies;
  if(MacroTable::findMacro(nameOfMacro))
  {
    std::cout << "Macro is already defined." << std::endl;
    return;
  }
  if(tempSymbol)
  {
    if(tempSymbol->getDefined())
    {
      std::cout << "Symbol with the same name is already defined" << std::endl;
      return;
    }
    SymbolTable::removeSymbolFromTable(tempSymbol);
    for(auto iToken : parameters[0].expression)
    {
      if(iToken.type == TokenType::SYMBOL)
      {
        dependencies.push_back(iToken.symbol);
      }
    }
    Macro* tempMacro = new Macro(MacroTable::getOffsetInTableOfMacroString(), parameters[1].expression,
                                dependencies,tempSymbol->getForwardReference());
  }

}

void directiveSkip(const std::vector<MacroParameter>& parameters)
{
  int sizeOfAllocatedMemory = parameters[0].integerValue;
  if(sizeOfAllocatedMemory == -1 )
  {
    std::cout << "Literal can't be negative." << "\n";
    return; 
  }

  Section* currentSection = Assembler::getCurrentSection();
  
  currentSection->callocMemory(sizeOfAllocatedMemory);
}

void directiveAscii(const std::vector<MacroParameter>& parameters)
{
  Section* currentSection = Assembler::getCurrentSection();
  currentSection->insertString(parameters[0].stringValue);
}

