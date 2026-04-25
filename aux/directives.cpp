#include "directives.hpp"
#include "directive_functions.hpp"

std::unordered_map<std::string, void (*)(const std::string&)> Directives::tableOfDirectives = {
  {"global", directiveGlobal},
  {"section", directiveSection},
  {"extern", directiveExtern},
  {"skip", directiveSkip},
  {"ascii", directiveAscii}
};

void Directives::execute(const std::string& directive_name, const std::string& arguments)
{
  tableOfDirectives[directive_name](arguments);
}

