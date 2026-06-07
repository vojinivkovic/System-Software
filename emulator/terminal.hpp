#ifndef _TERMINAL_HPP_
#define _TERMINAL_HPP_
#include <termios.h>
#include <iostream>
#include <unistd.h>

class Terminal
{
public:
  Terminal() = delete;
  Terminal(const Terminal& term) = delete;
  Terminal& operator=(const Terminal& term) = delete;

  static void initializeTerminal();
  static void restoreTerminalSettings();
  static void pollKeyboard();
  static void displayCharacter();
private:
  static termios oldSettings, newSettings;
  static uint8_t charByte;
};

#endif