#pragma once

#include "Interpreter.hpp"
#include "Token.hpp"
#include "RuntimeError.hpp"
#include <string>

class Executor
{
public:
  static std::string source;
  static bool repl;
  static bool printTokens;

  void runFile(std::string path);
  void runPrompt();

  static void error(int line, std::string message);
  static void error(Token token, std::string message);
  static void warn(Token token, std::string message);
  static void runtimeError(RuntimeError &error);

private:
  static Interpreter interpreter;
  static bool hadError;
  static bool hadRuntimeError;

  void run();
  static inline void report(int line, std::string where, std::string message);
  static inline void
  reportWarning(int line, std::string where, std::string message);
};
