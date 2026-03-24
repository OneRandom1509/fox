#pragma once

#include "Token.hpp"
#include <unordered_map>
#include <vector>

class Scanner
{
public:
  Scanner(std::string source);
  auto scanTokens() -> std::vector<Token>;

private:
  const std::string source;
  std::vector<Token> tokens;
  size_t start = 0;
  size_t current = 0;
  int line = 1;
  static const std::unordered_map<std::string, TokenType> keywords;

  inline auto isAtEnd() -> bool;
  inline auto advance() -> char;

  void scanToken();

  inline void addToken(TokenType type);
  inline void addToken(TokenType type, literal_t literal);

  // simple one character look-ahead
  inline auto match(char expected) -> bool;
  inline auto peek() -> char;
  inline auto peekNext() -> char;

  void string();
  void number();
  void identifier();

  auto isalpha(char c) -> bool;
  auto isalnum(char c) -> bool;
};
