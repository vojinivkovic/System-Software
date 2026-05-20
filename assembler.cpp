#include "assembler.hpp"
#include "symbol/symbol_table.hpp"
#include "macro/macro_table.hpp"
#include "aux/exceptions.hpp"
#include "relocation/relocation_table.hpp"
#include "elf_header.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

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
  SymbolTable::addContentInSection();
  Section::makeContentOfSectionsNames(); 
}

void Assembler::makeELFFiles(const std::string &name)
{
  size_t sizeOfFile = 0;
  std::vector<uint8_t> binaryContent;
  std::vector<std::string> textContent;

  std::cout << "Making Elf files" << std::endl;

  for(auto iSection : arrayOfSections)
  {

    iSection->setOffsetInFile(sizeOfFile);
    std::vector<uint8_t> tempContent = iSection->getContent();
    if(!tempContent.empty())
    {
      // for(size_t i = 0; i < tempContent.size(); i += 4)
      // {
      //   std::reverse(tempContent.begin() + i, tempContent.begin() + i + 4);
      // }
      binaryContent.insert(binaryContent.end(), tempContent.begin(), tempContent.end());
      std::vector<std::string> tempStringContent = iSection->getTextContent();
      if(!tempStringContent.empty())
      {
        textContent.insert(textContent.end(), tempStringContent.begin(), tempStringContent.end());
      }
      
      sizeOfFile += iSection->getLocationCounter();
    }
    
  }

  ELFHeader::makeELFHeader(ELFHeader::ELFHeaderType::ELF_REL, 0, 0, sizeOfFile, 0, 0, 
  6 * sizeof(size_t), arrayOfSections.size(), arrayOfSections.size() - 1);

  for(auto iSection : arrayOfSections) 
  {
    std::vector<uint8_t> tempContentSec = iSection->getLittleEndiandOfSection();
    binaryContent.insert(binaryContent.end(), tempContentSec.begin(), tempContentSec.end());
    textContent.push_back(iSection->getTextFormatOfSection());
  }
  

  std::vector<uint8_t> elfHeaderBinaryContent = ELFHeader::getLittleEndianFormat();
  binaryContent.insert(binaryContent.begin(), elfHeaderBinaryContent.begin(), elfHeaderBinaryContent.end());
  textContent.insert(textContent.begin(), ELFHeader::getStringFormat());
  textContent.push_back(MacroTable::getMacroNames());
  std::vector<std::string> textRepresentationMacroTable = MacroTable::getTextRepresentationOfMacros();
  textContent.insert(textContent.end(), textRepresentationMacroTable.begin(), textRepresentationMacroTable.end());

  std::ofstream outBin(name);
  outBin << std::hex << std::uppercase << std::setfill('0');

  for(size_t i = 0; i < binaryContent.size(); i++)
  {
    outBin << std::setw(2)
        << static_cast<int>(binaryContent[i])
        << ' ';

    if ((i + 1) % 4 == 0)
        outBin << '\n';
  }
  std::ofstream outText(name.substr(0, name.size() - 2) + ".txt");
  for(size_t i = 0; i < textContent.size(); i++)
  {
    outText << textContent[i] << '\n';
  }
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
