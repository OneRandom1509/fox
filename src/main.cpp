#include <cstdlib>
#include <sysexits.h>
#include <iostream>
#include <string>
#include "Executor.hpp"

void printUsage()
{
  std::cout << "Usage: fox [options] [script]\n"
            << "\n"
            << "Fox is a tree-walking interpreter for the Fox language.\n"
            << "With no arguments it starts an interactive prompt (REPL).\n"
            << "\n"
            << "Options:\n"
            << "  -h, --help     Show this help message and exit\n"
            << "  -t, --tokens   Print all tokens scanned from the source\n"
            << std::endl;
}

auto main(int argc, char *argv[]) -> int
{
  Executor executor;
  std::string scriptPath;

  for(int i = 1; i < argc; ++i)
    {
      std::string arg(argv[i]);
      if(arg == "-h" || arg == "--help")
        {
          printUsage();
          exit(EX_OK);
        }
      else if(arg == "-t" || arg == "--tokens")
        {
          Executor::printTokens = true;
        }
      else if(!scriptPath.empty())
        {
          std::cerr << "Usage: fox [options] [script]" << std::endl;
          exit(EX_USAGE);
        }
      else
        {
          scriptPath = arg;
        }
    }

  if(!scriptPath.empty())
    {
      executor.runFile(scriptPath);
    }
  else
    {
      executor.runPrompt();
    }

  // To play around with AstPrinter tool
  //
  // std::unique_ptr<Expr> expression = std::make_unique<Binary>(
  //   std::make_unique<Unary>(Token(TokenType::MINUS, "-", std::monostate{},
  //   1),
  //                           std::make_unique<Literal>(123.0)),
  //   Token(TokenType::STAR, "*", std::monostate{}, 1),
  //   std::make_unique<Grouping>(std::make_unique<Binary>(
  //     std::make_unique<Unary>(Token(TokenType::MINUS, "-", std::monostate{},
  //     1),
  //                             std::make_unique<Literal>(123.0)),
  //     Token(TokenType::STAR, "*", std::monostate{}, 1),
  //     std::make_unique<Literal>(400.0))));
  //
  // AstPrinter printer;
  // std::cout << printer.print(*expression) << std::endl;

  return EX_OK;
}
