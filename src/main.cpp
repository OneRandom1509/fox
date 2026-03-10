#include <cstdlib>
#include <sysexits.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include "Executor.hpp"
#include "Constants.hpp"

int main(int argc, char *argv[])
{
  Executor executor;
  if(argc > 2)
    {
      std::cerr << "Usage: fox [script]" << std::endl;
      exit(EX_USAGE);
    }
  else if(argc == 2)
    {
      executor.runFile(std::string(argv[1]));
    }
  else
    {
      executor.runPrompt();
    }
  return EX_OK;
}
