#include "terminal.hpp"
#include "memory.hpp"
#include "register_file.hpp"
#include <poll.h>

termios Terminal::oldSettings; 
termios Terminal::newSettings;
uint8_t Terminal::charByte;


void Terminal::initializeTerminal()
{
  tcgetattr(STDIN_FILENO, &oldSettings);
  newSettings = oldSettings;
  newSettings.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

void Terminal::restoreTerminalSettings()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
}

void Terminal::pollKeyboard()
{
  pollfd pfd;
  pfd.fd = STDIN_FILENO;
  pfd.events = POLLIN;

  if(poll(&pfd, 1, 0) > 0)
  {
    read(STDIN_FILENO, &charByte, 1);
    Memory::memWrite4Bytes(0xFFFFFF04, static_cast<uint32_t>(charByte));
    RegisterFile::writeToSPRegister(2, 3);
  }
  
}

void Terminal::displayCharacter()
{
  uint32_t termOut = Memory::memRead4bytes(0xFFFFFF00);
  uint8_t outputChar = static_cast<uint8_t>(termOut);
  write(STDOUT_FILENO, &outputChar, 1);
}
