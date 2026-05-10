#ifndef _DIRECTIVE_FUNCTIONS_HPP_
#define _DIRECTIVE_FUNCTIONS_HPP_

#include <string>
#include <vector>
#include "expression_tokens.hpp"
#include "instructions.hpp"

enum class MacroParameterType
{
  Literal, 
  Symbol,
  String,
  Expression
};

struct MacroParameter
{
  MacroParameterType type; 
  std::string stringValue;
  int integerValue;
  std::vector<Token> expression;
};

void directiveGlobal(const std::vector<MacroParameter>& parameters);
void exceptionDirectiveGlobal(const std::vector<MacroParameter>& parameters);


void directiveSection(const std::vector<MacroParameter>& parameters);
void exceptionDirectiveSection(const std::vector<MacroParameter>& parameters);


void directiveExtern(const std::vector<MacroParameter>& parameters);
void exceptionDirectiveExtern(const std::vector<MacroParameter>& parameters);


void directiveWord(const std::vector<MacroParameter>& parameters);
void exceptionDirectiveWord(const std::vector<MacroParameter>& parameters);


void directiveEqu(const std::vector<MacroParameter>& parameters);
void exceptionDirectiveEqu(const std::vector<MacroParameter>& parameters);

void directiveSkip(const std::vector<MacroParameter>& parameters);
void exceptionDirectiveSkip(const std::vector<MacroParameter>& parameters);


void directiveAscii(const std::vector<MacroParameter>& parameters);
void exceptionDirectiveAscii(const std::vector<MacroParameter>& parameters);


#endif