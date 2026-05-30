#include "linker.hpp"
#include "elf_header.hpp"
#include "section.hpp"
#include <iostream>
#include <fstream>
#include "relocation/relocation_table.hpp"
#include "aux/instructions_functions.hpp"
#include <algorithm>

std::vector<StringTable*> Linker::arrayOfSymbolStringsTables;
std::vector<StringTable*> Linker::arrayOfSectionStringTables;
std::vector<std::vector<Section*>> Linker::arrayOfFilesSections;
std::vector<std::vector<Symbol*>> Linker::arrayOfSymbolTables;
std::vector<std::vector<RelocationEntry*>> Linker::arrayOfRelocationEntryTables;
std::vector<Section*> Linker::linkerSections;
std::vector<Symbol*> Linker::linkerSymbols;
std::vector<std::string> Linker::files;
std::map<std::pair<size_t, size_t>, std::pair<size_t, size_t>> Linker::mappingFileSectionToSectionOffset;
std::map<std::pair<size_t, size_t>, size_t> Linker::mappingOfSymbols;
StringTable* Linker::sectionStringTable = new StringTable(StringTable::STType::SectionName);
StringTable* Linker::symbolStringTable = new StringTable(StringTable::STType::SymbolName);;
std::unordered_map<std::string, uint32_t> Linker::placeMapping;

static StringTable* readSectionStringTable(const std::string& fileName, const Section* sectionStringTable, const StringTable::STType& type)
{
  std::ifstream inputFile(fileName);
  size_t offset = sectionStringTable->getOffsetInFile();
  size_t offsetInFile, numOfRows = offset / 4, numOfCol = offset % 4;
  size_t size = sectionStringTable->getLocationCounter();
  char tempChar;
  StringTable* tempStringTable;

  offsetInFile = numOfRows * 13 + numOfCol;
  std::string sectionNames;
  inputFile.seekg(offsetInFile);

  for(size_t i = 0; i < size; i++)
  {
    inputFile >> tempChar;
    sectionNames.push_back(tempChar);
  }
  tempStringTable = new StringTable(type, sectionNames);
  return tempStringTable;
}

static void findSymbolTables(Section*& symbolStringTable, Section*& symbolTable, 
  const StringTable* sectionStringTable, const std::vector<Section*>& sections)
{
  size_t nameSymbolTable = sectionStringTable->findString(".symtable");
  size_t nameSymbolStringTable = sectionStringTable->findString(".symstrtab");

  for(auto iSection : sections)
  {
    if(iSection->getSectionName() == nameSymbolTable)
    {
      symbolTable = iSection;
      break;
    }
  }

  for(auto iSection : sections)
  {
    if(iSection->getSectionName() == nameSymbolStringTable)
    {
      symbolStringTable = iSection;
      break;
    }
  }
}
static void findRelocationTableSection(const StringTable* sectionStringTable, const std::vector<Section*>& sections, Section*& relocationTableSection)
{
  size_t nameRelocationTable = sectionStringTable->findString(".rela");

  for(auto iSection : sections)
  {
    if(iSection->getSectionName() == nameRelocationTable)
    {
      relocationTableSection = iSection;
      break;
    }
  }
}
void Linker::readElfFile(const std::string &fileName)
{
  ELFHeader::ELFHeaderType elfType; 
  size_t entry, phoff, shoff, phentsize, phnum, shentsize, shnum, shstrndx;
  std::vector<Section*> tableOfSections;
  std::vector<Symbol*> tableOfSymbols;
  std::vector<RelocationEntry*> tableOfRelocationTables;
  StringTable* tempSectionStringTable, *tempSymbolStringTable;
  Section* symbolTable, *symbolStringTable, *relocationTable;

  ELFHeader::readElfHeader(fileName, elfType, entry, phoff, shoff, phentsize, phnum, shentsize, shnum, shstrndx);
  tableOfSections = Section::readSectionHeader(fileName, shoff, shnum);
  tempSectionStringTable = readSectionStringTable(fileName, tableOfSections[shstrndx], StringTable::STType::SectionName);

  findSymbolTables(symbolStringTable, symbolTable, tempSectionStringTable, tableOfSections);
  findRelocationTableSection(tempSectionStringTable, tableOfSections, relocationTable);

  tempSymbolStringTable = readSectionStringTable(fileName, symbolStringTable, StringTable::STType::SymbolName);

  tableOfSymbols = SymbolTable::readSymbolsFromElfFile(fileName, symbolTable);
  tableOfRelocationTables = RelocationTable::readRelocationTableFromElfFile(fileName, relocationTable);

  //SORT RELOCATION ENTRIES BASED ON OFFSET
    std::sort(tableOfRelocationTables.begin(), tableOfRelocationTables.end(),
    [](const RelocationEntry* a, const RelocationEntry* b)
    {
        return a->getOffset() < b->getOffset();
    }
    );


}

static void checkDefinitionOfTheSymbolInSymbolTable(const Symbol* sym, const std::string& symbolName, 
  const std::vector<Symbol*>& symTable, const StringTable* symbolStringTable)
{
  if(sym->getScope() == Symbol::Scope::Global && sym->getBinding() == Symbol::Binding::Export)
  {
    for(size_t i = 0; i < symTable.size(); i++)
    {
      std::string tempSymbolName = symbolStringTable->getNameOfElement(symTable[i]->getName());

      if(tempSymbolName == symbolName && symTable[i]->getBinding() == Symbol::Binding::Export && 
      symTable[i]->getScope() == Symbol::Scope::Global)
      {
        throw LinkerErrors(ErrorType::ErrorMultipleDefinitions, "Symbol [" + symbolName + "] is globaly defined multiple times");
      }
    }
  }
  return;
}

void Linker::checkMultipleDefinitions()
{
  std::vector<Symbol*> tempSymbolTable;
  StringTable* tempSymbolStringTable;
  for(size_t i = 0; i < arrayOfSymbolTables.size(); i++)
  {
    tempSymbolTable = arrayOfSymbolTables[i];
    tempSymbolStringTable = arrayOfSymbolStringsTables[i];
    for(size_t j = 0; j < tempSymbolTable.size(); j++)
    {
      std::string symbolName = tempSymbolStringTable->getNameOfElement(tempSymbolTable[j]->getName());
      for(size_t k = 0; k < arrayOfSymbolTables.size(); k++)
      {
        if(i == k)
        {
          continue;
        }
        else
        {
          checkDefinitionOfTheSymbolInSymbolTable(tempSymbolTable[j], symbolName, 
            arrayOfSymbolTables[k], arrayOfSymbolStringsTables[k]);
        }
      }
    }
  }
}

static bool findExportedSymbol(const std::string& symName, const std::vector<Symbol*>& symTable, const StringTable* symbolStringTable)
{
  for(int i = 0; i < symTable.size(); i++)
  {
    if(symName == symbolStringTable->getNameOfElement(symTable[i]->getName()))
    {
      if(symTable[i]->getBinding() == Symbol::Binding::Export && symTable[i]->getScope() == Symbol::Scope::Global)
      {
        return true;
      }
    }
  }
  return false;
}

void Linker::checkUnresolvedSymbols()
{
  std::vector<Symbol*> tempSymbolTable;
  StringTable* tempSymbolStringTable;
  bool defined;
  for(size_t i = 0; i < arrayOfSymbolTables.size(); i++)
  {
    tempSymbolTable = arrayOfSymbolTables[i];
    tempSymbolStringTable = arrayOfSymbolStringsTables[i];
    for(size_t j = 0; j < tempSymbolTable.size(); j++)
    {
      std::string symbolName = tempSymbolStringTable->getNameOfElement(tempSymbolTable[j]->getName());
      defined = false;
      if(tempSymbolTable[j]->getBinding() == Symbol::Binding::Import && tempSymbolTable[j]->getScope() == Symbol::Scope::Global)
      {
        for(size_t k = 0; k < arrayOfSymbolTables.size(); k++)
        {
          if(i == k)
          {
            continue;
          }
          else
          {
            defined = defined ? defined : findExportedSymbol(symbolName, arrayOfSymbolTables[k], arrayOfSymbolStringsTables[k]);
            if(defined)
            {
              break;
            }
          }
        }
        if(!defined)
        {
          throw LinkerErrors(ErrorType::ErrorUnresolvedSymbol, "Symbol [" + symbolName + "] is not resolved");
      
        }
      }
      
    }
  }
}
std::vector<uint8_t> getContentOfSection(const std::string& fileName, const size_t& offset, const size_t& size)
{
  size_t offsetInFile, numOfRows = offset / 4, numOfCol = offset % 4;
  std::ifstream inputFile(fileName);
  offsetInFile = numOfRows * 13  + numOfCol;
  inputFile.seekg(offsetInFile);
  std::string hexByte;
  std::vector<uint8_t> content;

  for(size_t i = 0; i < size; i++)
  {
    inputFile >> hexByte;
    content.push_back(static_cast<uint8_t>(std::stoul(hexByte, nullptr, 16)));
  }
  return content;
}
static size_t findSection(const size_t& name, std::vector<Section*> arrayOfSections)
{
  for(size_t i = 0; i < arrayOfSections.size(); i++)
  {
    if(arrayOfSections[i]->getSectionName() == name)
    {
      return i;
    }
  }
}
void Linker::makeLinkerSections()
{
  std::vector<Section*> tempSections;
  StringTable* sectionStringTable; 
  std::vector<uint8_t> tempContent;
  std::string tempSectionName;
  size_t newSectionName;
  Section* newSection, *oldSection;
  Symbol* newSymbol;
  size_t idxSection;


  //dodati .UND sekciju


  for(size_t i = 0; i < arrayOfFilesSections.size(); i++)
  {
    tempSections = arrayOfFilesSections[i];
    sectionStringTable = arrayOfSectionStringTables[i];

    for(int j = 0; j < tempSections.size(); j++)
    {
      tempContent = getContentOfSection(files[i], tempSections[j]->getOffsetInFile(), tempSections[j]->getLocationCounter());

      tempSectionName = sectionStringTable->getNameOfElement(tempSections[j]->getSectionName());
      newSectionName = sectionStringTable->findString(tempSectionName);

      if(!tempContent.size() || tempSectionName == ".rela" || 
      tempSectionName == ".symtable" || tempSectionName == ".symstrtab" 
      || tempSectionName == ".secstrtab")
      {
        continue;
      }
      if(!newSectionName)
      {
        
        newSection = new Section(sectionStringTable->getOffset(), tempSections[j]->getSectionType(), 0, 0, tempSections[j]->getLocationCounter(), tempSections[j]->getSizeOfEntry(), linkerSections.size());
        newSection->setContent(tempContent);
        mappingFileSectionToSectionOffset[{i, j}] = {linkerSections.size(), 0};

        Symbol* newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), 0, 0, linkerSections.size(), Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
        symbolStringTable->addString(tempSectionName);
        linkerSymbols.push_back(newSymbol);

        sectionStringTable->addString(tempSectionName);
        linkerSections.push_back(newSection);      
      }
      else
      {
        idxSection = findSection(newSectionName, linkerSections);
        oldSection = linkerSections[idxSection];
        mappingFileSectionToSectionOffset[{i, j}] = {idxSection, oldSection->getLocationCounter()};
        oldSection->addContent(tempContent);
      }
    }

  }
}

void Linker::fixRelocationEntries()
{
  std::vector<RelocationEntry*> tempRelocationTable;
  std::vector<Symbol*> tempSymbolTable;
  std::vector<Section*> tempSectionTable;
  StringTable* tempSymbolStringTable, *tempSectionStringTable;
  Symbol* tempSymbol;
  Section* tempSection, *newSection;
  
  for(int i = 0; i < arrayOfRelocationEntryTables.size(); i++)
  {
    tempRelocationTable = arrayOfRelocationEntryTables[i];
    tempSymbolTable = arrayOfSymbolTables[i];
    tempSectionTable = arrayOfFilesSections[i];
    tempSymbolStringTable = arrayOfSymbolStringsTables[i];
    tempSectionStringTable = arrayOfSectionStringTables[i];

    for(int j = 0; j < tempRelocationTable.size(); j++)
    {
      tempSymbol = tempSymbolTable[tempRelocationTable[j]->getIdxSymbol()];
      tempSection = tempSectionTable[tempRelocationTable[j]->getIdxSection()];

      std::pair<size_t, size_t> sectionAndOffset = mappingFileSectionToSectionOffset[{i, tempSection->getIdxOfSection()}];
      size_t startOffset = sectionAndOffset.second;
      size_t offsetOfRelocation = startOffset + tempRelocationTable[j]->getOffset();
      newSection = linkerSections[sectionAndOffset.first];
      std::vector<uint8_t> tempContent = newSection->getContent();
      
      uint8_t destReg = tempContent[offsetOfRelocation + 2];
      destReg >>= 4;
      size_t value;
      

      if(tempSymbol->getDefined())
      {

        std::pair<size_t, size_t> symbolSectionAndOffset = mappingFileSectionToSectionOffset[{i, tempSymbol->getSection()}];
        Section* newSymbolSection = linkerSections[symbolSectionAndOffset.first];
        value = newSymbolSection->getVirtualAddress() + symbolSectionAndOffset.second + tempSymbol->getValue();

        std::vector<uint8_t> newContent = transformLoadInstruction(destReg, value);

        tempContent.erase(tempContent.begin() + offsetOfRelocation, tempContent.begin() + offsetOfRelocation + 4);
        tempContent.insert(tempContent.begin() + offsetOfRelocation, newContent.begin(), newContent.end());
        newSection->setContent(tempContent);

      }
      else
      {
        std::string symbolName = tempSymbolStringTable->getNameOfElement(tempSymbol->getName());
        value = findValueOfSymbol(i, symbolName);
        std::vector<uint8_t> newContent = transformLoadInstruction(destReg, value);

        for(size_t i = 0; i < newContent.size(); i += 4)
        {
          std::reverse(newContent.begin() + i, newContent.begin() + i + 4);
        }

        tempContent.erase(tempContent.begin() + offsetOfRelocation, tempContent.begin() + offsetOfRelocation + 4);
        tempContent.insert(tempContent.begin() + offsetOfRelocation, newContent.begin(), newContent.end());
        newSection->setContent(tempContent);

      }

    }
  }
}

void Linker::makeLinkerSymbolTable()
{
}

void Linker::makeLinkerRelocationEntries()
{
}

void Linker::addSectionMapping(const std::string &secionName, const uint32_t &memAddress)
{
  placeMapping[secionName] = memAddress;
}

void Linker::fixLinkerSymbolTable(const Symbol* symbol, const size_t& symbolName, const size_t& idxFile, const size_t& idxSymbol)
{
  Symbol* linkerSymbol;
  if(!symbol->getDefined())
  {
    return;
  }

  for(size_t i = 0; i < linkerSymbols.size(); i++)
  {
    if(linkerSymbols[i]->getName() == symbolName)
    {
      linkerSymbol = linkerSymbols[i];
    }
  }

  std::pair<size_t, size_t> sectionAndOffset = mappingFileSectionToSectionOffset[{idxFile, symbol->getSection()}];
  linkerSymbol->setSize(symbol->getSize());
  linkerSymbol->setValue(sectionAndOffset.second + symbol->getValue());
  linkerSymbol->setSection(sectionAndOffset.first);
  linkerSymbol->setBinding(Symbol::Binding::Export);
  linkerSymbol->setType(symbol->getType());
  linkerSymbol->setScope(Symbol::Scope::Global);
  linkerSymbol->setDefined();
  mappingOfSymbols[{idxFile, idxSymbol}] = linkerSymbol->getIdx();

}
void Linker::makeLinkersSymbolTable()
{
  std::vector<Symbol*> tempSymbolTable;
  StringTable* tempSymbolStringTable;
  Symbol* tempSymbol;

  for(size_t i = 0; i < arrayOfSymbolTables.size(); i++)
  {
    tempSymbolTable = arrayOfSymbolTables[i];
    tempSymbolStringTable = arrayOfSymbolStringsTables[i];
    for(size_t j = 0; j < tempSymbolTable.size(); j++)
    { 
      tempSymbol = tempSymbolTable[j];
      if(tempSymbol->getType() == Symbol::Type::Section)
      {
        continue;
      }
      std::string symName = tempSymbolStringTable->getNameOfElement(tempSymbol->getName());
      size_t symNameInLinker = symbolStringTable->findString(symName);
      if(tempSymbol->getScope() == Symbol::Scope::Local)
      {
        //dodavanje lokalnog simbola
        std::string newSymbolName = files[i] + "::" + symName;
        std::pair<size_t, size_t> sectionAndOffset = mappingFileSectionToSectionOffset[{i, tempSymbol->getSection()}];
        size_t newValue;

        newValue = sectionAndOffset.second + tempSymbol->getValue();        

        Symbol* newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), tempSymbol->getSize(), newValue, sectionAndOffset.first, Symbol::Binding::NoBinding, tempSymbol->getType(),  Symbol::Scope::Local, true);
        symbolStringTable->addString(newSymbolName);
        linkerSymbols.push_back(newSymbol); 
        mappingOfSymbols[{i, j}] = newSymbol->getIdx();
      }
      if(symNameInLinker)
      {
        fixLinkerSymbolTable(tempSymbol, symNameInLinker, i, j);
      }
      else
      {
        Symbol* newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), 0, 0, 0, Symbol::Binding::Import, Symbol::Type::NoType, Symbol::Scope::Global, false);
        symbolStringTable->addString(symName);
        linkerSymbols.push_back(newSymbol); 
        mappingOfSymbols[{i, j}] = newSymbol->getIdx();
      }
    }
  }
}

static void fixRelocationTable(std::vector<std::vector<RelocationEntry *>> &array, const size_t &numOfTable, const size_t &numOfEntry)
{
  for(size_t i = numOfEntry + 1; i < array[numOfTable].size(); i++)
  {
    array[numOfTable][i]->setOffset(array[numOfTable][i]->getOffset() + 40);
  }
}
static void fixSymbolTable(std::vector<std::vector<Symbol*>>& array, const size_t& numOfTable, const size_t& offset, const size_t& idxSection)
{
  for(size_t i = 0; i < array[numOfTable].size(); i++)
  {
    if(array[numOfTable][i]->getSection() == idxSection && array[numOfTable][i]->getValue() > offset)
    {
      array[numOfTable][i]->setValue(array[numOfTable][i]->getValue() + 40);
    }
  }
}

void Linker::fixVirtualAddressOfSections()
{
  uint32_t currentVirtualAddress = 0, newVirtualAddress;
  Section* tempSection;
  std::string sectionName;

  for(size_t i = 0; i < linkerSections.size(); i++)
  {
    tempSection = linkerSections[i];
    sectionName = sectionStringTable->getNameOfElement(tempSection->getSectionName());

    auto it = placeMapping.find(sectionName);

    if(it != placeMapping.end())
    {
      newVirtualAddress = it->second;
      if(currentVirtualAddress > newVirtualAddress)
      {
        throw LinkerErrors(ErrorType::ErrorOverlappingSections, "Section [" + sectionName + "] can't be placed ad define address");
      }
      tempSection->setVirtualAddress(newVirtualAddress);
      currentVirtualAddress = newVirtualAddress;
    }
    else
    {
      tempSection->setVirtualAddress(currentVirtualAddress);
    }
    currentVirtualAddress += tempSection->getLocationCounter();

  }
}

void Linker::addOffsetToSections(std::map<std::pair<size_t, size_t>, std::pair<size_t, size_t>> &map,
                                 const size_t &idxFile, const size_t &idxSection)
{
  std::pair<size_t, size_t> sectionAndOffset = map[{idxFile, idxSection}];
  for(auto& entry : map)
  {
      const std::pair<size_t, size_t>& key = entry.first;
      const std::pair<size_t, size_t>& value = entry.second;
      if(value.first == sectionAndOffset.first && value.second > sectionAndOffset.second)
      {
        map[key] = {value.first, value.second + 40};
        Section* tempSection = linkerSections[value.first];
        tempSection->incrementLocationCounter(40);
      }
  }
}

void Linker::adjustOffset()
{
  std::vector<RelocationEntry*> tempRelocationTable;
  for(size_t i = 0; i < arrayOfRelocationEntryTables.size(); i++)
  {
    tempRelocationTable = arrayOfRelocationEntryTables[i];

    for(size_t j = 0; j < tempRelocationTable.size(); j++)
    {
      fixRelocationTable(arrayOfRelocationEntryTables, i, j);
      addOffsetToSections(mappingFileSectionToSectionOffset, i, tempRelocationTable[j]->getIdxSection());
      fixSymbolTable(arrayOfSymbolTables, i, tempRelocationTable[j]->getOffset(), tempRelocationTable[j]->getIdxSection());
    }
  }
}

static Symbol* findSymbolInSection(const std::vector<Symbol*>& symTable, const size_t& symName)
{
  for(size_t i = 0; i < symTable.size(); i++)
  {
    if(symTable[i]->getName() == symName)
    {
      return symTable[i];
    }
  }
}
size_t Linker::findValueOfSymbol(const size_t &currentFile, const std::string &symbolName)
{
  std::vector<Symbol*> tempSymbolTable;
  StringTable* tempSymbolStringTable;
  Symbol* tempSymbol;
  for(size_t i = 0; i < arrayOfSymbolTables.size(); i++)
  {
    if(i == currentFile)
    {
      continue;
    }
    tempSymbolTable = arrayOfSymbolTables[i];
    tempSymbolStringTable = arrayOfSymbolStringsTables[i];

    size_t nameOfSymbol = tempSymbolStringTable->findString(symbolName);
    if(nameOfSymbol)
    {
      tempSymbol = findSymbolInSection(tempSymbolTable, nameOfSymbol);
      if(tempSymbol->getDefined() && tempSymbol->getBinding() == Symbol::Binding::Export && tempSymbol->getScope() == Symbol::Scope::Global)
      {
        size_t value;
        std::pair<size_t, size_t> symbolSectionAndOffset = mappingFileSectionToSectionOffset[{i, tempSymbol->getSection()}];
        Section* newSymbolSection = linkerSections[symbolSectionAndOffset.first];
        value = newSymbolSection->getVirtualAddress() + symbolSectionAndOffset.second + tempSymbol->getValue();
        return value;
      }
    }
    // else
    // {
    //   continue;
    // }
  }
}


