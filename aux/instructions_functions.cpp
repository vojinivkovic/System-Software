#include "instructions_functions.hpp"
#include "../assembler.hpp"
#include "instructions.hpp"
#include "exceptions.hpp"

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
  uint32_t operand;
  switch (literal[1])
  {
    case 'x': 
    case 'X':
      operand = stoul(literal, nullptr, 16);
      break;
    case 'b':
    case 'B':
      operand = stoul(literal.substr(2), nullptr, 2);
      break;
    default:
      operand = stoul(literal);
      break;
    }
    return operand;
}


static void exceptionInstructionConditionalJump(const std::vector<Argument>& arguments)
{

  if(arguments.size() > 3) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instruction [.beq/bne/bgt] except three arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instruction [.beq/bne/bgt] except three argument",
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
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "ALU instructions except two arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "ALU instructions except two argument",
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
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instructions [.not/push/pop] except only one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instructions [.not/push/pop] except only one argument",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  
  if(arguments[0].type != ArgumentType::Register)
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.not/push/pop] except only one argument as operand",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
}

static void exceptionSpecialRegisters(const std::vector<Argument>& arguments, bool read)
{
  if(arguments.size() > 2) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instructions [.csrrd/csrwr] except two arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instructions [.csrrd/csrwr] except two arguments",
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
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Instructions [.jmp/call] except only one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
  if(arguments.size() == 0) 
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Instructions [.jmp/call] except only one argument",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(arg.type != ArgumentType::OperandLiteral && arg.type != ArgumentType::OperandSymbol)
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.jmp/call] except only literal or symbol",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
  }
  if(typeOfJump == UnconditionalJumpType::Call)
  {
    instr.push_back(0x20);
  }
  else
  {
    instr.push_back(0x30);
  }
  instr.push_back(0x00);

  if(arg.type == ArgumentType::OperandLiteral)
  {
    operand = resolveLiteral(arg.variable);
    instr.push_back(operand >> 8 & 0xFF);
    instr.push_back(operand & 0xFF);
  }  
  else
  {
    if(Instructions::resolveSymbol(arg.variable, &operand))
    {
      if(((int)operand) < 0)
      {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.jmp/call] can only have positive value for adresses",
        Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
 
      }
      instr.push_back(operand >> 8 & 0x0F);
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

  instr.push_back(arguments[0].registerNum);

  if(Instructions::resolveSymbol(arguments[2].variable, &operand))
  {
    if(((int)operand) < 0)
    {
      throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Instructions [.beq/bnt/bgt] can only have positive value for adresses",
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

    }
    instr.push_back((arguments[1].registerNum << 4) | (operand >> 8 & 0x0F));
    instr.push_back(operand & 0XFF);
  }
  else
  {
    instr.push_back(arguments[1].registerNum << 4);
    instr.push_back(0x00);
  }

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

std::vector<uint8_t> instructionStore(const std::vector<Argument> &arguments)
{
  std::vector<uint8_t> instr;
  uint32_t operand;
  int signedOperand;
  if(arguments[1].addressing == AddressingType::RegisterDirect)
  {
    instr.push_back(0x91);
    instr.push_back((arguments[1].registerNum << 4) | arguments[0].registerNum);
    instr.push_back(0x00);
    instr.push_back(0x00);
    return instr;
  }

  instr.push_back(0x80);
  if(arguments[1].addressing == AddressingType::RegisterIndirect)
  {
    instr.push_back(arguments[1].registerNum << 4);
  }
  else
  {
    instr.push_back(0x00);
  }

  if(arguments[1].type == ArgumentType::Register)
  {
    instr.push_back(arguments[0].registerNum << 4);
    instr.push_back(0x00);
    return instr;
  }
  else
  { 
    if(arguments[1].type == ArgumentType::RegisterAndLiteral)
    {
      signedOperand = (int)resolveLiteral(arguments[1].variable);
      instr.push_back((arguments[0].registerNum << 4) | ((uint32_t)signedOperand >> 8 & 0x0F));
      instr.push_back((uint32_t)signedOperand & 0xFF);
      return instr;
    }
    else
    {
      if(Instructions::resolveSymbol(arguments[1].variable, &operand))
      {
        instr.push_back((arguments[0].registerNum << 4) | (operand >> 8 & 0x0F));
        instr.push_back(operand & 0xFF);
      }
      else
      {
        instr.push_back(arguments[0].registerNum << 4);
        instr.push_back(0x00);
      }
      return instr;
    }
  
  }

}

std::vector<uint8_t> instructionLoad(const std::vector<Argument> &arguments)
{
  std::vector<uint8_t> instr;
  uint32_t operand;
  int signedOperand;

  uint8_t secondField = (arguments[1].registerNum << 4); 
  if(arguments[0].addressing == AddressingType::RegisterDirect || arguments[0].addressing == AddressingType::Immediate)
  {
    instr.push_back(0x91);
  }
  else
  {
    instr.push_back(0x92);
  }
  
  if(arguments[0].addressing == AddressingType::MemoryDirect || arguments[0].addressing == AddressingType::Immediate)
  {
    instr.push_back(secondField);
  }
  else
  {
    instr.push_back(secondField | arguments[0].registerNum);
  }

  if(arguments[0].addressing == AddressingType::RegisterDirect)
  {
    instr.push_back(0x00);
    instr.push_back(0x00);
  }
  else
  {
    if(arguments[0].type == ArgumentType::OperandLiteral || arguments[0].type == ArgumentType::RegisterAndLiteral)
    {

      signedOperand = (int)resolveLiteral(arguments[1].variable);
      instr.push_back((uint32_t)signedOperand >> 8 & 0x0F);
      instr.push_back((uint32_t)signedOperand & 0xFF);
    }
    else
    {
      if(Instructions::resolveSymbol(arguments[1].variable, &operand))
      {
        instr.push_back(operand >> 8 & 0x0F);
        instr.push_back(operand & 0xFF);
      }
      else
      {
        instr.push_back(0x00);
        instr.push_back(0x00);
      }

    }

  }
  return instr;
}
