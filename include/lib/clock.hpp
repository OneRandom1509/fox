#pragma once
#include "FoxCallable.hpp"

class Clock : public FoxCallable
{
public:
  auto arity() -> size_t override;

  auto call(Interpreter interpreter, std::vector<literal_t> arguments)
    -> literal_t override;

  auto toString() -> std::string override;
};
