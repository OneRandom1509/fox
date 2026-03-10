#pragma once
#include <string>

class Executor
{
public:
  void runFile(std::string path);

  void runPrompt();

private:
  std::string source;
  bool hadError;

  void run();
  inline void error(int line, std::string message);
  inline void report(int line, std::string where, std::string message);
};
