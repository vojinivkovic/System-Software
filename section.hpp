#ifndef _SECTION_HPP_
#define _SECTION_HPP_


#include <cstdlib>
#include <cstdint>


class Section
{
public:
  Section(const char* sectionName);
  size_t getLocationCounter() const { return locationCounter;}
  int translateInstruction(const char* line);
  int executeDirective(const char* line);
  int copyContent();

private:
  size_t name, locationCounter;
  uint8_t * content;

};


#endif