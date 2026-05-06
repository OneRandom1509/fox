#include "FoxFunction.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include <utility>

FoxFunction::FoxFunction(std::string name,
                         std::shared_ptr<FunctionBody> declaration,
                         Environment environment)
    : name(std::move(name)), declaration(std::move(declaration)),
      closure(std::move(environment))
{}

auto FoxFunction::arity() -> size_t { return declaration->params.size(); }

auto FoxFunction::call(Interpreter interpreter,
                       std::vector<literal_t> arguments) -> literal_t
{
  Environment environment = Environment(closure);
  for(size_t i = 0; i < declaration->params.size(); i++)
    {
      environment.define(declaration->params.at(i).lexeme,
                         {true, arguments.at(i)});
    }
  try
    {
      interpreter.executeBlock(declaration->body,
                               std::make_shared<Environment>(environment));
    }
  catch(ReturnFlow &ret)
    {
      return ret.value;
    }

  // returning nil in case there is no explicit return statement
  return std::monostate{};
}

auto FoxFunction::toString() -> std::string
{
  if(name == "")
    return "<fn>";
  return "<fn " + name + ">";
}
