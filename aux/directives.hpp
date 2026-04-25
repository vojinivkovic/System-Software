#ifndef _DIRECTIVES_HPP_
#define _DIRECTIVES_HPP_

#include <unordered_map>
#include <string>
#include <vector>

class Directives
{
public:
  Directives() = delete;
  Directives(const Directives& dir) = delete;
  Directives& operator=(const Directives& dir) = delete;

  static void execute(const std::string& directive_name, const std::string& arguments);
private:
  static std::unordered_map<std::string, void (*)(const std::string&)> tableOfDirectives;
  
};

#endif