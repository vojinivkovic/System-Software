#include "directives.hpp"

std::unordered_map<std::string, void (*)(const std::string&)> Directives::tableOfDirectives = {
  {"global", directive_global},
  {"section", directive_section},
  {"extern", directive_extern},
  {"skip", directive_skip},
  {"ascii", directive_ascii}
};

void Directives::execute(const std::string& directive_name, const std::string& arguments)
{
  tableOfDirectives[directive_name](arguments);
}

