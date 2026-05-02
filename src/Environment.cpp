#include "Environment.hpp"
#include "RuntimeError.hpp"
#include <utility>

Environment::Environment() { enclosing = nullptr; };

Environment::Environment(std::shared_ptr<Environment> enclosing)
    : enclosing(std::move(enclosing))
{}

auto Environment::get(Token name) const -> literal_t
{
  auto it = values.find(name.lexeme);
  if(it != values.end())
    {
      // Checking if the variable is uninitialized
      if(!it->second.first)
        throw RuntimeError(name,
                           "Unintialized variable '" + name.lexeme + "'.");
      return it->second.second;
    }
  if(enclosing != nullptr)
    return enclosing->get(name);

  throw RuntimeError{name, "Undefined variable '" + name.lexeme + "'."};
}

/// define -> name: name of the literal, value -> pair of bool
/// (initialized/uninitialized) and literal_t
void Environment::define(std::string name, std::pair<bool, literal_t> value)
{
  values.insert_or_assign(name, value);
}

void Environment::assign(Token name, std::pair<bool, literal_t> value)
{
  if(values.contains(name.lexeme))
    {
      values.at(name.lexeme) = value;
      return;
    }

  if(enclosing != nullptr)
    {
      enclosing->assign(name, value);
      return;
    }
  throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}
