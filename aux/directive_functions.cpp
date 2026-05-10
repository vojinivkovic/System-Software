#include "directive_functions.hpp"
#include "../section.hpp"
#include "../assembler.hpp"
#include "../symbol/symbol_table.hpp"
#include <iostream>
#include "auxiliary_func.hpp"
#include "../macro/macro_table.hpp"
#include "exceptions.hpp"


void directiveSection(const std::vector<MacroParameter>& parameters)
{

  exceptionDirectiveSkip(parameters);
  Symbol* tempSymbol = SymbolTable::findSymbol(parameters[0].stringValue);
  Macro* tempMacro = MacroTable::findMacro(parameters[0].stringValue);
  if(tempSymbol->getType() != Symbol::Type::Section || tempMacro)
  {
    throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Directive [.section] can have arguments that is not already non-section symbol or macro", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  std::string::size_type findName = Section::findSectionInStringTable(parameters[0].stringValue);
  if(findName == std::string::npos)
  {
    Section* newSection = new Section(parameters[0].stringValue);
    Assembler::addSection(newSection);
    Assembler::setCurrentSection(newSection);
    return;
  }
  else
  {
    Section* oldSection = Assembler::findSection(findName);
    Assembler::setCurrentSection(oldSection);
  }
  return;
}

void exceptionDirectiveSection(const std::vector<MacroParameter> &parameters)
{
    if(parameters.size() > 1)
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Directive [.section] can only have one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }

  if(parameters.size() == 0)
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Directive [.section] can only have one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }

  if(parameters[0].type != MacroParameterType::Symbol)
  {
    throw AssemblerErrors(ErrorType::ErrorInvalidArgumentType, "Directive [.section] can only have argument that is symbol", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
}

void directiveGlobal(const std::vector<MacroParameter>& parameters)
{

  exceptionDirectiveGlobal(parameters);
  for(auto iParameter : parameters)
  {
    Symbol* tempSymbol = SymbolTable::findSymbol(iParameter.stringValue);
    if(tempSymbol == nullptr)
    {
      Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, 0, Symbol::Binding::Export, Symbol::Type::NoType, Symbol::Scope::Global, false);
      SymbolTable::addSymbol(iParameter.stringValue,newSymbol);
    }
    else
    {
      tempSymbol->setBinding(Symbol::Binding::Export);
      tempSymbol->setScope(Symbol::Scope::Global);
    }
  }
}

void exceptionDirectiveGlobal(const std::vector<MacroParameter> &parameters)
{
    if(parameters.size() == 0)
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Directive [.global] must have at least one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  for (auto iParameter : parameters)
  {
    if(iParameter.type != MacroParameterType::Symbol)
    {
      throw AssemblerErrors(ErrorType::ErrorInvalidArgumentType, "Directive [.global] can only have arguments that are symbols", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
    }
  }
}

void directiveExtern(const std::vector<MacroParameter>& parameters)
{

  exceptionDirectiveExtern(parameters);
  for(auto iParameter : parameters)
  {
    Symbol* tempSymbol = SymbolTable::findSymbol(iParameter.stringValue);
    if(tempSymbol == nullptr)
    {
      Symbol* newSymbol = new Symbol(SymbolTable::getNewIdxInSymbolTable(), SymbolTable::getOffsetInTableOfSymbolString(), 0, 0, 0, Symbol::Binding::Import, Symbol::Type::NoType, Symbol::Scope::Global, false);
      SymbolTable::addSymbol(iParameter.stringValue, newSymbol);
    }
    else
    {
      if(tempSymbol->getDefined() || tempSymbol->getScope() == Symbol::Scope::Local || 
          (tempSymbol->getScope() == Symbol::Scope::Global && tempSymbol->getBinding() == Symbol::Binding::Export))
      {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Directive [.extern] can be used only for the symbols that are not arleady defined in this translation unit", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
      }
    }
  }
}

void exceptionDirectiveExtern(const std::vector<MacroParameter> &parameters)
{
    if(parameters.size() == 0)
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Directive [.extern] must have at least one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  for (auto iParameter : parameters)
  {
    if(iParameter.type != MacroParameterType::Symbol)
    {
      throw AssemblerErrors(ErrorType::ErrorInvalidArgumentType, "Directive [.extern] can only have arguments that are symbols", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
    }
  }
}

void directiveWord(const std::vector<MacroParameter> &parameters)
{

  uint32_t value;
  Section* currSection = Assembler::getCurrentSection();

  exceptionDirectiveWord(parameters);

  for(auto iParameter: parameters)
  {
    if(iParameter.type == MacroParameterType::Symbol)
    {
      if(Instructions::resolveSymbol(iParameter.stringValue, &value))
      {
        currSection->insertContent(value);
      }
      else
      {
        currSection->insertContent(0);
      }
    }
    else
    {
      currSection->insertContent(iParameter.integerValue);
    }
  }
}

void exceptionDirectiveWord(const std::vector<MacroParameter> &parameters)
{

  if(parameters.size() == 0)
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Directive [.word] must have at least one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
}

void directiveEqu(const std::vector<MacroParameter> &parameters)
{
  std::string nameOfMacro = parameters[0].stringValue;
  Symbol* tempSymbol = SymbolTable::findSymbol(nameOfMacro);
  std::vector<std::string> dependencies;
  Macro* tempMacro;

  exceptionDirectiveEqu(parameters);
  for(auto iToken : parameters[1].expression)
  {
    if(iToken.type == TokenType::SYMBOL)
    {
      dependencies.push_back(iToken.symbol);
    }
  }
  
  if(tempSymbol)
  {

    SymbolTable::removeSymbolFromTable(tempSymbol);
    
    tempMacro = new Macro(MacroTable::getOffsetInTableOfMacroString(), parameters[1].expression,
                                dependencies,tempSymbol->getForwardReference());
    
  }
  else
  {
    tempMacro = new Macro(MacroTable::getOffsetInTableOfMacroString(), parameters[1].expression,
                                dependencies, {});
  }

  MacroTable::AddMacro(nameOfMacro, tempMacro);
  return;

}

void exceptionDirectiveEqu(const std::vector<MacroParameter> &parameters)
{
  std::string nameOfMacro = parameters[0].stringValue;
  Symbol* tempSymbol = SymbolTable::findSymbol(nameOfMacro);
  std::vector<std::string> dependencies;
  Macro* tempMacro;

  if(parameters.size() == 0)
  {
        throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Directive [.equ] can only have two arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }

  if(parameters.size() > 2)
  {
        throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Directive [.equ] can only have two arguments", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }
  if(parameters[0].type != MacroParameterType::Symbol)
  {
    throw AssemblerErrors(ErrorType::ErrorInvalidArgumentType, "Directive [.equ] as first argument must have symbol", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }  
  if(MacroTable::findMacro(nameOfMacro))
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "In directive [.equ] this macro is already defined", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }
  if(tempSymbol)
  {
    if(tempSymbol->getDefined())
    {
          throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "In directive [.equ] there is already symbol with same name", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

    }
  }
}

void directiveSkip(const std::vector<MacroParameter>& parameters)
{

  exceptionDirectiveSkip(parameters);

  Section* currentSection = Assembler::getCurrentSection();
  currentSection->callocMemory(parameters[0].integerValue);
}

void exceptionDirectiveSkip(const std::vector<MacroParameter> &parameters)
{

  if(parameters.size() > 1)
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Directive [.skip] can only have one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }

  if(parameters.size() == 0)
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Directive [.skip] can only have one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }

  if(parameters[0].type != MacroParameterType::Literal)
  {
    throw AssemblerErrors(ErrorType::ErrorInvalidArgumentType, "Directive [.skip] must have literal as argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }

  if(parameters[0].integerValue < 0)
  {
    throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Directive [.skip] must have at positive argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());
  }
}

void directiveAscii(const std::vector<MacroParameter>& parameters)
{

  exceptionDirectiveAscii(parameters);
  Section* currentSection = Assembler::getCurrentSection();
  currentSection->insertString(parameters[0].stringValue);
}

void exceptionDirectiveAscii(const std::vector<MacroParameter> &parameters)
{

  if(parameters.size() > 1)
  {
    throw AssemblerErrors(ErrorType::ErrorTooManyArguments, "Directive [.ascii] can only have one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }

  if(parameters.size() == 0)
  {
    throw AssemblerErrors(ErrorType::ErrorTooFewArguments, "Directive [.ascii] can only have one argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }

  if(parameters[0].type != MacroParameterType::String)
  {
        throw AssemblerErrors(ErrorType::ErrorInvalidArgument, "Directive [.ascii] must have string as argument", 
      Assembler::getCurrentSection()->getSectionName(), Assembler::getCurrentSection()->getLocationCounter());

  }
}
