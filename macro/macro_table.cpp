#include "macro_table.hpp"
#include "../aux/exceptions.hpp"
#include "../assembler.hpp"
#include <iostream>
#include "../symbol/symbol.hpp"
#include "../symbol/symbol_table.hpp"

std::vector<Macro*> MacroTable::table;
StringTable* MacroTable::tableOfMacroString = new StringTable(StringTable::STType::MacroName);
std::unordered_map<size_t, size_t> MacroTable::mappingMacroToSymbol;

Macro *MacroTable::findMacro(const std::string &name)
{
  std::string::size_type findIdx = tableOfMacroString->findString(name);
  if(findIdx == std::string::npos)
  {
    return nullptr;
  }

  for(auto iMacro : table)
  {
    if(iMacro->getName() == findIdx)
    {
      return iMacro;
    }
  } 
}

void MacroTable::AddMacro(const std::string &name, Macro *newMacro)
{
  std::cout << "Macro was added" << std::endl;
  tableOfMacroString->addString(name);
  table.push_back(newMacro);
}

void MacroTable::tryToResolveAllMacros(const std::vector<std::string>& definedSymbols)
{
  std::vector<std::string> newDefinedSymbols;
  for(auto iMacro : table)
  {
    if(!iMacro->getDefined())
    {
      if(iMacro->checkForResolving(definedSymbols))
      {
        iMacro->resolveMacro();
        newDefinedSymbols.push_back(MacroTable::getNameOfMacro(iMacro->getName()));
      }
    }
  }
  if(!newDefinedSymbols.empty())
  {
    tryToResolveAllMacros(newDefinedSymbols);
  }
}

std::string MacroTable::getNameOfMacro(size_t name)
{

  return tableOfMacroString->getNameOfElement(name);
}

bool MacroTable::checkDefinition(Macro*& undefinedMacro)
{
  for(auto iMacro : table)
  {
    if(!iMacro->getDefined())
    {
      undefinedMacro = iMacro;
      return false;
    }
  }
  return true;
}


static bool checkIfLoadStore(std::vector<uint8_t> content, size_t offset)
{
  uint8_t firstField = content[offset];
  if(firstField == 0x80 || firstField == 0x92)
  {
    return true;
  }
  return false;
}

void MacroTable::resolveForwardReference()
{
  for(auto iMacro : table)
  {
    std::vector<ForwardReference*> tableOfForwardReference = iMacro->getForwardReference();
    if(tableOfForwardReference.size() > 0) 
    {
      for(auto forwardReference : tableOfForwardReference) 
      {
        Section* tempSection = Assembler::getSections()[forwardReference->getSection()];
        if(checkIfLoadStore(tempSection->getContent(), forwardReference->getOffset()) && (iMacro->getValue() < -(1 << 11) || iMacro->getValue() > 1 << 11 - 1))
        {
          throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.ld/st] expects (signed) literals that can be represented with 12 bits",
    forwardReference->getSection(), forwardReference->getOffset()); 
        }
        else
        {
          tempSection->insertValueInContent((uint32_t)iMacro->getValue(), forwardReference->getOffset());
        }
      }
    }
  }
}

std::vector<std::string> MacroTable::getTextRepresentationOfMacros()
{
  std::vector<std::string> macroTable;
  std::string tempElem;
  for(auto iMacro : table)
  {
    tempElem = "Name: " + std::to_string(iMacro->getName()) + 
    ", Value: " + std::to_string((uint32_t)iMacro->getValue());
    macroTable.push_back(tempElem);
  }
  return macroTable;
}

void MacroTable::addMapping(const size_t &idxMacro, const size_t &idxSymbol)
{
  mappingMacroToSymbol[idxMacro] = idxSymbol;
}

void MacroTable::setValuesOfSymbols()
{
  for(size_t i = 0; i < table.size(); i++)
  {
    Symbol* tempSymbol = SymbolTable::getSymbolBasedOnIdx(mappingMacroToSymbol[i]);
    tempSymbol->setDefined();
    tempSymbol->setValue(table[i]->getValue());
  }
}
