#include "section.hpp"
#include "assembler.hpp"
#include "symbol_table.hpp"

StringTable* Section::tableOfSectionString = new StringTable(StringTable::STType::SectionName);

Section::Section(const std::string& sectionName) : name(tableOfSectionString->getOffset()), locationCounter(0), idxSection(Assembler::getNumberOfSections())
{ 
  StringTable* table = SymbolTable::getTableOfSymbolString();
  Symbol* newSymbol = new Symbol(SymbolTable::getSizeOfSymbolTable(), table->getOffset(), 0, 0, idxSection, Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  table->addString(sectionName);
  SymbolTable::addSymbol(newSymbol);
  tableOfSectionString->addString(sectionName);
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
