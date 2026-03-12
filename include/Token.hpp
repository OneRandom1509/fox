#pragma once

#include <string>
#include <variant>

using Literal = std::variant<std::monostate, double, std::string>;

enum class TokenType
{
  // Single Character Tokens
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // Double Character Tokens
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESSER,
  LESSER_EQUAL,

  // Multi Character Tokens
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keyword Tokens
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  EOF_TOK,
};

class Token
{
public:
  const TokenType type;
  const std::string lexeme;
  const Literal literal;
  const int line;

  Token(TokenType type, std::string lexeme, Literal literal, int line);

  std::string toString();

private:
  std::string tokenTypeString();

  std::string literalToString() const;
};
