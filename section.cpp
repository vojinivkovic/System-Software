#include "section.hpp"
#include "assembler.hpp"
#include "symbol/symbol_table.hpp"
#include "aux/instructions.hpp"
#include "aux/instructions_functions.hpp"
#include "aux/auxiliary_func.hpp"
#include "macro/macro_table.hpp"
#include "aux/exceptions.hpp"
#include "aux/string_table.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>

StringTable* Section::tableOfSectionString = new StringTable(StringTable::STType::SectionName);

Section::Section(const std::string& sectionName, const SectionType& type_) : name(tableOfSectionString->getOffset()), locationCounter(0), idxSection(Assembler::getNumberOfSections()), type(type_)
{ 
  std::cout << "Make new section" << std::endl;
  Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, idxSection, Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  SymbolTable::addSymbol(sectionName, newSymbol);
  tableOfSectionString->addString(sectionName);
}

Section::Section(const size_t &name_, const Section::SectionType &type_, const uint32_t &vAddress_, const size_t &offset_, const size_t &size_, const size_t &sizeOfEntry_, const size_t &idx_)
: name(name_), type(type_), virtualAddress(vAddress_), offsetInFile(offset_), locationCounter(size_), sizeOfEntry(sizeOfEntry_), idxSection(idx_)  
{}

int Section::translateInstruction(const std::string &instruction, const std::vector<Argument> &arguments)
{
  std::cout << "Instruction translation[" << instruction << "]" << std::endl;
  std::vector<uint8_t> binaryInstruction = Instructions::translate(instruction, arguments);
  textRepresentationOfInstruction(instruction, arguments);
  for(size_t i = 0; i < binaryInstruction.size(); i += 4)
  {
    std::reverse(binaryInstruction.begin() + i, binaryInstruction.begin() + i + 4);
  }
  content.insert(content.end(), binaryInstruction.begin(), binaryInstruction.end());
  locationCounter += binaryInstruction.size();
  // if(instruction != "iret")
  // {
  //   content.insert(content.end(), binaryInstruction.rbegin(), binaryInstruction.rend());
  //   locationCounter += 4;
  // }
  // else
  // {
  //   for(int i = 3; i >= 0; i--)
  //   {
  //     content.push_back(binaryInstruction[i]);
  //   }
  //   for(int i = 7; i >= 4; i--)
  //   {
  //     content.push_back(binaryInstruction[i]);
  //   }
  //   locationCounter += 8;
  // }
  
  return 0;
}

int Section::executeDirective(const std::string &command, const std::vector<MacroParameter>& parameters)
{
  std::cout << "Directive execution [" << command << "]" << std::endl;
  Directives::execute(command, parameters);

  return 0;
}

void Section::defineSymbol(const std::string &symbolName)
{
  std::cout << "Definition of the symbol" << std::endl;
  Symbol* tempSymol = SymbolTable::findSymbol(symbolName);
  Macro* tempMacro = MacroTable::findMacro(symbolName);

  if(tempMacro)
  {
    throw AssemblerErrors(ErrorType::ErrorSymbolAlreadyDefined, "There is already macro with same name",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
  }
  if(tempSymol)
  {
    if(tempSymol->getDefined())
    {
      throw AssemblerErrors(ErrorType::ErrorSymbolAlreadyDefined, "Symbol is already defined",
        Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
    }
    tempSymol->setValue(locationCounter);
    tempSymol->setSection(idxSection);
    tempSymol->setType(Symbol::Type::Object);
    if(tempSymol->getScope() == Symbol::Scope::NoScope)
    {
      tempSymol->setScope(Symbol::Scope::Local);
    }
    tempSymol->setDefined();
  }
  else
  {
    Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, locationCounter, idxSection, Symbol::Binding::NoBinding, Symbol::Type::Object, Symbol::Scope::Local, true);
    SymbolTable::addSymbol(symbolName, newSymbol);
  }
  MacroTable::tryToResolveAllMacros(std::vector<std::string>{symbolName});
}

std::string::size_type Section::findSectionInStringTable(const std::string &sectionName)
{
  return tableOfSectionString->findString(sectionName); 
}

void Section::callocMemory(size_t sizeOfAllocation)
{
  locationCounter += sizeOfAllocation;
  content.insert(content.end(), sizeOfAllocation, 0);
  textContent.insert(textContent.end(), sizeOfAllocation, "\n");
  
}

void Section::insertString(const std::string &stringToInsert)
{
  std::cout << "Insert string" << std::endl;
  size_t endOfString = stringToInsert.size() - 1;
  std::string text;
  locationCounter += endOfString;
  for(size_t i = 1; i < endOfString; i++)
  {
    content.push_back(stringToInsert[i]);
    text.push_back(stringToInsert[i]);
  }
  content.push_back('\0');
  textContent.push_back(text);
}

void Section::insertContent(const uint32_t &value)
{
  textContent.push_back(std::to_string(value));
  locationCounter += 4;
  content.push_back(value & 0xFF);
  content.push_back((value >> 8) & 0xFF);
  content.push_back((value >> 16) & 0xFF);
  content.push_back((value >> 24) & 0xFF);
}

void Section::insertValueInContent(const uint32_t &value, size_t offset)
{
  if(content[offset] == 0x92 || content[offset] == 0x80)
  {
    content[offset + 2] = (content[offset + 2] & 0xF0) | ((value >> 8) & 0xF);
    content[offset + 3] = value & 0xFF;
  }
  else
  {
    uint8_t tempReg = (content[offset + 1] >> 4 ) & 0xF;
    content.erase(content.begin() + offset, content.begin() + offset + 4);
    std::vector<uint8_t> tempInstr = transformLoadInstruction(tempReg, value);
    content.insert(content.begin() + offset, tempInstr.begin(), tempInstr.end());
  }
}

void Section::addContent(const std::vector<uint8_t>& newContent)
{
  content.insert(content.end(), newContent.begin(), newContent.end());
  locationCounter += newContent.size();
} 

void Section::makeSectionOfSectionNames()
{
  tableOfSectionString->makeSection(".secstrtab", Section::SectionType::SectionStrTabSection);
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
static Section* readSectionFromSectionHeader(std::ifstream& inputFile, const size_t& idx)
{
  std::vector<uint8_t> bytes;
  std::string hexByte;
  size_t currIdx = 0, name, virtualAddress, offsetInFile, locationCounter, sizeOfEntry;
  Section::SectionType type;

  for(size_t i = 0; i < 6 * sizeof(size_t); i++)
  {
    inputFile >> hexByte;
    bytes.push_back(static_cast<uint8_t>(std::stoul(hexByte, nullptr, 16))); 
  }

  name = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  type = (Section::SectionType)readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  virtualAddress = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  offsetInFile= readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  locationCounter = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  sizeOfEntry = readAndConvertFromLittleEndian(currIdx, bytes);
  currIdx += sizeof(size_t);

  return new Section(name, type, virtualAddress, offsetInFile, locationCounter, sizeOfEntry, idx);
}
std::vector<Section *> Section::readSectionHeader(const std::string &fileName, const size_t &startOfHeader, const size_t& numOfSections)
{
  std::vector<Section*> arrayOfSections;
  size_t offsetInFile, numOfRows = startOfHeader / 4, numOfCol = startOfHeader % 4;
  std::ifstream inputFile(fileName);
  offsetInFile = numOfRows * 13 + numOfCol;

  inputFile.seekg(offsetInFile);

  for(size_t i = 0; i < numOfSections; i++)
  {
    arrayOfSections.push_back(readSectionFromSectionHeader(inputFile, i));
  }
  return arrayOfSections;
  
}

void Section::makeContentOfSectionsNames()
{
  tableOfSectionString->makeContentOfSection();
}

static std::vector<uint8_t> convertToLittleEndiand(size_t classMember)
{
  std::vector<uint8_t> littleEndianFormat;
  for(size_t i = 0; i < sizeof(size_t); i++)
  {
    littleEndianFormat.push_back(static_cast<uint8_t>((classMember >> (i * 8)) & 0xFF));
  }
  return littleEndianFormat;
}

std::vector<uint8_t> Section::getLittleEndiandOfSection()
{
  std::vector<uint8_t> sectionHeader;
  std::vector<uint8_t> container;

  container = convertToLittleEndiand(name);
  sectionHeader.insert(sectionHeader.end(), container.begin(), container.end());

  container = convertToLittleEndiand(static_cast<size_t>(type));
  sectionHeader.insert(sectionHeader.end(), container.begin(), container.end());

  container = convertToLittleEndiand(0);
  sectionHeader.insert(sectionHeader.end(), container.begin(), container.end());

  container = convertToLittleEndiand(offsetInFile);
  sectionHeader.insert(sectionHeader.end(), container.begin(), container.end());

  container = convertToLittleEndiand(locationCounter);
  sectionHeader.insert(sectionHeader.end(), container.begin(), container.end());

  if(type == SectionType::RelaSection)
  {
    container = convertToLittleEndiand(4 * sizeof(size_t));
  }
  else if (type == SectionType::SymTabSection)
  {
    container = convertToLittleEndiand(7 * sizeof(size_t));
  }
  else 
  {
    container = convertToLittleEndiand(0);
  }
  sectionHeader.insert(sectionHeader.end(), container.begin(), container.end());

  return sectionHeader;

}

std::string Section::getTextFormatOfSection()
{
  std::string stringFormat;
  stringFormat = "Name: " + std::to_string(name) + 
    ", Type: " + std::to_string(static_cast<size_t>(type)) + 
    ", Virtual Address: " + std::to_string(0) + 
    ", Offset in ELF file: " + std::to_string(offsetInFile) + 
    ", Size of Section: " + std::to_string(locationCounter) + 
    ", Size of Entry: ";
  if(type == SectionType::RelaSection)
  {
    stringFormat += std::to_string(4 * sizeof(size_t));
  }
  else if (type == SectionType::SymTabSection)
  {
    stringFormat += std::to_string(7 * sizeof(size_t));
  }
  else 
  {
    stringFormat += std::to_string(0);
  }

  return stringFormat;

}

void Section::textRepresentationOfInstruction(const std::string& command, const std::vector<Argument>& arguments)
{
  std::string instr;
  instr.append(command);
  instr.push_back(' ');
  if(!arguments.empty())
  {
    int lastIdx = arguments.size() - 1;
    for(size_t i = 0; i < arguments.size() - 1; i++)
    {
      switch(arguments[i].addressing)
      {
        case AddressingType::RegisterDirect:
          instr.append("%r");
          instr.append(std::to_string(static_cast<unsigned int>(arguments[i].registerNum)));
          break;
        case AddressingType::Immediate:
          if(command == "jmp" || command == "call" || command == "beq" || 
          command == "bgt" || command == "bne")
          {
            instr.append(arguments[i].variable);
          }
          else
          {
            instr.push_back('$');
            instr.append(arguments[i].variable);
          }
          break;
        case AddressingType::MemoryDirect:
          instr.append(arguments[i].variable);
          break;
        case AddressingType::RegisterIndirect:
          instr.append("[%r");
          instr.append(std::to_string(static_cast<unsigned int>(arguments[i].registerNum)));
          if(arguments[i].type == ArgumentType::Register)
          {
            instr.push_back(']');
          }
          else
          {
            instr.append(" + ");
            instr.append(arguments[i].variable);
            instr.push_back(']');
          }
          break;
      }
      instr.append(", ");
    }

    if(arguments.size() >= 1)
    {
      switch(arguments[lastIdx].addressing)
      {
        case AddressingType::RegisterDirect:
          instr.append("%r");
          instr.append(std::to_string(static_cast<unsigned int>(arguments[lastIdx].registerNum)));
          break;
        case AddressingType::Immediate:
          if(command == "jmp" || command == "call" || command == "beq" || 
          command == "bgt" || command == "bne")
          {
            instr.append(arguments[lastIdx].variable);
          }
          else
          {
            instr.push_back('$');
            instr.append(arguments[lastIdx].variable);
          }
          break;
        case AddressingType::MemoryDirect:
          instr.append(arguments[lastIdx].variable);
          break;
        case AddressingType::RegisterIndirect:
          instr.append("[%r");
          instr.append(std::to_string(static_cast<unsigned int>(arguments[lastIdx].registerNum)));
          if(arguments[lastIdx].type == ArgumentType::Register)
          {
            instr.push_back(']');
          }
          else
          {
            instr.append(" + ");
            instr.append(arguments[lastIdx].variable);
            instr.push_back(']');
          }
          break;
      }
    }
  }
  
  textContent.push_back(instr);
}
