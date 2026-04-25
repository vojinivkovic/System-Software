#ifndef _DIRECTIVE_FUNCTIONS_HPP_
#define _DIRECTIVE_FUNCTIONS_HPP_

#include <string>

void directiveGlobal(const std::string& arguments);

void directiveSection(const std::string& arguments);

void directiveExtern(const std::string& arguments);

void directiveSkip(const std::string& arguments);

void directiveAscii(const std::string& arguments);


#endif