#include <cstdlib>
#include <sysexits.h>
#include <iostream>
#include "Token.hpp"
#include "Expr.hpp"
#include "AstPrinter.cpp"

#include <string>
#include "Executor.hpp"

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
  //
  return EX_OK;
}
