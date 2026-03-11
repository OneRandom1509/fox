#pragma once

#include <string>
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
  START,

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
  FLASE,
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
  // const std::string literal;
  const int line;

  Token(TokenType type, std::string lexeme,
        /*std::string literal, */ int line);

  std::string toString();

private:
  std::string tokenTypeString();
};
