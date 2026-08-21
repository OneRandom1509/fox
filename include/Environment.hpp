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
  auto getAt(int distance, std::string name) const -> literal_t;
  void assignAt(int distance, Token name, std::pair<bool, literal_t> value);
  void define(std::string name, std::pair<bool, literal_t> value);
  void assign(Token name, std::pair<bool, literal_t> value);
  auto ancestor(int distance) const -> std::shared_ptr<Environment>;

private:
  // For each variable, the boolean will track if it has been initialized yet
  // or not, and literal_t will hold the literal value (nil in case of an
  // uninitialized variable)
  std::unordered_map<std::string, std::pair<bool, literal_t>> values;
  std::shared_ptr<Environment> enclosing;
};
