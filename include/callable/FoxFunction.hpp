#pragma once

#include "Environment.hpp"
#include "FoxCallable.hpp"
#include "Interpreter.hpp"
#include "Token.hpp"
#include <memory>
#include <vector>

class FoxFunction : public FoxCallable
{
public:
  FoxFunction(std::string name, std::shared_ptr<FunctionBody> declaration,
              Environment closure);
  auto arity() -> size_t override;
  auto call(Interpreter interpreter, std::vector<literal_t> arguments)
    -> literal_t override;
  auto toString() -> std::string override;

private:
  const std::string name;
  const std::shared_ptr<FunctionBody> declaration;

  // inherits the parent environment (either main interpreter or parent
  // function's environment)
  const Environment closure;
};
