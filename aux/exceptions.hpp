#ifndef _EXCEPTIONS_HPP_
#define _EXCEPTIONS_HPP_

#include <stdexcept>

enum class ErrorType 
{
  ErrorTooManyArguments,
  ErrorInvalidArgumentType,
  ErrorTooFewArguments,
  ErrorInvalidArgument,
  ErrorInvalidAddressing,
  ErrorSymbolAlreadyDefined,
  ErrorInvalidSymbolInMacroExpression,
  ErrorUndefinedSymbol,
  ErrorUndefinedMacro
};

class AssemblerErrors : public std::runtime_error
{
public:
  AssemblerErrors(ErrorType code, const std::string& msg, const int& section, const int& line, const std::string& detail = "") : 
    std::runtime_error(msg), errorCode(code), sectionName(section), lineInSection(line), detailMessage(detail) {} 
  
    std::string toString(ErrorType code) {
      switch (code) {
          case ErrorType::ErrorTooManyArguments: return "TooManyArguments";
          case ErrorType::ErrorUndefinedMacro: return "UndefinedMacro";
          case ErrorType::ErrorUndefinedSymbol: return "UndefinedSymbol";
          case ErrorType::ErrorInvalidArgumentType: return "InvalidArgumentType";
          case ErrorType::ErrorTooFewArguments: return "TooFewArguments";
          case ErrorType::ErrorInvalidArgument: return "InvalidArgument";
          case ErrorType::ErrorInvalidAddressing: return "InvalidAddressing";
          case ErrorType::ErrorSymbolAlreadyDefined: return "SymbolAlreadyDefined";
          case ErrorType::ErrorInvalidSymbolInMacroExpression: return "SymbolInMacroExpression";
          default: return "UnknownError";
      }
      
    }
   
    int getSectionName() const { return sectionName; }
    int getLineInSection() const { return lineInSection; }
    ErrorType getErrorType() const { return errorCode; }
    std::string getDetailMessage() const { return detailMessage; }

private:
  int sectionName, lineInSection;
  ErrorType errorCode;
  std::string detailMessage;
};


#endif