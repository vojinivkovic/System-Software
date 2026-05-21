#ifndef _EXPRESSION_RESOLVER_HPP_
#define _EXPRESSION_RESOLVER_HPP_

#include <vector>
#include <stack>
#include <cstdint>
#include "expression_tokens.hpp"
#include <unordered_map>

enum class Associativity
{
  LEFT,
  RIGHT 
};

struct PriorityValue
{
  uint8_t initialPriority, postPriority;
  int rang;
  Associativity associativityType;
};

class ExpressionResolver
{
public:
  ExpressionResolver() = delete;
  ExpressionResolver(const ExpressionResolver& expr) = delete;
  ExpressionResolver& operator=(const ExpressionResolver& expr) = delete;

  static std::int32_t evaluteExpression(const std::vector<Token>& expr);

private:
  static std::vector<Token> postfixExpression;
  static void infixToPostfix(const std::vector<Token>& expr);
  static std::int32_t evaluate();
  static std::stack<Token> auxStack;
  static std::unordered_map<TokenType, PriorityValue> priorityTable;
};

#endif