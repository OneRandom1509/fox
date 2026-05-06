#include "FoxFunction.hpp"
#include <utility>

FoxFunction::FoxFunction(std::shared_ptr<Function> declaration)
    : declaration(std::move(declaration))
{}

auto FoxFunction::arity() -> size_t { return declaration->params.size(); }

auto FoxFunction::call(Interpreter interpreter,
                       std::vector<literal_t> arguments) -> literal_t
{
  Environment environment = Environment(interpreter.globals);
  for(size_t i = 0; i < declaration->params.size(); i++)
    {
      environment.define(declaration->params.at(i).lexeme,
                         {true, arguments.at(i)});
    }
  interpreter.executeBlock(declaration->body,
                           std::make_shared<Environment>(environment));

  // returning nil in case there is no explicit return statement
  return std::monostate{};
}

auto FoxFunction::toString() -> std::string
{
  return "<fn " + declaration->name.lexeme + ">";
}
