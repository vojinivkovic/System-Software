#include "linker.hpp"
#include "elf_header.hpp"
#include "section.hpp"
#include <iostream>
#include <fstream>

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

void Linker::readElfFile(const std::string &fileName)
{
  ELFHeader::ELFHeaderType elfType; 
  size_t entry, phoff, shoff, phentsize, phnum, shentsize, shnum, shstrndx;
  std::vector<Section*> tableOfSections;
  std::vector<Symbol*> tableOfSymbols;
  StringTable* tempSectionStringTable, *tempSymbolStringTable;
  Section* symbolTable, *symbolStringTable;

  ELFHeader::readElfHeader(fileName, elfType, entry, phoff, shoff, phentsize, phnum, shentsize, shnum, shstrndx);
  tableOfSections = Section::readSectionHeader(fileName, shoff, shnum);
  tempSectionStringTable = readSectionStringTable(fileName, tableOfSections[shstrndx], StringTable::STType::SectionName);

  findSymbolTables(symbolStringTable, symbolTable, tempSectionStringTable, tableOfSections);

  tempSymbolStringTable = readSectionStringTable(fileName, symbolStringTable, StringTable::STType::SymbolName);

  tableOfSymbols = SymbolTable::readSymbolsFromElfFile(fileName, symbolTable);
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
