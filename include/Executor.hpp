#pragma once
#include <string>

class Executor
{
public:
  static std::string source;

  void runFile(std::string path);

  void runPrompt();

  static void error(int line, std::string message);

private:
  static bool hadError;

  void run();
  static inline void report(int line, std::string where, std::string message);
};
