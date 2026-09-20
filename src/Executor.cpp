#include "Executor.hpp"
#include <ios>
#include "Constants.hpp"
#include <iostream>
#include <sysexits.h>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include "Resolver.hpp"
#include "Scanner.hpp"
#include "Token.hpp"
#include "Parser.hpp"
#include <readline/readline.h>
#include <readline/history.h>

namespace
{
  constexpr const char *ANSI_RED = "\033[31m";
  constexpr const char *ANSI_YELLOW = "\033[33m";
  constexpr const char *ANSI_RESET = "\033[0m";
} // namespace

// Initializing static variables
std::string Executor::source = "";
bool Executor::hadError = false;
bool Executor::hadRuntimeError = false;
bool Executor::repl = false;
bool Executor::printTokens = false;

// Only paint stderr in color when it is an actual terminal
namespace
{
  const bool colorEnabled = isatty(STDERR_FILENO) != 0;

  auto colored(const char *color, const char *text) -> std::string
  {
    if(!colorEnabled)
      return text;
    return std::string(color) + text + ANSI_RESET;
  }
} // namespace

Interpreter Executor::interpreter = Interpreter();

void Executor::runFile(std::string path)
{
  repl = false;
  std::stringstream ss;

  {
    std::fstream input_file(path, std::ios::in);
    ss << input_file.rdbuf();
  }

  source = ss.str();
  run();
  if(hadError)
    exit(EX_DATAERR);
  if(hadRuntimeError)
    exit(EX_SOFTWARE);
}

void Executor::runPrompt()
{
  char *line;
  while((line = readline("fox> ")) != nullptr)
    {
      repl = true;
      if(*line)
        add_history(line);
      std::string buf(line);
      free(line);
      if(buf == "exit();")
        {
          std::cout << FOX_PROMPT_EXIT << std::endl;
          exit(EX_OK);
        }
      else
        {
          source = buf;
          run();
          // removing the hadError flag to not kill the interactive session
          hadError = false;
        }
    }
  std::cout << FOX_PROMPT_EXIT << std::endl;
  exit(EX_OK);
}

void Executor::run()
{
  Scanner scanner(this->source);
  std::vector<Token> tokens = scanner.scanTokens();

  if(printTokens)
    {
      for(auto token : tokens)
        {
          std::cout << token.toString() << std::endl;
        }
      std::cout << std::endl;
    }

  Parser parser(tokens);
  auto statements = parser.parse();

  // Stop if there's a syntax error
  if(hadError)
    return;

  Resolver resolver = Resolver(interpreter);
  resolver.resolve(statements);
  if(hadError)
    return;

  interpreter.interpret(statements);
}

void Executor::error(int line, std::string message)
{ report(line, "", message); }

void Executor::error(Token token, std::string message)
{
  if(token.type == TokenType::EOF_TOK)
    report(token.line, " at end", message);
  else
    report(token.line, " at '" + token.lexeme + "'", message);
}

void Executor::warn(Token token, std::string message)
{
  if(token.type == TokenType::EOF_TOK)
    reportWarning(token.line, " at end", message);
  else
    reportWarning(token.line, " at '" + token.lexeme + "'", message);
}

void Executor::reportWarning(int line, std::string where, std::string message)
{
  std::cerr << "[line " << line << "] " << colored(ANSI_YELLOW, "[WARNING]")
            << where << ": " << message << std::endl;
}

void Executor::report(int line, std::string where, std::string message)
{
  std::cerr << "[line " << line << "] " << colored(ANSI_RED, "[ERROR]")
            << where << ": " << message << std::endl;
  hadError = true;
}

void Executor::runtimeError(RuntimeError &error)
{
  std::cerr << error.what() << "\n[line " << error.token.line << "] "
            << colored(ANSI_RED, "[ERROR]") << std::endl;
  hadRuntimeError = true;
}
