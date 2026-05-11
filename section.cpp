#include "section.hpp"
#include "assembler.hpp"
#include "symbol/symbol_table.hpp"
#include "aux/instructions.hpp"
#include "aux/auxiliary_func.hpp"
#include "macro/macro_table.hpp"
#include "aux/exceptions.hpp"

StringTable* Section::tableOfSectionString = new StringTable(StringTable::STType::SectionName);

Section::Section(const std::string& sectionName) : name(tableOfSectionString->getOffset()), locationCounter(0), idxSection(Assembler::getNumberOfSections())
{ 
  
  Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, idxSection, Symbol::Binding::NoBinding, Symbol::Type::Section, Symbol::Scope::NoScope, true);
  SymbolTable::addSymbol(sectionName, newSymbol);
  tableOfSectionString->addString(sectionName);
}


int Section::translateInstruction(const std::string &instruction, const std::vector<Argument> &arguments)
{
  std::vector<uint8_t> binaryInstruction = Instructions::translate(instruction, arguments);
  textRepresentationOfInstruction(instruction, arguments);
  if(instruction != "iret")
  {
    content.insert(content.end(), binaryInstruction.rbegin(), binaryInstruction.rend());
    locationCounter += 4;
  }
  else
  {
    for(int i = 3; i >= 0; i--)
    {
      content.push_back(binaryInstruction[i]);
    }
    for(int i = 7; i >= 4; i--)
    {
      content.push_back(binaryInstruction[i]);
    }
    locationCounter += 8;
  }
  
  
  return 0;
}

int Section::executeDirective(const std::string &command, const std::vector<MacroParameter>& parameters)
{

  Directives::execute(command, parameters);

  return 0;
}

void Section::defineSymbol(const std::string &symbolName)
{
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
    tempSymol->setScope(Symbol::Scope::Local);
    tempSymol->setDefined();
  }
  else
  {
    Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, locationCounter, idxSection, Symbol::Binding::NoBinding, Symbol::Type::Object, Symbol::Scope::Local, true);
    SymbolTable::addSymbol(symbolName, newSymbol);
  }
  MacroTable::tryToResolveAllMacros(std::vector<std::string>{symbolName}, true);
}

void Section::callocMemory(size_t sizeOfAllocation)
{
  locationCounter += sizeOfAllocation;
  content.insert(content.end(), sizeOfAllocation, 0);
  textContent.insert(textContent.end(), sizeOfAllocation, "\n");
  
}

void Section::insertString(const std::string &stringToInsert)
{
  size_t endOfString = stringToInsert.find("\"", 1);
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

void Section::textRepresentationOfInstruction(const std::string& command, const std::vector<Argument>& arguments)
{
  std::string instr;
  int lastIdx = arguments.size() - 1;
  instr.append(command);
  instr.push_back(' ');
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
  textContent.push_back(instr);
}
