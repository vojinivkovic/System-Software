#ifndef _DIRECTIVE_FUNCTIONS_HPP_
#define _DIRECTIVE_FUNCTIONS_HPP_

#include <string>

void directive_global(const std::string& arguments);

void directive_section(const std::string& arguments);

void directive_extern(const std::string& arguments);

void directive_skip(const std::string& arguments);

void directive_ascii(const std::string& arguments);


#endif