#pragma once

#include "Interpreter.hpp"
#include "Token.hpp"
#include <vector>

class FoxCallable
{
public:
  /// arity -> number of arguments
  virtual auto arity() -> size_t = 0;
  virtual auto
  call(Interpreter interpreter, std::vector<literal_t> arguments) -> literal_t
    = 0;
  virtual auto toString() -> std::string = 0;
  virtual ~FoxCallable() = default;
};
