#include "symbol_table.hpp"
#include "../assembler.hpp"
#include "../aux/exceptions.hpp"
#include "../relocation/relocation_entry.hpp"
#include "../relocation/relocation_table.hpp"

StringTable* SymbolTable::tableOfSymbolString = new StringTable(StringTable::STType::SymbolName);
std::vector<Symbol*> SymbolTable::table;

void SymbolTable::addSymbol(const std::string &name, Symbol *newSymbol)
{
  tableOfSymbolString->addString(name);
  table.push_back(newSymbol); 
}

Symbol *SymbolTable::findSymbol(const std::string &name)
{
  std::string::size_type findIdx = tableOfSymbolString->findString(name);
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

  size_t startName = tempSymbol->getName(), endName;

  std::string namesOfSymbols = tableOfSymbolString->getNames();


  for(endName = startName; namesOfSymbols[endName] != '\0'; endName++){}
  endName++;
  tableOfSymbolString->removeName(startName, endName);

}

bool SymbolTable::checkDefinition()
{
  for(auto iSymbol : table)
  {
    if(!iSymbol->getDefined() && iSymbol->getBinding() != Symbol::Binding::Import)
    {
      return false;
    }
  }
  return true;
}
static bool checkIfLoadStore(std::vector<uint8_t> content, size_t offset)
{
  uint8_t firstField = content[offset + 3];
  if(((firstField >> 4) & 0x0F) == 0x8 || ((firstField >> 4) & 0x0F) == 0x9)
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

void SymbolTable::makeSection()
{
  Section* newSection = new Section(".symtable", Section::SectionType::SymTabSection);
  std::vector<uint8_t> content;
  std::vector<uint8_t> subContent;
  std::string subContentString;
  std::vector<std::string> textContent;

  for(auto iSymbol : table)
  {
    subContent = iSymbol->getLittleEndianFormatOfSymbol();
    content.insert(content.end(), subContent.begin(), subContent.end());
    newSection->incrementLocationCounter(7 * sizeof(size_t));
    subContentString = "Name: " + std::to_string(iSymbol->getName()) +
      ", Size: " + std::to_string(iSymbol->getSize()) + 
      ", Value: " + std::to_string(iSymbol->getValue()) + 
      ", Section: " + std::to_string(iSymbol->getSection()) + 
      ", Bind: " + std::to_string(static_cast<size_t>(iSymbol->getBinding())) + 
      ", Type: " + std::to_string(static_cast<size_t>(iSymbol->getType())) + 
      ", Scope: " + std::to_string(static_cast<size_t>(iSymbol->getScope()));
    
    textContent.push_back(subContentString);
  }

  newSection->setContent(content);
  newSection->setTextContent(textContent);
  Assembler::addSection(newSection);

  tableOfSymbolString->makeSection(".symstrtab", Section::SectionType::SymStrTabSection);
}
