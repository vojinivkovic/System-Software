#include "expression_resolver.hpp"
#include "../macro/macro.hpp"
#include "../macro/macro_table.hpp"
#include <iostream>

std::stack<Token> ExpressionResolver::auxStack;
std::vector<Token> ExpressionResolver::postfixExpression;

std::unordered_map<TokenType, PriorityValue> ExpressionResolver::priorityTable = 
{
  {TokenType::BINPLUS, {3, 3, -1, Associativity::LEFT}},
  {TokenType::BINMINUS, {3, 3, -1, Associativity::LEFT}},
  {TokenType::UNPLUS, {3, 2, 0, Associativity::RIGHT}},
  {TokenType::UNMINUS, {3, 2, 0, Associativity::RIGHT}},
  {TokenType::LPARENTHESES, {6, 0, 0, Associativity::LEFT}},
  {TokenType::RPARENTHESES, {1, 0, 0, Associativity::LEFT}}
};


void ExpressionResolver::infixToPostfix(const std::vector<Token>& expr)
{
  int currentRang = 0;
  Token tempToken;
  for(size_t i = 0; i < expr.size(); i++)
  {
    if(expr[i].type == TokenType::LITERAL || expr[i].type == TokenType::SYMBOL)
    {
      postfixExpression.push_back(expr[i]);
      currentRang++;
    }
    else
    {
      while((!auxStack.empty()) && 
      (priorityTable[expr[i].type].initialPriority <= priorityTable[auxStack.top().type].postPriority))
      {
        tempToken = auxStack.top();
        auxStack.pop();
        postfixExpression.push_back(tempToken);
        currentRang += priorityTable[tempToken.type].rang;
        
        if(currentRang < 1) 
        {
          std::cout << "Expression is not valid. " << std::endl;
          return;
        }
      }

      if(expr[i].type != TokenType::RPARENTHESES) 
      {
        auxStack.push(expr[i]);
      }
      else
      {
        auxStack.pop();
      }
    }
  }

  while(!auxStack.empty())
  {
    tempToken = auxStack.top();
    auxStack.pop();
    postfixExpression.push_back(tempToken);
    currentRang += priorityTable[tempToken.type].rang;
  }

  auxStack = {};
  if(currentRang != 1)
  {
    std::cout << "Expression is not valid. " << std::endl;
  }
}

int ExpressionResolver::evaluate()
{
  Token operand1, operand2;
  int operand1Value, operand2Value;
  Macro* tempMacro;
  for(size_t i = 0; i < postfixExpression.size(); i++)
  {
    if(postfixExpression[i].type == TokenType::LITERAL || postfixExpression[i].type == TokenType::SYMBOL)
    {
      auxStack.push(postfixExpression[i]);
    }
    else if(postfixExpression[i].type == TokenType::UNPLUS || postfixExpression[i].type == TokenType::UNMINUS)
    {
      operand1 = auxStack.top();
      auxStack.pop();
      if(operand1.type == TokenType::LITERAL)
      {
        operand1Value = operand1.value;
      }
      else
      {
        tempMacro = MacroTable::findMacro(operand1.symbol);
        operand1Value = tempMacro->getValue();

      }
      switch (postfixExpression[i].type)
      {
        case TokenType::UNPLUS:
          auxStack.push((Token){TokenType::LITERAL, operand1Value, ""});
          break;
        case TokenType::UNMINUS:
          auxStack.push((Token){TokenType::LITERAL, -operand1Value, ""});
          break;

      }
    }
    else
    {
      operand2 = auxStack.top();
      auxStack.pop();
      operand1 = auxStack.top();
      auxStack.pop();

      if(operand1.type == TokenType::LITERAL)
      {
        operand1Value = operand1.value;
      }
      else
      {
        tempMacro = MacroTable::findMacro(operand1.symbol);
        operand1Value = tempMacro->getValue();

      }

      if(operand2.type == TokenType::LITERAL)
      {
        operand2Value = operand2.value;
      }
      else
      {
        tempMacro = MacroTable::findMacro(operand2.symbol);
        operand2Value = tempMacro->getValue();

      }
      switch (postfixExpression[i].type)
      {
        case TokenType::BINPLUS:
          auxStack.push((Token){TokenType::LITERAL, operand1Value + operand2Value, ""});
          break;
        
        case TokenType::BINMINUS:
          auxStack.push((Token){TokenType::LITERAL, operand1Value - operand2Value, ""});
          break; 
      }
    }
  }
  operand1Value = auxStack.top().value;
  auxStack.pop();
  postfixExpression = {};
  if(auxStack.empty())
  {
    return operand1Value;
    
  }
  else
  {
    std::cout << "Wrong Expression" << std::endl;
  }
  return 0;
}

int ExpressionResolver::evaluteExpression(const std::vector<Token> &expr)
{
  infixToPostfix(expr);
  return evaluate();
}