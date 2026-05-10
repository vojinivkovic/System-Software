#ifndef _MACRO_HPP_
#define _MACRO_HPP_

#include <vector>
#include <cstdint>
#include <string>
#include "../aux/expression_tokens.hpp"
#include "../aux/forward_reference.hpp"
#include "../symbol/symbol.hpp"


class Macro
{
  public:
    Macro(const size_t& name_, const std::vector<Token>& expression_, 
      const std::vector<std::string> dependencySymbol_, const std::vector<ForwardReference*>& fReference);
    
    size_t getName() const { return name; }
    bool getDefined() const { return defined; }
    int getValue() const {return value; }
    void addForwardReference(ForwardReference* newReference) { tableForwardReference.push_back(newReference); }
    bool checkForResolving(std::vector<std::string> definedSymbols);
    void resolveMacro();
    
    private:
    bool defined;
    size_t name;
    int value;
    std::vector<std::string> dependencySymbol;
    std::vector<Token> expression;
    std::vector<ForwardReference*> tableForwardReference;
    
};

#endif