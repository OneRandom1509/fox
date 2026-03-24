#pragma once

#include "Expr.hpp"
#include "Stmt.hpp"
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
/// statement < expression < assignment <  comma < ternary < logical < equality
/// < comparison < term < factor < unary < primary
class Parser
{
public:
  Parser(std::vector<Token> tokens);

  auto parse() -> std::vector<std::shared_ptr<Stmt>>;

private:
  std::vector<Token> tokens;
  size_t current = 0;
  class ParseError : public std::exception
  {};

  /// declaration    → varDecl | statement ;
  auto declaration() -> std::shared_ptr<Stmt>;

  /// varDecl        → "let" IDENTIFIER ( "=" expression )? ";" ;
  auto varDeclaration() -> std::shared_ptr<Stmt>;

  /// statement      → exprStmt | printStmt ;
  auto statement() -> std::shared_ptr<Stmt>;

  /// block          → "{" declaration* "}" ;
  auto block() -> std::vector<std::shared_ptr<Stmt>>;

  /// printStatement → "print" expression ; ;
  auto printStatement() -> std::shared_ptr<Stmt>;

  /// exprStatement  → expression ; ;
  auto expressionStatement() -> std::shared_ptr<Stmt>;

  /// expression     → assignment ;
  auto expression() -> std::shared_ptr<Expr>;

  /// assignment     → IDENTIFIER "=" assignment | comma ;
  auto assignment() -> std::shared_ptr<Expr>;

  /// comma          → ternary ("," comma)* ;
  auto comma() -> std::shared_ptr<Expr>;

  /// ternary        → logical "?" ternary ":" ternary | logical ;
  auto ternary() -> std::shared_ptr<Expr>;

  /// logical        → equality ( ( "and" | "or" ) equality )* ;
  auto logical() -> std::shared_ptr<Expr>;

  /// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
  auto equality() -> std::shared_ptr<Expr>;

  /// comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
  auto comparison() -> std::shared_ptr<Expr>;

  /// term           → factor ( ( "-" | "+" ) factor )* ;
  auto term() -> std::shared_ptr<Expr>;

  /// factor         → unary ( ( "/" | "*" ) unary )* ;
  auto factor() -> std::shared_ptr<Expr>;

  /// unary          → ( "!" | "-" ) unary
  //                | primary ;
  auto unary() -> std::shared_ptr<Expr>;

  /// primary        → NUMBER | STRING | "true" | "false" | "nil"
  ///                | "(" expression ")" | IDENTIFIER ;
  auto primary() -> std::shared_ptr<Expr>;

  void synchronize();

  auto match(std::initializer_list<TokenType> types) -> bool;

  auto check(TokenType type) -> bool;

  inline auto advance() -> Token;

  inline auto isAtEnd() -> bool;

  [[nodiscard]] auto peek() const -> const Token &;
  [[nodiscard]] auto previous() const -> const Token &;

  auto consume(TokenType type, std::string message) -> Token;

  auto error(Token token, std::string message) -> ParseError;
};
