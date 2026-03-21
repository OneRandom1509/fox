#pragma once

#include "Expr.hpp"
#include "Token.hpp"
#include <exception>
#include <memory>
#include <vector>

/// This is the implementation of a recursive descent parser where we move from
/// the resursively descent from the lowest precedence to the highest
/// precedence Exprs. That is, each production will only call either itself, or
/// the production which has higher precedence than it.
///
/// The precedence order is:
/// comma < expression < logical < equality < comparison < term < factor <
/// unary < primary
class Parser
{
public:
  Parser(std::vector<Token> tokens);

  auto parse() -> std::unique_ptr<Expr>;

private:
  std::vector<Token> tokens;
  size_t current = 0;
  class ParseError : public std::exception
  {};

  // comma          → expression ("," comma)* ;
  auto comma() -> std::unique_ptr<Expr>;

  // ternary        → expression "?" ternary ":" ternary | expression ;
  auto ternary() -> std::unique_ptr<Expr>;

  // expression     → logical ;
  auto expression() -> std::unique_ptr<Expr>;

  // logical        → equality ( ( "and" | "or" ) equality )* ;
  auto logical() -> std::unique_ptr<Expr>;

  // equality       → comparison ( ( "!=" | "==" ) comparison )* ;
  auto equality() -> std::unique_ptr<Expr>;

  // comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
  auto comparison() -> std::unique_ptr<Expr>;

  // term           → factor ( ( "-" | "+" ) factor )* ;
  auto term() -> std::unique_ptr<Expr>;

  // factor         → unary ( ( "/" | "*" ) unary )* ;
  auto factor() -> std::unique_ptr<Expr>;

  // unary          → ( "!" | "-" ) unary
  //                | primary ;
  auto unary() -> std::unique_ptr<Expr>;

  // primary        → NUMBER | STRING | "true" | "false" | "nil"
  //                | "(" expression ")" ;
  auto primary() -> std::unique_ptr<Expr>;

  void synchronize();

  // need to see what initializer_list exactly does
  auto match(std::initializer_list<TokenType> types) -> bool;

  auto check(TokenType type) -> bool;

  inline auto advance() -> Token;

  inline auto isAtEnd() -> bool;

  [[nodiscard]] auto peek() const -> const Token &;
  [[nodiscard]] auto previous() const -> const Token &;

  auto consume(TokenType type, std::string message) -> Token;

  auto error(Token token, std::string message) -> ParseError;
};
