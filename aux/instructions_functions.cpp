#include "instructions_functions.hpp"
#include "../assembler.hpp"
#include "../section.hpp"
#include "instructions.hpp"
#include "exceptions.hpp"
#include "../symbol/symbol_table.hpp"
#include "../macro/macro_table.hpp"
#include <iostream>
#include <algorithm>

enum class UnconditionalJumpType 
{
  Call,
  Jump
};

enum class ConditionalJumpType 
{
  Equal,
  NotEqual,
  Greater
};

enum class ArithmeticOperationType
{
  Add,
  Sub, 
  Mul,
  Div
};

enum class LogicalOpetaionType
{
  Not,
  And,
  Or, 
  Xor
};

enum class ShiftOperationType
{
  Left, 
  Right
};

static uint32_t resolveLiteral(const std::string& literal)
{
    std::size_t pos = 0;
    bool negative = false;

    if (pos < literal.size() &&
        (literal[pos] == '+' || literal[pos] == '-'))
    {
        negative = (literal[pos] == '-');
        ++pos;
    }

    int base = 10;
    if (pos + 1 < literal.size() &&
        literal[pos] == '0' &&
        (literal[pos + 1] == 'x' || literal[pos + 1] == 'X'))
    {
        base = 16;
        pos += 2;   // skip 0x
    }
    else if (pos + 1 < literal.size() &&
             literal[pos] == '0' &&
             (literal[pos + 1] == 'b' || literal[pos + 1] == 'B'))
    {
        base = 2;
        pos += 2; 
    }

    uint32_t value = static_cast<uint32_t>(
        std::stoul(literal.substr(pos), nullptr, base)
    );

    if (negative) {
        value = static_cast<uint32_t>(-static_cast<int32_t>(value));
    }

    return value;
}

std::vector<uint8_t> transformLoadInstruction(const uint8_t& destReg, uint32_t operand)
{
  std::vector<uint8_t> instr;
  uint8_t victimReg;
  uint8_t dirtyReg = 13;

  for(size_t i = 1; i <= 12; i++)
  {
    if(i != destReg)
    {
      victimReg = i;
      break;
    }
  }
  //PUSH victimReg ON STACK
  instr.push_back(0x81); 
  instr.push_back(0xE0);
  instr.push_back((victimReg << 4) | 0xF);
  instr.push_back(0xFC);

  //LOAD 8 IN victimReg
  instr.push_back(0x91);
  instr.push_back(victimReg << 4);
  instr.push_back(0x00);
  instr.push_back(0x08);

  //LOAD value[31:24]
  instr.push_back(0x91);
  instr.push_back(dirtyReg << 4);
  instr.push_back(0x00);
  instr.push_back((operand >> 24) & 0xFF);

  //SHIFT arguments[0].registerNum << victimReg 
  instr.push_back(0x70);
  instr.push_back((dirtyReg << 4) | (dirtyReg & 0x0F));
  instr.push_back(victimReg << 4);
  instr.push_back(0x00);


  //LOAD REST OF THE BYTES
  for(size_t i = 1; i <= 3; i++)
  {
    instr.push_back(0x91);
    instr.push_back((dirtyReg << 4) | (dirtyReg & 0x0F));
    instr.push_back(0x00);
    instr.push_back((operand >> (8 * (3 - i))) & 0xFF);
    if(i != 3)
    {
      instr.push_back(0x70);
      instr.push_back((dirtyReg << 4) | (dirtyReg & 0x0F));
      instr.push_back(victimReg << 4);
      instr.push_back(0x00);
    }
  }

  //POP victimReg
  instr.push_back(0x93);
  instr.push_back((victimReg << 4) | 0xE);
  instr.push_back(0x00);
  instr.push_back(0x04);

  //LOAD dirtyReg to destReg
  instr.push_back(0x91);
  instr.push_back((destReg << 4) | 0xD);
  instr.push_back(0x00);
  instr.push_back(0x00);

  // for(size_t i = 0; i + 3 < instr.size(); i += 4)
  // {
  //   std::reverse(instr.begin() + i, instr.begin() + i + 4);
  // }
  return instr;
} 

static void exceptionInstructionConditionalJump(const std::vector<Argument>& arguments)
{

  if(arguments.size() > 3) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instruction [.beq/bne/bgt] expects three arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instruction [.beq/bne/bgt] expects three argument",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments[0].type != ArgumentType::Register || arguments[1].type != ArgumentType::Register || (arguments[2].type != ArgumentType::OperandLiteral && arguments[2].type != ArgumentType::OperandSymbol))
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.beq/bne/bgt] must have registers and symbol/literal as operand",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
}

static void exceptionInstructionsALU(const std::vector<Argument>& arguments)
{

  if(arguments.size() > 2) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "ALU instructions expects two arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "ALU instructions expects two argument",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  
  if(arguments[0].type != ArgumentType::Register || arguments[1].type != ArgumentType::Register)
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "ALU instructions must have registers as operands",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
}

static void exceptionInstructions_Not_Push_Pop(const std::vector<Argument>& arguments)
{

  if(arguments.size() > 1) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instructions [.not/push/pop] expects only one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instructions [.not/push/pop] expects only one argument",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  
  if(arguments[0].type != ArgumentType::Register)
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.not/push/pop] expects only one argument as operand",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
}

static void exceptionSpecialRegisters(const std::vector<Argument>& arguments, bool read)
{
  if(arguments.size() > 2) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instructions [.csrrd/csrwr] expects two arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instructions [.csrrd/csrwr] expects two arguments",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arguments[0].type != ArgumentType::Register || arguments[1].type != ArgumentType::Register)
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.csrrd/csrwr] must have registers as operands",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(read)
  {
    if(arguments[0].registerNum > 3)
    {
              throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Invalid special purpose register as operand",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());  
    }

  }
  else
  {
    if(arguments[1].registerNum > 3)
    {
              throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Invalid special purpose register as operand",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());  
    }
  }

}

static void exceptionInstructionLoad(const std::vector<Argument>& arguments)
{
  int signedOperand;

  if(arguments.size() > 2) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instruction [.ld] expects two arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instruction [.ld] expects two arguments",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arguments[1].type != ArgumentType::Register)
  {
    throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.ld] expects register as second argument",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  
  }
  if(arguments[0].type == ArgumentType::RegisterAndLiteral)
  {
    signedOperand =(int)resolveLiteral(arguments[0].variable);
    if((signedOperand < - (1 << 11) || signedOperand > 1 << 11 - 1))
    {
      throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.ld] expects (signed) literals that can be represented with 12 bits",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter()); 
    }
  }

  if(arguments[0].type == ArgumentType::RegisterAndSymbol)
  {
    Symbol* tempSymbol = SymbolTable::findSymbol(arguments[0].variable);
    if(tempSymbol)
    {
      if(tempSymbol->getDefined())
      {
            throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.ld] expects symbol which value is known in assembly time",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter()); 
  
      }
    }
    Macro* tempMacro = MacroTable::findMacro(arguments[0].variable);
    if(tempMacro)
    {
      if(tempMacro->getDefined())
      {
        if((tempMacro->getValue() < - (1 << 11) || tempMacro->getValue() > 1 << 11 - 1) && arguments[0].type == ArgumentType::RegisterAndSymbol)
        {
              throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.ld] expects symbol which value can be represented with 12 bits",
            Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter()); 
  
        }
      }
    }
  }
  
} 

static void exceptionInstructionStore(const std::vector<Argument>& arguments)
{
  int signedOperand; 
  if(arguments.size() > 2) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instruction [.st] expects two arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instruction [.st] expects two arguments",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  } 

  if(arguments[0].type != ArgumentType::Register)
  {
    throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.st] expects register as first argument",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }
  if(arguments[1].addressing == AddressingType::Immediate)
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidAddressing, "Instruction [.st] can't have immediate addressing",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arguments[1].type == ArgumentType::RegisterAndLiteral)
  {
    signedOperand = resolveLiteral(arguments[1].variable);
    if((signedOperand < - (1 << 11) || signedOperand > 1 << 11 - 1))
    {
      throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.st] expects (signed) literals that can be represented with 12 bits",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter()); 
    }
  }

  if(arguments[1].type == ArgumentType::RegisterAndSymbol)
  {
    Symbol* tempSymbol = SymbolTable::findSymbol(arguments[1].variable);
    if(tempSymbol)
    {
      if(tempSymbol->getDefined())
      {
            throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.st] expects symbol which value is known in assembly time",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter()); 

      }
    }
    Macro* tempMacro = MacroTable::findMacro(arguments[1].variable);
    if(tempMacro)
    {
      if(tempMacro->getDefined())
      {
        if((tempMacro->getValue() < - (1 << 11) || tempMacro->getValue() > 1 << 11 - 1))
        {
              throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.st] expects symbol which value can be represented with 12 bits",
            Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter()); 

        }
      }

    }

  }
}

std::vector<uint8_t> instructionHalt(const std::vector<Argument> &arguments)
{
  if(arguments.size() > 0)
  {
        throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instruction [.halt] doesn't have arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  return std::vector<uint8_t>{0x00, 0x00, 0x00, 0x00};
}

std::vector<uint8_t> instructrionSoftwareInterrupt(const std::vector<Argument> &arguments)
{
  if(arguments.size() > 0)
  {
        throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instruction [.int] doesn't have arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }
  return std::vector<uint8_t>{0x10, 0x00, 0x00, 0x00};
}

static std::vector<uint8_t> instructionJumpOrCall(const std::vector<Argument> &arguments, const UnconditionalJumpType& typeOfJump)
{
  std::vector<uint8_t> instr;
  uint32_t operand;
  Argument arg = arguments[0];
  if(arguments.size() > 1) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instructions [.jmp/call] expects only one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instructions [.jmp/call] expects only one argument",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arg.type != ArgumentType::OperandLiteral && arg.type != ArgumentType::OperandSymbol)
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.jmp/call] expects only literal or symbol",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
  }
  if(arg.type == ArgumentType::OperandLiteral)
  {
    operand = resolveLiteral(arg.variable);
    instr = transformLoadInstruction(13, operand);
  }
  else 
  {
    if(Instructions::resolveSymbol(arg.variable, &operand))
    {
      if(int(operand) <  0) 
      {
          throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.jmp/call] expects only positive literal or symbol",
        Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
      }
      instr =  transformLoadInstruction(13, operand);
    }
    else
    {
      instr.push_back(0x91);
      instr.push_back(13 << 4);
      instr.push_back(0x00);
      instr.push_back(0x00);
    }
  }
  if(typeOfJump == UnconditionalJumpType::Call)
  {
    instr.push_back(0x20);
  }
  else
  {
    instr.push_back(0x30);
  }
  instr.push_back(13 << 4);
  instr.push_back(0x00);
  instr.push_back(0x00);

  return instr;
}

std::vector<uint8_t> instructionJump(const std::vector<Argument> &arguments)
{
  return instructionJumpOrCall(arguments, UnconditionalJumpType::Jump);
}

std::vector<uint8_t> instructionCall(const std::vector<Argument> &arguments)
{
  return instructionJumpOrCall(arguments, UnconditionalJumpType::Call);
}

static std::vector<uint8_t> instructionConditionalJump(const std::vector<Argument> &arguments, const ConditionalJumpType &typeOfJump)
{
  std::vector<uint8_t> instr;
  uint32_t operand;

  exceptionInstructionConditionalJump(arguments);
  if(arguments[2].type == ArgumentType::OperandLiteral)
  {
    operand = resolveLiteral(arguments[2].variable);
    instr = transformLoadInstruction(13, operand);
  }
  else
  {
    if(Instructions::resolveSymbol(arguments[2].variable, &operand))
    {
      if((int)operand < 0)
      {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.beq/bnt/bgt] can only have positive value for adresses",
        Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

      }
      instr = transformLoadInstruction(13, operand);
    }
    else
    {
      instr.push_back(0x91);
      instr.push_back(13 << 4);
      instr.push_back(0x00);
      instr.push_back(0x00);
    }
  }
  switch (typeOfJump)
  {
  case ConditionalJumpType::Equal:
    instr.push_back(0x31);
    break;
  case ConditionalJumpType::NotEqual:
    instr.push_back(0x32);
    break;
  
  case ConditionalJumpType::Greater:
    instr.push_back(0x33);
    break;
  }

  instr.push_back((13 << 4) | (arguments[0].registerNum & 0xF));
  instr.push_back(arguments[1].registerNum << 4);
  instr.push_back(0x00);

  return instr;
}

std::vector<uint8_t> instuctionJumpEqual(const std::vector<Argument> &arguments)
{
  return instructionConditionalJump(arguments, ConditionalJumpType::Equal);
}

std::vector<uint8_t> instuctionJumpNotEqual(const std::vector<Argument> &arguments)
{
  return instructionConditionalJump(arguments, ConditionalJumpType::NotEqual);
}

std::vector<uint8_t> instuctionJumpGreater(const std::vector<Argument> &arguments)
{
  return instructionConditionalJump(arguments, ConditionalJumpType::Greater);
}

std::vector<uint8_t> instructionAtomicExchange(const std::vector<Argument> &arguments)
{
  exceptionInstructionsALU(arguments);
  std::vector<uint8_t> instr{0x40};
  instr.push_back(arguments[0].registerNum);
  instr.push_back(arguments[1].registerNum << 4);
  instr.push_back(0x00);
  return instr;
}


static std::vector<uint8_t> instructionArithmeticOperation(const std::vector<Argument>& arguments, const ArithmeticOperationType& typeOfOperation)
{
  exceptionInstructionsALU(arguments);
  std::vector<uint8_t> instr;
  switch (typeOfOperation)
  {
  case ArithmeticOperationType::Add:
    instr.push_back(0x50);
    break;

  case ArithmeticOperationType::Sub:
    instr.push_back(0x51);
    break;

  case ArithmeticOperationType::Mul:
    instr.push_back(0x52);
    break;
  
  case ArithmeticOperationType::Div:
    instr.push_back(0x53);
    break;
  }
  instr.push_back((arguments[1].registerNum << 4) | arguments[1].registerNum);
  instr.push_back(arguments[0].registerNum << 4);
  instr.push_back(0x00);

  return instr;
}

std::vector<uint8_t> instructionAdd(const std::vector<Argument> &arguments)
{
  return instructionArithmeticOperation(arguments, ArithmeticOperationType::Add);
}

std::vector<uint8_t> instructionSub(const std::vector<Argument> &arguments)
{
  return instructionArithmeticOperation(arguments, ArithmeticOperationType::Sub);
}

std::vector<uint8_t> instructionMul(const std::vector<Argument> &arguments)
{
  return instructionArithmeticOperation(arguments, ArithmeticOperationType::Mul);
}

std::vector<uint8_t> instructionDiv(const std::vector<Argument> &arguments)
{
  return instructionArithmeticOperation(arguments, ArithmeticOperationType::Div);
}


static std::vector<uint8_t> instructionLogicalOperation(const std::vector<Argument> &arguments, const LogicalOpetaionType& typeOfOperation)
{
  if(typeOfOperation != LogicalOpetaionType::Not)
  {
    exceptionInstructionsALU(arguments);
  }
  else
  {
    exceptionInstructions_Not_Push_Pop(arguments);
  }

  std::vector<uint8_t> instr;
  switch (typeOfOperation)
  {
  case LogicalOpetaionType::Not:
    instr.push_back(0x60);
    break;
  case LogicalOpetaionType::And:
    instr.push_back(0x61);
    break;
  case LogicalOpetaionType::Or:
    instr.push_back(0x62);
    break;
  case LogicalOpetaionType::Xor:
    instr.push_back(0x63);
    break;
  }
  if(typeOfOperation == LogicalOpetaionType::Not)
  {
    instr.push_back((arguments[0].registerNum << 4) | arguments[0].registerNum);
    instr.push_back(0x00);
  }
  else
  {
    instr.push_back((arguments[1].registerNum << 4) | arguments[1].registerNum);
    instr.push_back(arguments[0].registerNum << 4);
  }
  
  instr.push_back(0x00);

  return instr;
}

std::vector<uint8_t> instructionNot(const std::vector<Argument> &arguments)
{
  return instructionLogicalOperation(arguments, LogicalOpetaionType::Not);
}

std::vector<uint8_t> instructionAnd(const std::vector<Argument> &arguments)
{
  return instructionLogicalOperation(arguments, LogicalOpetaionType::And);
}

std::vector<uint8_t> instructionOr(const std::vector<Argument> &arguments)
{
  return instructionLogicalOperation(arguments, LogicalOpetaionType::Or);
}

std::vector<uint8_t> instructionXor(const std::vector<Argument> &arguments)
{
  return instructionLogicalOperation(arguments, LogicalOpetaionType::Xor);
}


static std::vector<uint8_t> instructionShiftOperation(const std::vector<Argument> &arguments, const ShiftOperationType& typeOfOperation)
{
  exceptionInstructionsALU(arguments);
  std::vector<uint8_t> instr;
  switch(typeOfOperation)
  {
    case ShiftOperationType::Left:
    instr.push_back(0x70);
    break;

    case ShiftOperationType::Right:
    instr.push_back(0x71);
    break;
  }
  instr.push_back((arguments[1].registerNum << 4) | arguments[1].registerNum);
  instr.push_back(arguments[0].registerNum << 4);
  instr.push_back(0x00);

  return instr;
}

std::vector<uint8_t> instructionShiftLeft(const std::vector<Argument> &arguments)
{
  return instructionShiftOperation(arguments, ShiftOperationType::Left);
}

std::vector<uint8_t> instructionShiftRight(const std::vector<Argument> &arguments)
{
  return instructionShiftOperation(arguments, ShiftOperationType::Right);
}

std::vector<uint8_t> instructionPush(const std::vector<Argument> &arguments)
{
  exceptionInstructions_Not_Push_Pop(arguments);
  std::vector<uint8_t> instr{0x81, 0xE0};
  instr.push_back((arguments[0].registerNum << 4) | 0xF);
  instr.push_back(0xFC);
  return instr;
}

std::vector<uint8_t> instructionPop(const std::vector<Argument> &arguments)
{
  exceptionInstructions_Not_Push_Pop(arguments);
  std::vector<uint8_t> instr{0x93};
  instr.push_back((arguments[0].registerNum << 4) | 0xE);
  instr.push_back(0x00);
  instr.push_back(0x04);
  return instr;
}

std::vector<uint8_t> instructionReturnFromInterrupt(const std::vector<Argument> &arguments)
{
    if(arguments.size() > 0)
  {
        throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instruction [.halt] doesn't have arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }
  // POP PC;
  std::vector<uint8_t> instr{0x93};
  instr.push_back((0xF << 4) | 0xE);
  instr.push_back(0x00);
  instr.push_back(0x04);

  //POP STATUS;
  instr.push_back(0x97);
  instr.push_back(0x0E);
  instr.push_back(0x00);
  instr.push_back(0x04);

  return instr;
}

std::vector<uint8_t> instructionReturnFromFunction(const std::vector<Argument> &arguments)
{
  if(arguments.size() > 0)
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instruction [.ret] doesn't have arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  
  std::vector<uint8_t> instr{0x93};
  instr.push_back(0xFE);
  instr.push_back(0x00);
  instr.push_back(0x04);
  return instr;
}

std::vector<uint8_t> instructionReadFromCSRegister(const std::vector<Argument> &arguments)
{
  exceptionSpecialRegisters(arguments, true);
  std::vector<uint8_t> instr{0x90};
  instr.push_back((arguments[1].registerNum << 4) | arguments[0].registerNum);
  instr.push_back(0x00);
  instr.push_back(0x00);
  return instr;
}

std::vector<uint8_t> instructionWriteToCSRegister(const std::vector<Argument> &arguments)
{
  exceptionSpecialRegisters(arguments, false);
  std::vector<uint8_t> instr{0x94};
  instr.push_back((arguments[1].registerNum << 4) | arguments[0].registerNum);
  instr.push_back(0x00);
  instr.push_back(0x00);
  return instr;
}
static std::vector<uint8_t>transformMemoryDirectStore(const std::vector<Argument>& arguments)
{
  std::vector<uint8_t> instr, tempInstr;
  uint32_t operand;
  uint8_t victimReg = 13;
  // for(size_t i = 1; i <= 13; i++)
  // {
  //   if(i == arguments[0].registerNum)
  //   {
  //     victimReg = i;
  //     break;
  //   }
  // }

  //PUSH victimReg
  // instr.push_back(0x81); 
  // instr.push_back(0xE0);
  // instr.push_back((victimReg << 4) | 0xF);
  // instr.push_back(0xFC);

  // Assembler::getCurrentSection()->incrementLocationCounter(4);

  if(arguments[1].type == ArgumentType::OperandLiteral)
  {
    operand = resolveLiteral(arguments[1].variable);
    tempInstr = transformLoadInstruction(victimReg, operand);
    instr.insert(instr.end(), tempInstr.begin(), tempInstr.end());
  }
  else 
  {
    if(Instructions::resolveSymbol(arguments[1].variable, &operand))
    {
      // if((int)operand < 0)
      // {
      //       throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.ld] expects positive value for memory direct addressing",
      // Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
      // }
      tempInstr = transformLoadInstruction(victimReg, operand);
      instr.insert(instr.end(), tempInstr.begin(), tempInstr.end());
    }
    else
    {
      instr.push_back(0x91);
      instr.push_back(victimReg << 4);
      instr.push_back(0x00);
      instr.push_back(0x00);
    }
  }

  //Assembler::getCurrentSection()->incrementLocationCounter(-4);

  instr.push_back(0x80);
  instr.push_back(victimReg << 4);
  instr.push_back(arguments[0].registerNum << 4);
  instr.push_back(0x00);

  //POP victimReg
  // instr.push_back(0x93);
  // instr.push_back((victimReg << 4) | 0xE);
  // instr.push_back(0x00);
  // instr.push_back(0x04);

  return instr;
}
std::vector<uint8_t> instructionStore(const std::vector<Argument> &arguments)
{
  std::vector<uint8_t> instr;
  uint32_t operand;
  int signedOperand;
  exceptionInstructionStore(arguments);
  if(arguments[1].addressing == AddressingType::RegisterDirect)
  {
    instr.push_back(0x91);
    instr.push_back((arguments[1].registerNum << 4) | arguments[0].registerNum);
    instr.push_back(0x00);
    instr.push_back(0x00);
    return instr;
  } 
  else if(arguments[1].addressing == AddressingType::MemoryDirect)
  {
    return transformMemoryDirectStore(arguments);
  }
  else if(arguments[1].addressing == AddressingType::RegisterIndirect)
  {
    if(arguments[1].type == ArgumentType::Register)
    {
      instr.push_back(0x80);
      instr.push_back(arguments[1].registerNum << 4);
      instr.push_back(arguments[0].registerNum << 4);
      instr.push_back(0x00);
      return instr;
    }
    else
    {
      instr.push_back(0x80);
      instr.push_back(arguments[1].registerNum << 4);
      if(arguments[1].type == ArgumentType::RegisterAndLiteral)
      {
        operand = resolveLiteral(arguments[1].variable);
        instr.push_back((arguments[0].registerNum << 4) | ((operand >> 8) & 0xF));
        instr.push_back(operand & 0xFF);
      }
      else
      {
        if(Instructions::resolveSymbol(arguments[1].variable, &operand))
        {
          instr.push_back((arguments[0].registerNum << 4) | ((operand >> 8) & 0xF));
          instr.push_back(operand & 0xFF);
        }
        else
        {
          instr.push_back(arguments[0].registerNum << 4);
          instr.push_back(0x00);
        }
      }
      return instr; 
    }
  }



}

static std::vector<uint8_t> instructionImmediateLoad(const std::vector<Argument>& arguments)
{
  std::vector<uint8_t> instr;
  uint32_t operand;
  if(arguments[0].type == ArgumentType::OperandLiteral)
  {
    operand = resolveLiteral(arguments[0].variable);
    instr =  transformLoadInstruction(arguments[1].registerNum, operand);
    // for(size_t i = 0; i + 3 < instr.size(); i += 4)
    // {
    //   std::reverse(instr.begin() + i, instr.begin() + i + 4);
    // }
    return instr;
  }
  else
  {
    if(Instructions::resolveSymbol(arguments[0].variable, &operand))
    {
      instr =  transformLoadInstruction(arguments[1].registerNum, operand);
      // for(size_t i = 0; i + 3 < instr.size(); i += 4)
      // {
      //   std::reverse(instr.begin() + i, instr.begin() + i + 4);
      // }
      return instr;
    }
    else
    {
      instr.push_back(0x91);
      instr.push_back(arguments[1].registerNum << 4);
      instr.push_back(0x00);
      instr.push_back(0x00);
      //std::reverse(instr.begin(), instr.end());
      return instr;
    }
  }
}
static std::vector<uint8_t> transformMemoryDirectLoad(const std::vector<Argument>& arguments)
{
  std::vector<uint8_t> instr;
  uint32_t operand;
  if(arguments[0].type == ArgumentType::OperandLiteral)
  {
    operand = resolveLiteral(arguments[0].variable);
    instr =  transformLoadInstruction(arguments[1].registerNum, operand);
  }
  else
  {
    if(Instructions::resolveSymbol(arguments[0].variable, &operand))
    {
      // if((int)operand < 0)
      // {
      //       throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instruction [.ld] expects positive value for memory direct addressing",
      // Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  
      // }
      instr =  transformLoadInstruction(arguments[1].registerNum, operand);
    }
    else
    {
      instr.push_back(0x91);
      instr.push_back(arguments[1].registerNum << 4);
      instr.push_back(0x00);
      instr.push_back(0x00);
    }
  }
  instr.push_back(0x92);
  instr.push_back((arguments[1].registerNum << 4) | (arguments[1].registerNum & 0xF));
  instr.push_back(0x00);
  instr.push_back(0X00);

  // for(size_t i = 0; i + 3 < instr.size(); i += 4)
  // {
  //   std::reverse(instr.begin() + i, instr.begin() + i + 4);
  // }
  return instr;
}
std::vector<uint8_t> instructionLoad(const std::vector<Argument> &arguments)
{
  std::vector<uint8_t> instr;
  uint32_t operand;
  int signedOperand;
  exceptionInstructionLoad(arguments);
  uint8_t secondField = (arguments[1].registerNum << 4); 
  if(arguments[0].addressing == AddressingType::Immediate)
  {
   
    return instructionImmediateLoad(arguments);
  }
  else if(arguments[0].addressing == AddressingType::MemoryDirect)
  {
    return transformMemoryDirectLoad(arguments);
  }
  else if(arguments[0].addressing == AddressingType::RegisterDirect)
  {
    instr.push_back(0x91);
    instr.push_back((arguments[1].registerNum << 4) | (arguments[0].registerNum & 0xF));
    instr.push_back(0x00);
    instr.push_back(0x00);
    return instr;
  }
  else if(arguments[0].addressing == AddressingType::RegisterIndirect)
  {
    if(arguments[0].type == ArgumentType::Register)
    {
      instr.push_back(0x92);
      instr.push_back((arguments[1].registerNum << 4) | (arguments[0].registerNum & 0xF));
      instr.push_back(0x00);
      instr.push_back(0x00);
      return instr;
    }
    else
    {
      instr.push_back(0x92);
      instr.push_back((arguments[1].registerNum << 4) | (arguments[0].registerNum & 0xF));
      if(arguments[0].type == ArgumentType::RegisterAndLiteral)
      {
        operand = resolveLiteral(arguments[0].variable);
        instr.push_back((operand >> 8) & 0xF);
        instr.push_back(operand & 0xFF);
      }
      else
      {
        if(Instructions::resolveSymbol(arguments[0].variable, &operand))
        {
          instr.push_back((operand >> 8) & 0xF);
          instr.push_back(operand & 0xFF);
        }
        else
        {
          instr.push_back(0x00);
          instr.push_back(0x00);
        }
      }
      return instr;
    }
  }
}
