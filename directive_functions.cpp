#include "directive_functions.hpp"
#include "section.hpp"
#include "assembler.hpp"
#include "symbol_table.hpp"
#include <iostream>
#include "auxiliary.hpp"


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

void directive_section(const std::string &arguments)
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

void directive_global(const std::string& arguments)
{
  std::vector<std::string> listOfArguments = parseArguments(arguments);
  for(auto iArgument : listOfArguments)
  {
    std::string::size_type findName = SymbolTable::getTableOfSymbolString()->findString(iArgument);
    if(findName == std::string::npos)
    {
      StringTable* table = SymbolTable::getTableOfSymbolString();
      Symbol* newSymbol = new Symbol(SymbolTable::getSizeOfSymbolTable(), table->getOffset(), 0, 0, 0, Symbol::Binding::Export, Symbol::Type::NoType, Symbol::Scope::Global, false);
      table->addString(iArgument);
      SymbolTable::addSymbol(newSymbol);
    }
    else
    {
      Symbol* tempSymbol = SymbolTable::findSymbol(findName);
      tempSymbol->setBinding(Symbol::Binding::Export);
      tempSymbol->setScope(Symbol::Scope::Global);
    }
  }
}

void directive_extern(const std::string& arguments)
{
  std::vector<std::string> listOfArguments = parseArguments(arguments);
  for(auto iArgument : listOfArguments)
  {
    std::string::size_type findName = SymbolTable::getTableOfSymbolString()->findString(iArgument);
    if(findName == std::string::npos)
    {
      StringTable* table = SymbolTable::getTableOfSymbolString();
      Symbol* newSymbol = new Symbol(SymbolTable::getSizeOfSymbolTable(), table->getOffset(), 0, 0, 0, Symbol::Binding::Import, Symbol::Type::NoType, Symbol::Scope::Global, false);
      table->addString(iArgument);
      SymbolTable::addSymbol(newSymbol);
    }
    else
    {
      Symbol* tempSymbol = SymbolTable::findSymbol(findName);
      if(tempSymbol->getDefined() || tempSymbol->getScope() == Symbol::Scope::Local || 
          (tempSymbol->getScope() == Symbol::Scope::Global && tempSymbol->getBinding() == Symbol::Binding::Export))
      {
        std::cout << "Error: Symbol is already defined." << "\n";
      }
    }
  }
}


void directive_skip(const std::string& arguments)
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

void directive_ascii(const std::string& arguments)
{
  Section* currentSection = Assembler::getCurrentSection();
  currentSection->insertString(arguments);
}

