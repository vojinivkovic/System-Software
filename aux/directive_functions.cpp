#include "directive_functions.hpp"
#include "../section.hpp"
#include "../assembler.hpp"
#include "../symbol/symbol_table.hpp"
#include <iostream>
#include "auxiliary_func.hpp"


static int getSizeOfMemory(const std::string& argument)
{
  std::string tempString = argument;

  if(tempString[0] == '-')
  {
    return -1; 
  }

  if(tempString[0] == '+')
  {
    tempString = tempString.substr(1);
  }

  if(tempString[1] == 'x' || tempString[1] == 'X')
  {
    tempString = tempString.substr(2);
    return std::stoi(tempString, nullptr, 16);
  }
  else if(tempString[1] == 'b' || tempString[1] == 'B')
  {
    tempString = tempString.substr(2);
    return std::stoi(tempString, nullptr, 2);
  }
  else
  {
    return std::stoi(tempString, nullptr, 10);
  }
}

void directiveSection(const std::string &arguments)
{
  std::string::size_type findName = Section::getStringTable()->findString(arguments);
  if(findName == std::string::npos)
  {
    Section* newSection = new Section(arguments);
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

void directiveGlobal(const std::string& arguments)
{
  std::vector<std::string> listOfArguments = parseArguments(arguments);
  for(auto iArgument : listOfArguments)
  {
    Symbol* tempSymbol = SymbolTable::findSymbol(iArgument);
    if(tempSymbol == nullptr)
    {
      Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, 0, Symbol::Binding::Export, Symbol::Type::NoType, Symbol::Scope::Global, false);
      SymbolTable::addSymbol(iArgument,newSymbol);
    }
    else
    {
      tempSymbol->setBinding(Symbol::Binding::Export);
      tempSymbol->setScope(Symbol::Scope::Global);
    }
  }
}

void directiveExtern(const std::string& arguments)
{
  std::vector<std::string> listOfArguments = parseArguments(arguments);
  for(auto iArgument : listOfArguments)
  {
    Symbol* tempSymbol = SymbolTable::findSymbol(iArgument);
    if(tempSymbol == nullptr)
    {
      Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, 0, Symbol::Binding::Import, Symbol::Type::NoType, Symbol::Scope::Global, false);
      SymbolTable::addSymbol(iArgument, newSymbol);
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


void directiveSkip(const std::string& arguments)
{
  int sizeOfAllocatedMemory = getSizeOfMemory(arguments);
  if(sizeOfAllocatedMemory == -1 )
  {
    std::cout << "Literal can't be negative." << "\n";
    return; 
  }

  Section* currentSection = Assembler::getCurrentSection();
  
  currentSection->callocMemory(sizeOfAllocatedMemory);
}

void directiveAscii(const std::string& arguments)
{
  Section* currentSection = Assembler::getCurrentSection();
  currentSection->insertString(arguments);
}

