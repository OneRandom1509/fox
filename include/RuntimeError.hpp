#pragma once

#include "Token.hpp"
#include <stdexcept>
#include <string>

class RuntimeError : public std::runtime_error
{
public:
  const Token token;
  std::string message;

  RuntimeError(Token token, std::string message);
};
