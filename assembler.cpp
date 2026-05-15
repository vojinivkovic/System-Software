#include "assembler.hpp"
#include "symbol/symbol_table.hpp"
#include "macro/macro_table.hpp"
#include "aux/exceptions.hpp"
#include "relocation/relocation_table.hpp"
#include "elf_header.hpp"

Section* Assembler::currentSection = nullptr;
std::vector<Section*> Assembler::arrayOfSections;

void Assembler::initializeAssembler()
{
  Section* newSection = new Section(".UND", Section::SectionType::NullSection);
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
  fixRelocationEntries();
  RelocationTable::makeSection();
  SymbolTable::makeSection();
  Section::makeSectionOfSectionNames();
}

void Assembler::makeELFFiles(const std::string &names)
{
  size_t sizeOfFile = 0;
  std::vector<uint8_t> binaryContent;
  std::vector<std::string> textContent;

  

  for(auto iSection : arrayOfSections)
  {
    iSection->setOffsetInFile(sizeOfFile);
    binaryContent.insert(binaryContent.end(), iSection->getContent().begin(), iSection->getContent().end());
    textContent.insert(textContent.end(), iSection->getTextContent().begin(), iSection->getTextContent().end());
    sizeOfFile += iSection->getLocationCounter();
  }

  ELFHeader::makeELFHeader(ELFHeader::ELFHeaderType::ELF_REL, 0, 0, sizeOfFile, 0, 0, 
  6 * sizeof(size_t), arrayOfSections.size(), arrayOfSections.size() - 1);


  for(auto iSection : arrayOfSections) 
  {
    binaryContent.insert(binaryContent.end(), iSection->getLittleEndiandOfSection().begin(), iSection->getLittleEndiandOfSection().end());
    textContent.push_back(iSection->getTextFormatOfSection());
  }
  binaryContent.insert(binaryContent.begin(), ELFHeader::getLittleEndianFormat().begin(), ELFHeader::getLittleEndianFormat().end());
  textContent.insert(textContent.begin(), ELFHeader::getStringFormat());

  // make files with content
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
