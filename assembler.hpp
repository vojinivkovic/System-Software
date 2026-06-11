#ifndef _ASSEMBLER_HPP_
#define _ASSEMBLER_HPP_


#include <vector>
#include "section.hpp"

class Assembler
{
public:
  Assembler() = delete;
  Assembler(const Assembler& assembler) = delete;
  Assembler& operator=(const Assembler& assembler) = delete;
  static void initializeAssembler();
  static Section* getCurrentSection() { return currentSection; };
  static void addSection(Section* newSection) { arrayOfSections.push_back(newSection); }
  static void setCurrentSection(Section* newSection) { currentSection = newSection; }
  static size_t getNumberOfSections() { return arrayOfSections.size(); }
  static Section* findSection(size_t nameOfSection);
  static void afterFirstPass();
  static void cleanup();
  static std::vector<Section*> getSections() { return arrayOfSections; }
  static void makeELFFiles(const std::string& name);
  static void deleteTableOfSections();

private:
  static Section* currentSection;
  static std::vector<Section*> arrayOfSections;
  static void checkIfSymbolsDefined();
  static void checkIfMacrosDefined();
  static void fixRelocationEntries();

};


#endif