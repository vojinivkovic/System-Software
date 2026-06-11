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
    
    ~Macro() 
    { 
      for(auto iReference : tableForwardReference)
      {
        delete iReference;
      }
    }  

    size_t getName() const { return name; }
    bool getDefined() const { return defined; }
    std::int32_t getValue() const {return value; }
    void addForwardReference(ForwardReference* newReference) { tableForwardReference.push_back(newReference); }
    std::vector<ForwardReference*> getForwardReference() const { return tableForwardReference; }
    bool checkForResolving(std::vector<std::string> definedSymbols);
    void resolveMacro();

    private:
    bool defined;
    size_t name;
    std::int32_t value;
    std::vector<std::string> dependencySymbol;
    std::vector<Token> expression;
    std::vector<ForwardReference*> tableForwardReference;
    
};

#endif