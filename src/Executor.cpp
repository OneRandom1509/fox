#include "Executor.hpp"
#include <ios>
#include "Constants.hpp"
#include <iostream>
#include <sysexits.h>
#include <sstream>
#include <fstream>
#include <string>

void Executor::runFile(std::string path)
{
  std::stringstream ss;

  {
    std::fstream input_file(path, std::ios::in);
    ss << input_file.rdbuf();
  }

  this->source = ss.str();
  run();
  if(this->hadError)
    exit(EX_DATAERR);
}

void Executor::runPrompt()
{
  std::string buf;
  while(true)
    {
      std::cout << "fox> ";
      std::getline(std::cin, buf);
      if(buf == "exit()")
        {
          std::cout << FOX_PROMPT_EXIT << std::endl;
          exit(EX_OK);
        }
      else
        {
          this->source = std::move(buf);
          run();
          // removing the hadError flag to not kill the interactive session
          this->hadError = false;
        }
    }
  std::cout << FOX_PROMPT_EXIT << std::endl;
  exit(EX_OK);
}

void Executor::run()
{
  // Tokenizer tokenizer(source);
  // std::vector<Tokens> tokens = tokenizer.scanTokens();
  //
  // for(auto token: tokens) {
  //    std::cout << token << ", ";
  // }
}

void Executor::error(int line, std::string message)
{
  report(line, "", message);
}

void Executor::report(int line, std::string where, std::string message)
{
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  this->hadError = true;
}
