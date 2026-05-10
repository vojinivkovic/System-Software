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
  ErrorSymbolAlreadyDefined 
};

class AssemblerErrors : public std::runtime_error
{
public:
  AssemblerErrors(ErrorType code, const std::string& msg, const int& section, const int& line, const std::string& detail = "") : 
    std::runtime_error(msg), errorCode(code), sectionName(section), lineInSection(line), detailMessage(detail) {} 
private:
  int sectionName, lineInSection;
  ErrorType errorCode;
  std::string detailMessage;
};


#endif