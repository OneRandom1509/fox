#pragma once

#include "Token.hpp"
#include <memory>
#include <unordered_map>
class Environment
{
public:
  Environment();
  Environment(std::shared_ptr<Environment> enclosing);
  [[nodiscard]] auto get(Token name) const -> literal_t;
  void define(std::string name, std::pair<bool, literal_t> value);
  void assign(Token name, std::pair<bool, literal_t> value);

private:
  std::unordered_map<std::string, std::pair<bool, literal_t>> values;
  std::shared_ptr<Environment> enclosing;
};
