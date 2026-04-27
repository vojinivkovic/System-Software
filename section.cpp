#include "section.hpp"
#include "assembler.hpp"
#include "symbol/symbol_table.hpp"
#include "aux/instructions.hpp"
#include "aux/auxiliary_func.hpp"

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

int Section::executeDirective(const std::string &line)
{
  std::string::size_type idxSeparator = line.find("/");
  std::string command = line.substr(0, idxSeparator);
  std::string arguments = line.substr(idxSeparator + 1);

  Directives::execute(command, arguments);

  return 0;
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
