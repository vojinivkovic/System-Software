#include "relocation_table.hpp"
#include "../section.hpp"
#include "../assembler.hpp"

std::vector<RelocationEntry*> RelocationTable::table;

void RelocationTable::fixRelocationEntry(size_t idx, size_t newIdxOfSymbol, size_t addend)
{
  table[idx]->setIdxSymbol(newIdxOfSymbol);
  table[idx]->setAddend(addend);
}

void RelocationTable::makeSection()
{
  Section* newSection = new Section(".rela", Section::SectionType::RelaSection);
  std::vector<uint8_t> content;
  std::vector<uint8_t> subContent;
  std::string subContentString;
  std::vector<std::string> textContent;
  for(auto iRelocationEntry: table)
  {
    subContent = iRelocationEntry->getLittleEndianFormatOfRE();
    content.insert(content.end(), subContent.begin(), subContent.end());
    newSection->incrementLocationCounter(4 * sizeof(size_t));
    subContentString = "Offset: " + std::to_string(iRelocationEntry->getOffset()) + 
      ", idxSection: " + std::to_string(iRelocationEntry->getIdxSection()) + 
      ", idxSymbol: " + std::to_string(iRelocationEntry->getIdxSymbol()) + 
      ", Addend: " + std::to_string(iRelocationEntry->getAddend());
    
      textContent.push_back(subContentString);
  }
  newSection->setContent(content);
  newSection->setTextContent(textContent);
  Assembler::addSection(newSection);
}
