#pragma once

#include "FoxCallable.hpp"
#include "Interpreter.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <memory>
#include <vector>

class FoxFunction : public FoxCallable
{
public:
  FoxFunction(std::shared_ptr<Function> declaration);
  auto arity() -> size_t override;
  auto call(Interpreter interpreter, std::vector<literal_t> arguments)
    -> literal_t override;
  auto toString() -> std::string override;

private:
  const std::shared_ptr<Function> declaration;
};
