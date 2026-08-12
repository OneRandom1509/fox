#include "Executor.hpp"
#include <ios>
#include "Constants.hpp"
#include <iostream>
#include <sysexits.h>
#include <sstream>
#include <fstream>
#include <string>
#include "Scanner.hpp"
#include "Token.hpp"
#include "Parser.hpp"

// Initializing static variables
std::string Executor::source = "";
bool Executor::hadError = false;
bool Executor::hadRuntimeError = false;
bool Executor::repl = false;

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
  std::string buf;
  while(true)
    {
      repl = true;
      std::cout << "fox> ";
      std::getline(std::cin, buf);
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

  // for(auto token : tokens)
  //   {
  //     std::cout << token.toString() << std::endl;
  //   }
  // std::cout << std::endl;
  //
  Parser parser(tokens);
  auto statements = parser.parse();

  // Stop if there's a syntax error
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

void Executor::report(int line, std::string where, std::string message)
{
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  hadError = true;
}

void Executor::runtimeError(RuntimeError &error)
{
  std::cerr << error.what() << "\n[line" << error.token.line << "]"
            << std::endl;
  hadRuntimeError = true;
}
