#ifndef _EXPRESSION_TOKENS_HPP_
#define _EXPRESSION_TOKENS_HPP_
#include <cstdint>
#include <string>

enum class TokenType
{
  LITERAL,
  SYMBOL,
  BINPLUS,
  BINMINUS, 
  UNPLUS,
  UNMINUS,
  LPARENTHESES,
  RPARENTHESES
};

struct Token
{
  TokenType type;
  uint32_t value;
  std::string symbol;
};

#endif