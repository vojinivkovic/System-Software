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
  ErrorUndefinedMacro,
  ErrorMultipleDefinitions, 
  ErrorUnresolvedSymbol,
  ErrorOverlappingSections,
  ErrorUninitializedMemory,
  ErrorUndefinedTimerConfig
};

class AssemblerErrors : public std::runtime_error
{
public:
  AssemblerErrors(ErrorType code, const std::string& msg, const int& section, const int& line, const std::string& detail = "") : 
    std::runtime_error(msg), errorCode(code), sectionName(section), lineInSection(line), detailMessage(detail) {} 
  
    std::string toString(ErrorType code) 
    {
      switch (code) 
      {
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

class LinkerErrors : public std::runtime_error
{
public:
  LinkerErrors(ErrorType code, const std::string& msg, const std::string& detail = "") : 
    std::runtime_error(msg), errorCode(code), detailMessage(detail) {} 
  
    std::string toString(ErrorType code) 
    {
      switch (code) 
      {
          case ErrorType::ErrorMultipleDefinitions: return "Multiple global definitions of symbol";
          case ErrorType::ErrorUnresolvedSymbol: return "Unresolved symbol";
          case ErrorType::ErrorOverlappingSections: return "Sections overlap, place of sections need to changed";
          default: return "UnknownError";
      }
      
    }
    ErrorType getErrorType() const { return errorCode; }
    std::string getDetailMessage() const { return detailMessage; }


  private:
    std::string detailMessage;
    ErrorType errorCode;

};

class CPUErrors : public std::runtime_error 
{
public:
   CPUErrors(ErrorType code, const std::string& msg, const std::string& detail = "") : 
    std::runtime_error(msg), errorCode(code), detailMessage(detail) {} 
  

    std::string toString(ErrorType code) 
    {
      switch (code) 
      {
          case ErrorType::ErrorUninitializedMemory: return "Memory is not initialized.";
          case ErrorType::ErrorUndefinedTimerConfig: return "Undefined timer config.";
          default: return "UnknownError";
      }
      
    }
    ErrorType getErrorType() const { return errorCode; }
    std::string getDetailMessage() const { return detailMessage; }


private:
    std::string detailMessage;
    ErrorType errorCode;
};
#endif