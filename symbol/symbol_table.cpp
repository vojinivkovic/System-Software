#include "symbol_table.hpp"
#include "../assembler.hpp"
#include "../aux/exceptions.hpp"
#include "../relocation/relocation_entry.hpp"
#include "../relocation/relocation_table.hpp"
#include <iostream>
#include <fstream>

StringTable* SymbolTable::tableOfSymbolString = new StringTable(StringTable::STType::SymbolName);
std::vector<Symbol*> SymbolTable::table;
Section* SymbolTable::sectionSymbolTable = nullptr;

void SymbolTable::addSymbol(const std::string &name, Symbol *newSymbol)
{
  tableOfSymbolString->addString(name);
  table.push_back(newSymbol); 
}

Symbol *SymbolTable::findSymbol(const std::string &name)
{
  std::string::size_type findIdx = tableOfSymbolString->findString(name);
  //std::cout << "Finding symbol: " << std::to_string(findIdx) << std::endl;
  //std::cout << "Size of table: " << std::to_string(table.size()) << std::endl;
  if(findIdx == std::string::npos)
  {
    return nullptr;
  }

  for(auto iSymbol : table)
  {
    if(iSymbol->getName() == findIdx)
    {

      return iSymbol;
    }
  }
  return nullptr;
}

void SymbolTable::readSymbolTableInELfFile(const std::string &fileName, std::vector<Symbol *> &symTable, StringTable *&tableOfSymbolString)
{
}

Symbol *SymbolTable::findSymbolInSection(const size_t &sectionIdx, const size_t &offset)
{
  for(auto iSymbol : table)
  {
    if((iSymbol->getSection() == sectionIdx) && (iSymbol->getValue() == offset))
    {
      return iSymbol;
    }
  }
  return nullptr;
}

void SymbolTable::removeSymbolFromTable(const Symbol *tempSymbol)
{
  for(auto it = table.begin(); it != table.end();)
  {
    if(*it == tempSymbol)
    {
      table.erase(it);
      break;
    }
    it++;
  }

  // size_t startName = tempSymbol->getName(), endName;

  // std::string namesOfSymbols = tableOfSymbolString->getNames();


  // for(endName = startName; namesOfSymbols[endName] != '\0'; endName++){}
  // endName++;
  // tableOfSymbolString->removeName(startName, endName);

}

bool SymbolTable::checkDefinition(Symbol*& undefinedSymbol)
{
  for(auto iSymbol : table)
  {
    if(!iSymbol->getDefined() && iSymbol->getBinding() != Symbol::Binding::Import)
    {
      undefinedSymbol = iSymbol;
      return false;
    }
  }
  return true;
}
static bool checkIfLoadStore(std::vector<uint8_t> content, size_t offset)
{
  uint8_t firstField = content[offset];
  if(firstField  == 0x80 || firstField == 0x92)
  {
    return true;
  }
  return false;
}
void SymbolTable::resolveForwardReference()
{
  for(auto iSymbol : table)
  {
    std::vector<ForwardReference*> tableOfForwardReference = iSymbol->getForwardReference();
    if(tableOfForwardReference.size() > 0) 
    {
      for(auto forwardReference : tableOfForwardReference) 
      {
        Section* tempSection = Assembler::getSections()[forwardReference->getSection()];
        if(checkIfLoadStore(tempSection->getContent(), forwardReference->getOffset()))
        {
          throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.ld/st] expects symbol which value is known in assembly time",
    forwardReference->getSection(), forwardReference->getOffset()); 
        }
        else
        {
          RelocationEntry* newReloc = new RelocationEntry(forwardReference->getOffset(),
                                                      forwardReference->getSection(), iSymbol->getIdx(), 0);
          RelocationTable::addRelocationEntry(newReloc);
        }
      }
    }
  }
}

void SymbolTable::adjustSymbolValues(const size_t & idxSection, const size_t & offsetOfSymbol, const size_t & shift)
{
  for(auto iSymbol : table)
  {
    if(iSymbol->getSection() == idxSection && iSymbol->getValue() > offsetOfSymbol)
    {
      iSymbol->setValue(iSymbol->getValue() + shift);                                                                                                                                         
    }
  }
}

void SymbolTable::makeSection()
{
  sectionSymbolTable = new Section(".symtable", Section::SectionType::SymTabSection);
  Assembler::addSection(sectionSymbolTable);
  tableOfSymbolString->makeSection(".symstrtab", Section::SectionType::SymStrTabSection);
}

void SymbolTable::addContentInSection()
{
  std::vector<uint8_t> content;
  std::vector<uint8_t> subContent;
  std::string subContentString;
  std::vector<std::string> textContent;

  for(auto iSymbol : table)
  {
    subContent = iSymbol->getLittleEndianFormatOfSymbol();
    content.insert(content.end(), subContent.begin(), subContent.end());
    sectionSymbolTable->incrementLocationCounter(7 * sizeof(size_t));
    subContentString = "Name: " + std::to_string(iSymbol->getName()) +
      ", Size: " + std::to_string(iSymbol->getSize()) + 
      ", Value: " + std::to_string(iSymbol->getValue()) + 
      ", Section: " + std::to_string(iSymbol->getSection()) + 
      ", Bind: " + std::to_string(static_cast<size_t>(iSymbol->getBinding())) + 
      ", Type: " + std::to_string(static_cast<size_t>(iSymbol->getType())) + 
      ", Scope: " + std::to_string(static_cast<size_t>(iSymbol->getScope()));
    
    textContent.push_back(subContentString);
  }

  sectionSymbolTable->setContent(content);
  sectionSymbolTable->setTextContent(textContent);
  tableOfSymbolString->makeContentOfSection();
}

std::string SymbolTable::getNameOfSymbol(const size_t &name)
{
  return tableOfSymbolString->getNameOfElement(name);
}

static size_t readAndConvertFromLittleEndian(const size_t& startIdx, const std::vector<uint8_t>& bytes)
{
  size_t value = 0;
  for(size_t i = 0; i < sizeof(size_t); i++)
  {
    value = (value << 8) | (bytes[startIdx + sizeof(size_t) - 1 - i]);
  }
  return value;
}

static Symbol* readSymbolFromTable(std::ifstream& inputFile)
{
  std::vector<uint8_t> bytes;
  std::string hexByte;
  size_t currIdx = 0, name, size, value, section;
  Symbol::Binding bind;
  Symbol::Type type;
  Symbol::Scope scope;
 

  for(size_t i = 0; i < 7 * sizeof(size_t); i++)
  {
    inputFile >> hexByte;
    bytes.push_back(static_cast<uint8_t>(std::stoul(hexByte, nullptr, 16))); 
  }

  name = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  size = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  value = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  section = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  bind = (Symbol::Binding)readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  type = (Symbol::Type)readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  scope = (Symbol::Scope)readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  return new Symbol(name, size, value, section, bind, type, scope);
}



std::vector<Symbol *> SymbolTable::readSymbolsFromElfFile(const std::string &fileName, const Section *symTable)
{
  // std::cout << std::to_string(symTable->getSizeOfEntry()) << std::endl;
  // std::cout << std::to_string(symTable->getLocationCounter()) << std::endl;
  // std::cout << std::to_string(symTable->getOffsetInFile()) << std::endl;

  std::vector<Symbol*> arrayOfSymbols;
  size_t offset = symTable->getOffsetInFile();
  size_t offsetInFile, numOfRows = offset / 4, numOfCol = offset % 4;
  std::ifstream inputFile(fileName);
  offsetInFile = numOfRows * 13 + numOfCol * 3;
  size_t numOfSymbols = symTable->getLocationCounter() / symTable->getSizeOfEntry();

  inputFile.seekg(offsetInFile);
  for(size_t i = 0; i < numOfSymbols; i++)
  {
    arrayOfSymbols.push_back(readSymbolFromTable(inputFile));
  }
  return arrayOfSymbols;
}
