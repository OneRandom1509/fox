#include "Executor.hpp"
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
/// expression < equality < comparison < term < factor < unary < primary
class Parser
{
public:
  Parser(Executor &executor, std::vector<Token> tokens);

  std::unique_ptr<Expr> parse();

private:
  std::vector<Token> tokens;
  size_t current = 0;
  Executor &executor;
  class ParseError : public std::exception
  {};

  // expression     → equality ;
  std::unique_ptr<Expr> expression();

  // equality       → comparison ( ( "!=" | "==" ) comparison )* ;
  std::unique_ptr<Expr> equality();

  // comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
  std::unique_ptr<Expr> comparison();

  // term           → factor ( ( "-" | "+" ) factor )* ;
  std::unique_ptr<Expr> term();

  // factor         → unary ( ( "/" | "*" ) unary )* ;
  std::unique_ptr<Expr> factor();

  // unary          → ( "!" | "-" ) unary
  //                | primary ;
  std::unique_ptr<Expr> unary();

  // primary        → NUMBER | STRING | "true" | "false" | "nil"
  //                | "(" expression ")" ;
  std::unique_ptr<Expr> primary();

  void synchronize();

  // need to see what initializer_list exactly does
  bool match(std::initializer_list<TokenType> types);

  bool check(TokenType type);

  inline Token advance();

  inline bool isAtEnd();

  [[nodiscard]] const Token &peek() const;
  [[nodiscard]] const Token &previous() const;

  Token consume(TokenType type, std::string message);

  ParseError error(Token token, std::string message);
};
