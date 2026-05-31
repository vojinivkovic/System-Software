#include "linker.hpp"
#include "elf_header.hpp"
#include "section.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "relocation/relocation_table.hpp"
#include "aux/instructions_functions.hpp"
#include <algorithm>
#include <iomanip>

std::vector<StringTable*> Linker::arrayOfSymbolStringsTables;
std::vector<StringTable*> Linker::arrayOfSectionStringTables;
std::vector<std::vector<Section*>> Linker::arrayOfFilesSections;
std::vector<std::vector<Symbol*>> Linker::arrayOfSymbolTables;
std::vector<std::vector<RelocationEntry*>> Linker::arrayOfRelocationEntryTables;
std::vector<Section*> Linker::linkerSections;
std::vector<Symbol*> Linker::linkerSymbols;
std::map<size_t, size_t> Linker::occupiedMemoryRegions;
std::vector<std::string> Linker::files;
std::map<std::pair<size_t, size_t>, std::pair<size_t, size_t>> Linker::mappingFileSectionToSectionOffset;
std::map<std::pair<size_t, size_t>, size_t> Linker::mappingOfSymbols;
StringTable* Linker::sectionStringTable = new StringTable(StringTable::STType::SectionName);
StringTable* Linker::symbolStringTable = new StringTable(StringTable::STType::SymbolName);;
std::unordered_map<std::string, uint32_t> Linker::placeMapping;
std::vector<RelocationEntry*> Linker::linkerReloactionEntries;
Section* Linker::sectionSymTable;
Section* Linker::sectionSymStrTable; 
Section* Linker::sectionSecStrTable;


static StringTable* readSectionStringTable(const std::string& fileName, const Section* tempSectionStringTable, const StringTable::STType& type)
{
  std::ifstream inputFile(fileName);
  size_t offset = tempSectionStringTable->getOffsetInFile();
  size_t offsetInFile, numOfRows = offset / 4, numOfCol = offset % 4;
  size_t size = tempSectionStringTable->getLocationCounter();
  char tempChar;
  StringTable* tempStringTable;

  offsetInFile = numOfRows * 13 + numOfCol * 3;
  std::string sectionNames;
  inputFile.seekg(offsetInFile);

  for(size_t i = 0; i < size; i++)
  {
    std::string hexByte;

    inputFile >> hexByte;

    unsigned int value = std::stoul(hexByte, nullptr, 16);

    sectionNames.push_back(static_cast<char>(value));
  }
  tempStringTable = new StringTable(type, sectionNames);
  return tempStringTable;
}

static void findSymbolTables(Section*& symbolStringTable, Section*& symbolTable, 
  const StringTable* tempSectionStringTable, const std::vector<Section*>& sections)
{
  size_t nameSymbolTable = tempSectionStringTable->findString(".symtable");
  size_t nameSymbolStringTable = tempSectionStringTable->findString(".symstrtab");
  // std::cout << std::to_string(nameSymbolTable) << std::endl;
  // std::cout << sectionStringTable->getNames() << std::endl;
  //std::cout << std::to_string(sectionStringTable->getNames().size()) << std::endl;
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

static void findRelocationTableSection(const StringTable* tempSectionStringTable, const std::vector<Section*>& sections, Section*& relocationTableSection)
{
  size_t nameRelocationTable = tempSectionStringTable->findString(".rela");

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
  std::cout << "Reading of the elf file" << std::endl;
  ELFHeader::ELFHeaderType elfType; 
  size_t entry, phoff, shoff, phentsize, phnum, shentsize, shnum, shstrndx;
  std::vector<Section*> tableOfSections;
  std::vector<Symbol*> tableOfSymbols;
  std::vector<RelocationEntry*> tableOfRelocationTables;
  StringTable* tempSectionStringTable, *tempSymbolStringTable;
  Section* symbolTable, *symbolStringTable, *relocationTable;

  ELFHeader::readElfHeader(fileName, elfType, entry, phoff, shoff, phentsize, phnum, shentsize, shnum, shstrndx);
  //std::cout << "Elf Header read" << std::endl;

  tableOfSections = Section::readSectionHeader(fileName, shoff, shnum);
  //std::cout << "Sections read" << std::endl;

  tempSectionStringTable = readSectionStringTable(fileName, tableOfSections[shstrndx], StringTable::STType::SectionName);
  //std::cout << "Section string read" << std::endl;

  findSymbolTables(symbolStringTable, symbolTable, tempSectionStringTable, tableOfSections);
  findRelocationTableSection(tempSectionStringTable, tableOfSections, relocationTable);
  //std::cout << "Finding symbols and reloc fnished" << std::endl;


  tempSymbolStringTable = readSectionStringTable(fileName, symbolStringTable, StringTable::STType::SymbolName);
  //std::cout << "Symbol string read" << std::endl;

  tableOfSymbols = SymbolTable::readSymbolsFromElfFile(fileName, symbolTable);
  //std::cout << "Symbols read" << std::endl;

  tableOfRelocationTables = RelocationTable::readRelocationTableFromElfFile(fileName, relocationTable);
  //std::cout << "Reloc read" << std::endl;
  //SORT RELOCATION ENTRIES BASED ON OFFSET
    std::sort(tableOfRelocationTables.begin(), tableOfRelocationTables.end(),
    [](const RelocationEntry* a, const RelocationEntry* b)
    {
        if(a->getIdxSection() == b->getIdxSection())
        {
          return a->getOffset() < b->getOffset();
        }
        return a->getIdxSection() < b->getIdxSection();
    }
    );

    arrayOfSymbolStringsTables.push_back(tempSymbolStringTable);
    arrayOfSectionStringTables.push_back(tempSectionStringTable);
    arrayOfFilesSections.push_back(tableOfSections);
    arrayOfSymbolTables.push_back(tableOfSymbols);
    arrayOfRelocationEntryTables.push_back(tableOfRelocationTables);
}

void Linker::readInputFiles()
{
  for(size_t i = 0; i < files.size(); i++)
  {
    readElfFile(files[i]);
  }
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
  offsetInFile = numOfRows * 13  + numOfCol * 3;
  inputFile.seekg(offsetInFile);
  std::string hexByte;
  std::vector<uint8_t> content;

  //std::cout << "size: " << std::to_string(size) << std::endl;

  for(size_t i = 0; i < size; i++)
  {
    inputFile >> hexByte;
    content.push_back(static_cast<uint8_t>(std::stoul(hexByte, nullptr, 16)));
  }
  return content;
}

static std::vector<std::string> getTexContetOfSection(const std::string& objectFileName,const std::string& sectionName)
{
 std::string txtFileName = objectFileName;

    size_t pos = txtFileName.rfind('.');
    if (pos != std::string::npos)
    {
        txtFileName = txtFileName.substr(0, pos) + ".txt";
    }
    else
    {
        txtFileName += ".txt";
    }

    std::ifstream input(txtFileName);
    if (!input)
    {
        throw std::runtime_error(
            "Failed to open file: " + txtFileName);
    }

    std::vector<std::string> result;
    std::string line;

    const std::string targetHeader = ".section: " + sectionName;
    bool insideSection = false;

    while (std::getline(input, line))
    {
        if (!insideSection)
        {
            if (line == targetHeader)
            {
                insideSection = true;
            }
            continue;
        }

        if (line == "END_OF_SECTION")
        {
            break;
        }

        result.push_back(line);
    }

    if (!insideSection)
    {
        throw std::runtime_error(
            "Section not found: " + sectionName);
    }

    return result;
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
  std::vector<Symbol*> tempSymbolTable;
  StringTable* tempSectionStringTable, *tempSymbolStringTable; 
  std::vector<uint8_t> tempContent;
  std::vector<std::string> tempTextContent;
  std::string tempSectionName;
  size_t newSectionName, symbolName, idxSymbol;
  Section* newSection, *oldSection;
  Symbol* newSymbol, *sectionSymbol, *tempSymbol;
  size_t idxSection;


  newSection = new Section(sectionStringTable->getOffset(), Section::SectionType::NullSection, 0, 0, 0, 0, linkerSections.size());
  newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), 0, 0, linkerSections.size(), Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  symbolStringTable->addString(".UND");
  linkerSymbols.push_back(newSymbol);
  
  sectionStringTable->addString(".UND");
  linkerSections.push_back(newSection);
  
  for(size_t i = 0; i < arrayOfFilesSections.size(); i++)
  {
    //std::cout << "num of file: " << i << std::endl;

    tempSections = arrayOfFilesSections[i];
    tempSectionStringTable = arrayOfSectionStringTables[i];
    tempSymbolStringTable = arrayOfSymbolStringsTables[i];
    tempSymbolTable  = arrayOfSymbolTables[i];


    for(int j = 0; j < tempSections.size(); j++)
    {
      //std::cout << "num of section: " << j << std::endl;
      tempContent = getContentOfSection(files[i], tempSections[j]->getOffsetInFile(), tempSections[j]->getLocationCounter());
      //std::cout << "content got: " << tempContent.size() << std::endl; 
      tempSectionName = tempSectionStringTable->getNameOfElement(tempSections[j]->getSectionName());
      
      // std::cout << "section names: " << sectionStringTable->getNames() << std::endl;
      // std::cout << "section name: " << tempSectionName << std::endl;
      newSectionName = sectionStringTable->findString(tempSectionName);
      symbolName = tempSymbolStringTable->findString(tempSectionName);

      for(size_t k = 0; k < tempSymbolTable.size(); k++)
      {
        if(tempSymbolTable[k]->getName() == symbolName)
        {
          sectionSymbol = tempSymbolTable[k];
          idxSymbol = k;
          //std::cout << "num of symbol: " << k << std::endl;
          break;
        }
      }
      
      //std::cout << "new section name: " << newSectionName << std::endl;
      if(!tempContent.size() || tempSectionName == ".rela" || 
      tempSectionName == ".symtable" || tempSectionName == ".symstrtab" 
      || tempSectionName == ".secstrtab")
      {
        //std::cout << "continuing" << std::endl;
        continue;
      }
      
      if(newSectionName == std::string::npos)
      {
        //std::cout << "making new section: " << tempSectionName << std::endl; 
        newSection = new Section(sectionStringTable->getOffset(), tempSections[j]->getSectionType(), 0, 0, tempSections[j]->getLocationCounter(), tempSections[j]->getSizeOfEntry(), linkerSections.size());
        tempTextContent = getTexContetOfSection(files[i], tempSectionName);
        newSection->setContent(tempContent);
        newSection->setTextContent(tempTextContent);
        mappingFileSectionToSectionOffset[{i, j}] = {linkerSections.size(), 0};

        Symbol* newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), 0, 0, linkerSections.size(), Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
        symbolStringTable->addString(tempSectionName);
        linkerSymbols.push_back(newSymbol);
        mappingOfSymbols[{i, idxSymbol}] = newSymbol->getIdx();

        sectionStringTable->addString(tempSectionName);
        linkerSections.push_back(newSection);      
      }
      else
      {
        idxSection = findSection(newSectionName, linkerSections);
        oldSection = linkerSections[idxSection];
        mappingFileSectionToSectionOffset[{i, j}] = {idxSection, oldSection->getLocationCounter()};
        oldSection->addContent(tempContent);
        tempTextContent = getTexContetOfSection(files[i], tempSectionName);
        oldSection->addTextContent(tempTextContent);

        std::size_t linkerSymbolSectionName = symbolStringTable->findString(tempSectionName);
        for(size_t p = 0; p < linkerSymbols.size(); p++)
        {
          if(linkerSymbols[p]->getName() == linkerSymbolSectionName)
          {
            tempSymbol = linkerSymbols[p];
            break;
          }
        }
        mappingOfSymbols[{i, idxSymbol}] = tempSymbol->getIdx();

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
  
  for(size_t i = 0; i < arrayOfRelocationEntryTables.size(); i++)
  {
   // std::cout << "num of file: " << i << std::endl;
    tempRelocationTable = arrayOfRelocationEntryTables[i];
    tempSymbolTable = arrayOfSymbolTables[i];
    tempSectionTable = arrayOfFilesSections[i];
    tempSymbolStringTable = arrayOfSymbolStringsTables[i];
    tempSectionStringTable = arrayOfSectionStringTables[i];

    for(size_t j = 0; j < tempRelocationTable.size(); j++)
    {
      tempSymbol = tempSymbolTable[tempRelocationTable[j]->getIdxSymbol()];
      tempSection = tempSectionTable[tempRelocationTable[j]->getIdxSection()];
      //std::cout << "num of reloc: " << j << std::endl;
      //std::cout << "idx of symbol: " << tempRelocationTable[j]->getIdxSymbol() << ", idx of section: "
      //<< tempRelocationTable[j]->getIdxSection() << std::endl;

      std::pair<size_t, size_t> sectionAndOffset = mappingFileSectionToSectionOffset[{i, tempSection->getIdxOfSection()}];
      size_t startOffset = sectionAndOffset.second;
      size_t offsetOfRelocation = startOffset + tempRelocationTable[j]->getOffset();
      newSection = linkerSections[sectionAndOffset.first];
      std::vector<uint8_t> tempContent = newSection->getContent();
      
      uint8_t destReg = tempContent[offsetOfRelocation + 2];
      destReg >>= 4;
      size_t value;
      
      //std::cout << sectionStringTable->getNames() << std::endl;
      //std::cout << "section name: " << newSection->getSectionName() << std::endl;
      //std::cout << tempSymbolStringTable->getNames() << std::endl;
      //std::cout << tempSymbol->getName() << std::endl;

      if(tempSymbol->getDefined())
      {
        // std::cout << "symbol defined" << std::endl;
        std::pair<size_t, size_t> symbolSectionAndOffset = mappingFileSectionToSectionOffset[{i, tempSymbol->getSection()}];
        Section* newSymbolSection = linkerSections[symbolSectionAndOffset.first];
        //std::cout << "new virtual address: " << newSymbolSection->getVirtualAddress() << std::endl;
        value = newSymbolSection->getVirtualAddress() + symbolSectionAndOffset.second + tempSymbol->getValue() + tempRelocationTable[j]->getAddend();
        //std::cout << "new value: " << value << std::endl;
        std::vector<uint8_t> newContent = transformLoadInstruction(destReg, value);

        for(size_t i = 0; i < newContent.size(); i += 4)
        {
          std::reverse(newContent.begin() + i, newContent.begin() + i + 4);
        }

        tempContent.erase(tempContent.begin() + offsetOfRelocation, tempContent.begin() + offsetOfRelocation + 4);
        tempContent.insert(tempContent.begin() + offsetOfRelocation, newContent.begin(), newContent.end());
        newSection->setContent(tempContent);

      }
      else
      {
        std::string symbolName = tempSymbolStringTable->getNameOfElement(tempSymbol->getName());
        value = findValueOfSymbol(i, symbolName) + tempRelocationTable[j]->getAddend();
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


void Linker::makeLinkerRelocationEntries()
{
  RelocationEntry* tempRelocEntry;

  for(size_t i = 0; i < arrayOfRelocationEntryTables.size(); i++)
  {
    std::vector<RelocationEntry*> tempRelocationTable = arrayOfRelocationEntryTables[i];
    
    for(size_t j = 0; j < tempRelocationTable.size(); j++)
    {
      tempRelocEntry = tempRelocationTable[j];
      std::pair<size_t, size_t> sectionAndOffset = mappingFileSectionToSectionOffset[{i, tempRelocEntry->getIdxSection()}];
      size_t newOffset = sectionAndOffset.second + tempRelocEntry->getOffset();
      size_t newIdxSymbol = mappingOfSymbols[{i, tempRelocEntry->getIdxSymbol()}];

      RelocationEntry* newReloc = new RelocationEntry(newOffset, sectionAndOffset.first, newIdxSymbol, tempRelocEntry->getAddend());
      linkerReloactionEntries.push_back(newReloc);
    }
  }
}

void Linker::addSectionMapping(const std::string &secionName, const uint32_t &memAddress)
{
  placeMapping[secionName] = memAddress;
}

static std::vector<std::string> formatMemInitSection(size_t startVirtualAddress, const std::vector<uint8_t>& content)
{
    constexpr size_t bytesPerLine = 4;

    std::vector<std::string> lines;

    for (size_t i = 0; i < content.size(); i += bytesPerLine)
    {
        std::ostringstream oss;

        oss << std::uppercase << std::hex << std::setw(4) << std::setfill('0')
            << (startVirtualAddress + i) << ": ";

        for (size_t j = 0; j < bytesPerLine && i + j < content.size(); j++)
        {
            oss << std::setw(2)
                << std::setfill('0')
                << static_cast<unsigned>(content[i + j]);

            if (j + 1 < bytesPerLine && i + j + 1 < content.size())
            {
                oss << ' ';
            }
        }

        lines.push_back(oss.str());
    }

    return lines;
}

void Linker::fixAddresses()
{
  adjustOffset();
  fixVirtualAddressOfSections();
}

void Linker::makeExecElfFile(const std::string &name)
{
  size_t sizeOfFile = 10 * sizeof(size_t);
  std::vector<std::string> memInitializerContent;
  std::vector<std::string> textContent;
  std::string sectionName;

  std::sort(linkerSections.begin(), linkerSections.end(),
    [](const Section* a, const Section* b)
    {
        return a->getVirtualAddress() < b->getVirtualAddress();
    });

  for(auto iSection : linkerSections)
  {

    iSection->setOffsetInFile(sizeOfFile);
    std::vector<std::string> tempMemInitContent = formatMemInitSection(iSection->getVirtualAddress(), iSection->getContent());
    if(!tempMemInitContent.empty())
    {
      sectionName = sectionStringTable->getNameOfElement(iSection->getSectionName());
      if(sectionName != ".symtable" && sectionName != ".symstrtable" && sectionName != ".secstrtab")
      {
        memInitializerContent.insert(memInitializerContent.end(), tempMemInitContent.begin(), tempMemInitContent.end());
      }
      std::vector<std::string> tempStringContent = iSection->getTextContent();
      if(!tempStringContent.empty())
      {
        textContent.push_back(".section: " + sectionName);
        textContent.insert(textContent.end(), tempStringContent.begin(), tempStringContent.end());
      }
      textContent.push_back("END OF SECTION");
      sizeOfFile += iSection->getLocationCounter();
    }
    
  }

  ELFHeader::makeELFHeader(ELFHeader::ELFHeaderType::ELF_EXEC, 0, 0, sizeOfFile, 0, 0, 
  6 * sizeof(size_t), linkerSections.size(), linkerSections.size() - 1);

  for(auto iSection : linkerSections) 
  {
    textContent.push_back(iSection->getTextFormatOfSection());
  }
  
  textContent.insert(textContent.begin(), ELFHeader::getStringFormat());

  std::ofstream outBin(name);

  for(const auto& line : memInitializerContent)
  {
    outBin << line << '\n';
  }

  std::ofstream outText("hex_" + name.substr(0, name.size() - 4) + ".txt");
  for(size_t i = 0; i < textContent.size(); i++)
  {
    outText << textContent[i] << '\n';
  }

}

void Linker::execPipeline()
{
  makeSymbolTableSection();
  makeSectionOfSectionString();
  addContentInSectionSymTable();
  addContentInSectionSymStrTable();
  addContentInSectionSecStrTable();
}

void Linker::relocPipeline()
{
  makeRelocationTableSection();
  makeSymbolTableSection();
  makeSectionOfSectionString();
  addContentInSectionSymTable();
  addContentInSectionSymStrTable();
  addContentInSectionSecStrTable();
}

void Linker::makeRelocElfFile(const std::string &name)
{
  size_t sizeOfFile = 10 * sizeof(size_t);
  std::vector<uint8_t> binaryContent;
  std::vector<std::string> textContent;
  std::string sectionName;

  for(auto iSection : linkerSections)
  {

    iSection->setOffsetInFile(sizeOfFile);
    std::vector<uint8_t> tempContent = iSection->getContent();
    if(!tempContent.empty())
    {
      binaryContent.insert(binaryContent.end(), tempContent.begin(), tempContent.end());
      sectionName = sectionStringTable->getNameOfElement(iSection->getSectionName());

      std::vector<std::string> tempStringContent = iSection->getTextContent();
      if(!tempStringContent.empty())
      {
        textContent.push_back(".section: " + sectionName);
        textContent.insert(textContent.end(), tempStringContent.begin(), tempStringContent.end());
      }
      textContent.push_back("END OF SECTION");
      sizeOfFile += iSection->getLocationCounter();
    }
    
  }

  ELFHeader::makeELFHeader(ELFHeader::ELFHeaderType::ELF_REL, 0, 0, sizeOfFile, 0, 0, 
  6 * sizeof(size_t), linkerSections.size(), linkerSections.size() - 1);

  for(auto iSection : linkerSections) 
  {
    std::vector<uint8_t> tempContentSec = iSection->getLittleEndiandOfSection();
    binaryContent.insert(binaryContent.end(), tempContentSec.begin(), tempContentSec.end());
    textContent.push_back(iSection->getTextFormatOfSection());
  }
  

  std::vector<uint8_t> elfHeaderBinaryContent = ELFHeader::getLittleEndianFormat();
  binaryContent.insert(binaryContent.begin(), elfHeaderBinaryContent.begin(), elfHeaderBinaryContent.end());
  textContent.insert(textContent.begin(), ELFHeader::getStringFormat());

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
  std::ofstream outText("reloc: " + name.substr(0, name.size() - 2) + ".txt");
  for(size_t i = 0; i < textContent.size(); i++)
  {
    outText << textContent[i] << '\n';
  }
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
      break;
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
void Linker::makeRelocationTableSection()
{
  std::vector<uint8_t> content;
  std::vector<uint8_t> subContent;
  std::string subContentString;
  std::vector<std::string> textContent;
  Section* newSection = new Section(sectionStringTable->getOffset(), Section::SectionType::RelaSection, 0, 0, linkerReloactionEntries.size() * 4 * sizeof(size_t), 4 * sizeof(size_t), linkerSections.size());
  sectionStringTable->addString(".rela");
  linkerSections.push_back(newSection);
  
  for(auto iReloc : linkerReloactionEntries)
  {
    subContent = iReloc->getLittleEndianFormatOfRE();
    content.insert(content.end(), subContent.begin(), subContent.end());
    newSection->incrementLocationCounter(4 * sizeof(size_t));
    subContentString = "Offset: " + std::to_string(iReloc->getOffset()) + 
      ", idxSection: " + std::to_string(iReloc->getIdxSection()) + 
      ", idxSymbol: " + std::to_string(iReloc->getIdxSymbol()) + 
      ", Addend: " + std::to_string(iReloc->getAddend());
    
    textContent.push_back(subContentString);
  }

  newSection->setContent(content);
  newSection->setTextContent(textContent);
        

  Symbol* newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), 0, 0, linkerSections.size(), Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  symbolStringTable->addString(".rela");
  linkerSymbols.push_back(newSymbol);
        
}
void Linker::makeSymbolTableSection()
{
  sectionSymTable = new Section(sectionStringTable->getOffset(), Section::SectionType::SymTabSection, 0, 0, 0, 0, linkerSections.size());
  sectionStringTable->addString(".symtable");
  linkerSections.push_back(sectionSymTable);

  Symbol* newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), 0, 0, linkerSections.size(), Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  symbolStringTable->addString(".symtable");
  linkerSymbols.push_back(newSymbol);

  sectionSymStrTable = new Section(sectionStringTable->getOffset(), Section::SectionType::SymStrTabSection, 0, 0, 0, 0, linkerSections.size());
  sectionStringTable->addString(".symstrtable");
  linkerSections.push_back(sectionSymStrTable);

  newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), 0, 0, linkerSections.size(), Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  symbolStringTable->addString(".symstrtable");
  linkerSymbols.push_back(newSymbol);

}
void Linker::makeSectionOfSectionString()
{
  sectionSecStrTable = new Section(sectionStringTable->getOffset(), Section::SectionType::SectionStrTabSection, 0, 0, 0, 0, linkerSections.size());
  sectionStringTable->addString(".secstrtab");
  linkerSections.push_back(sectionSecStrTable);

  Symbol* newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), 0, 0, linkerSections.size(), Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  symbolStringTable->addString(".secstrtab");
  linkerSymbols.push_back(newSymbol);
}
void Linker::addContentInSectionSymTable()
{
  std::vector<uint8_t> content;
  std::vector<uint8_t> subContent;
  std::string subContentString;
  std::vector<std::string> textContent;
  for(auto iSymbol : linkerSymbols)
  {
    subContent = iSymbol->getLittleEndianFormatOfSymbol();
    content.insert(content.end(), subContent.begin(), subContent.end());
    sectionSymTable->incrementLocationCounter(7 * sizeof(size_t));
    subContentString = "Name: " + std::to_string(iSymbol->getName()) +
      ", Size: " + std::to_string(iSymbol->getSize()) + 
      ", Value: " + std::to_string(iSymbol->getValue()) + 
      ", Section: " + std::to_string(iSymbol->getSection()) + 
      ", Bind: " + std::to_string(static_cast<size_t>(iSymbol->getBinding())) + 
      ", Type: " + std::to_string(static_cast<size_t>(iSymbol->getType())) + 
      ", Scope: " + std::to_string(static_cast<size_t>(iSymbol->getScope()));
    
    textContent.push_back(subContentString);
  }
  sectionSymTable->setContent(content);
  sectionSymTable->setTextContent(textContent);
}
void Linker::addContentInSectionSymStrTable()
{
  std::vector<uint8_t> content;
  std::vector<std::string> textContent;
  std::string tableOfNames = symbolStringTable->getNames();
  for(size_t i = 0; i < tableOfNames.size(); i++)
  {
    content.push_back(static_cast<uint8_t>(tableOfNames[i]));
    sectionSymStrTable->incrementLocationCounter(1);
  }
  textContent.push_back(tableOfNames);
  sectionSymStrTable->setContent(content);
  sectionSymStrTable->setTextContent(textContent);
}
void Linker::addContentInSectionSecStrTable()
{
  std::vector<uint8_t> content;
  std::vector<std::string> textContent;
  std::string tableOfNames = sectionStringTable->getNames();
  for(size_t i = 0; i < tableOfNames.size(); i++)
  {
    content.push_back(static_cast<uint8_t>(tableOfNames[i]));
    sectionSecStrTable->incrementLocationCounter(1);
  }
  textContent.push_back(tableOfNames);
  sectionSecStrTable->setContent(content);
  sectionSecStrTable->setTextContent(textContent);
}
bool Linker::checkSectionsOverlapping(const size_t& currentStartSection)
{
  size_t currentEndOfSection = occupiedMemoryRegions[currentStartSection];
  for(const auto& entry : occupiedMemoryRegions)
  {
    if(entry.first == currentStartSection)
    {
      continue;
    }
    if((currentStartSection >= entry.first && currentStartSection <= entry.second) ||
      (currentEndOfSection >= entry.first && currentEndOfSection <= entry.second)) 
    {
      return true;
    }
  }
  return false;
}
void Linker::makeLinkerSymbolTable()
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
        std::string newSymbolName = files[i] + "::" + symName;
        std::pair<size_t, size_t> sectionAndOffset = mappingFileSectionToSectionOffset[{i, tempSymbol->getSection()}];
        size_t newValue;

        newValue = sectionAndOffset.second + tempSymbol->getValue();        

        Symbol* newSymbol = new Symbol(linkerSymbols.size(), symbolStringTable->getOffset(), tempSymbol->getSize(), newValue, sectionAndOffset.first, Symbol::Binding::NoBinding, tempSymbol->getType(),  Symbol::Scope::Local, true);
        symbolStringTable->addString(newSymbolName);
        linkerSymbols.push_back(newSymbol); 
        mappingOfSymbols[{i, j}] = newSymbol->getIdx();
      }
      if(symNameInLinker != std::string::npos)
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
    if(array[numOfTable][i]->getIdxSection() == array[numOfTable][numOfEntry]->getIdxSection())
    {
      array[numOfTable][i]->setOffset(array[numOfTable][i]->getOffset() + 40);
    }
    else
    {
      break;
    }
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
  uint32_t currentVirtualAddress = 0, newVirtualAddress, currentStartMemorySection = 0;
  Section* tempSection;
  std::string sectionName;
  //std::cout << sectionStringTable->getNames() << std::endl;
  for(size_t i = 0; i < linkerSections.size(); i++)
  {
    tempSection = linkerSections[i];
    sectionName = sectionStringTable->getNameOfElement(tempSection->getSectionName());

    auto it = placeMapping.find(sectionName);

    if(it != placeMapping.end())
    {
      newVirtualAddress = it->second;
      //std::cout << "current address: " << currentVirtualAddress << std::endl;
      //std::cout << "new virtual address: " << newVirtualAddress << std::endl; 
      // if(currentVirtualAddress > newVirtualAddress)
      // {
      //   throw LinkerErrors(ErrorType::ErrorOverlappingSections, "Section [" + sectionName + "] can't be placed ad define address");
      // }
      tempSection->setVirtualAddress(newVirtualAddress);
      currentStartMemorySection = newVirtualAddress;
      currentVirtualAddress = newVirtualAddress;
    }
    else
    {
      tempSection->setVirtualAddress(currentVirtualAddress);
    }

    currentVirtualAddress += tempSection->getLocationCounter();
    occupiedMemoryRegions[currentStartMemorySection] = currentVirtualAddress;
    if(checkSectionsOverlapping(currentStartMemorySection))
    {
     throw LinkerErrors(ErrorType::ErrorOverlappingSections, 
      "Section [" + sectionName + "] can't be placed ad define address");
      
    }
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
    if(nameOfSymbol != std::string::npos)
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
  }
}


