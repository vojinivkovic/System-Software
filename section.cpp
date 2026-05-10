#include "section.hpp"
#include "assembler.hpp"
#include "symbol/symbol_table.hpp"
#include "aux/instructions.hpp"
#include "aux/auxiliary_func.hpp"
#include "macro/macro_table.hpp"
#include "aux/exceptions.hpp"

StringTable* Section::tableOfSectionString = new StringTable(StringTable::STType::SectionName);

Section::Section(const std::string& sectionName) : name(tableOfSectionString->getOffset()), locationCounter(0), idxSection(Assembler::getNumberOfSections())
{ 
  
  Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, idxSection, Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  SymbolTable::addSymbol(sectionName, newSymbol);
  tableOfSectionString->addString(sectionName);
}


int Section::translateInstruction(const std::string &instruction, const std::vector<Argument> &arguments)
{
  std::vector<uint8_t> binaryInstruction = Instructions::translate(instruction, arguments);
  if(instruction != "iret")
  {
    content.insert(content.end(), binaryInstruction.rbegin(), binaryInstruction.rend());
    locationCounter += 4;
  }
  else
  {
    for(int i = 3; i >= 0; i--)
    {
      content.push_back(binaryInstruction[i]);
    }
    for(int i = 7; i >= 4; i--)
    {
      content.push_back(binaryInstruction[i]);
    }
    locationCounter += 8;
  }
  
  
  return 0;
}

int Section::executeDirective(const std::string &command, const std::vector<MacroParameter>& parameters)
{

  Directives::execute(command, parameters);

  return 0;
}

void Section::defineSymbol(const std::string &symbolName)
{
  Symbol* tempSymol = SymbolTable::findSymbol(symbolName);
  Macro* tempMacro = MacroTable::findMacro(symbolName);

  if(tempMacro)
  {
    throw AssemblerErrors(ErrorType::ErrorSymbolAlreadyDefined, "There is already macro with same name",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
  }
  if(tempSymol)
  {
    if(tempSymol->getDefined())
    {
      throw AssemblerErrors(ErrorType::ErrorSymbolAlreadyDefined, "Symbol is already defined",
        Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
    }
    tempSymol->setValue(locationCounter);
    tempSymol->setSection(idxSection);
    tempSymol->setType(Symbol::Type::Object);
    tempSymol->setScope(Symbol::Scope::Local);
    tempSymol->setDefined();
  }
  else
  {
    Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, locationCounter, idxSection, Symbol::Binding::NoBinding, Symbol::Type::Object, Symbol::Scope::Local, true);
    SymbolTable::addSymbol(symbolName, newSymbol);
  }
  MacroTable::tryToResolveAllMacros(std::vector<std::string>{symbolName});
}

void Section::callocMemory(size_t sizeOfAllocation)
{
  locationCounter += sizeOfAllocation;
  content.insert(content.end(), sizeOfAllocation, 0);
  
}

void Section::insertString(const std::string &stringToInsert)
{
  size_t endOfString = stringToInsert.find("\"", 1);
  locationCounter += endOfString;
  for(size_t i = 1; i < endOfString; i++)
  {
    content.push_back(stringToInsert[i]);
  }
  content.push_back('\0');
}

void Section::insertContent(const uint32_t &value)
{
  locationCounter += 4;
  content.push_back(value & 0xFF);
  content.push_back((value >> 8) & 0xFF);
  content.push_back((value >> 16) & 0xFF);
  content.push_back((value >> 24) & 0xFF);
}
