#include "instructions.hpp"
#include "instructions_functions.hpp"
#include "../macro/macro_table.hpp"
#include "../symbol/symbol_table.hpp"
#include "forward_reference.hpp"
#include "../assembler.hpp"
#include <iostream>
#include "../relocation/relocation_table.hpp"

std::unordered_map<std::string, std::vector<uint8_t> (*)(const std::vector<Argument>& arguments)> Instructions::tableOfInstructions = {
  {"halt", instructionHalt},
  {"int", instructrionSoftwareInterrupt},
  {"call", instructionCall},
  {"jmp", instructionJump},
  {"beq", instuctionJumpEqual},
  {"bne", instuctionJumpNotEqual},
  {"bgt", instuctionJumpGreater},
  {"xchg", instructionAtomicExchange},
  {"add", instructionAdd},
  {"sub", instructionSub},
  {"mul", instructionMul},
  {"div", instructionDiv},
  {"not", instructionNot},
  {"and", instructionAnd},
  {"or", instructionOr},
  {"xor", instructionXor},
  {"shl", instructionShiftLeft},
  {"shr", instructionShiftRight},
  {"push", instructionPush},
  {"pop", instructionPop},
  {"ret", instructionPop},
  {"iret", instructionReturnFromInterrupt},
  {"csrrd", instructionReadFromCSRegister},
  {"csrwr", instructionWriteToCSRegister},
  {"st", instructionStore},
  {"ld", instructionLoad}
};

std::vector<uint8_t> Instructions::translate(const std::string &instruction_name, const std::vector<Argument> &arguments)
{

  return tableOfInstructions[instruction_name](arguments);
}

bool Instructions::resolveSymbol(const std::string &symbol, uint32_t* value)
{
  Symbol* tempSymbol = SymbolTable::findSymbol(symbol);
  Macro* tempMacro = MacroTable::findMacro(symbol);
  

  if(!tempSymbol && !tempMacro)
  {
    Section* currentSection = Assembler::getCurrentSection();
    ForwardReference* newForwardReference = new ForwardReference(currentSection->getIdxOfSection(), currentSection->getLocationCounter());

    Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, 0, 
                                   Symbol::Binding::NoBinding, Symbol::Type::NoType, Symbol::Scope::NoScope, false, newForwardReference);
    SymbolTable::addSymbol(symbol, newSymbol);
    return false;
  }
  else if(tempSymbol)
  {
    if(tempSymbol->getDefined())
    {
      *value = tempSymbol->getValue();
      Section* currentSection = Assembler::getCurrentSection();
      RelocationEntry* newReloc = new RelocationEntry(currentSection->getLocationCounter(),
                                                      currentSection->getIdxOfSection(), tempSymbol->getIdx(), 0);
      RelocationTable::addRelocationEntry(newReloc);
      return true;
    }
    else
    {
      Section* currentSection = Assembler::getCurrentSection();
      ForwardReference* newForwardReference = new ForwardReference(currentSection->getIdxOfSection(), currentSection->getLocationCounter());
      tempSymbol->addForwardReference(newForwardReference);
      return false;
    }
  }
  else
  {
    if(tempMacro->getDefined())
    {
      *value = tempMacro->getValue();
      return true;
    }
    else
    {
      Section* currentSection = Assembler::getCurrentSection();
      ForwardReference* newForwardReference = new ForwardReference(currentSection->getIdxOfSection(), currentSection->getLocationCounter());
      tempMacro->addForwardReference(newForwardReference);
      return false;
    }
  }
}
