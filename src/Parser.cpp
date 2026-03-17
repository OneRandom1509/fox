#include "Executor.hpp"
#include "Expr.hpp"
#include "Token.hpp"
#include <memory>
#include <utility>
#include <vector>
#include "Parser.hpp"

/// This is the implementation of a recursive descent parser where we move from
/// the resursively descent from the lowest precedence to the highest
/// precedence Exprs. That is, each production will only call either itself, or
/// the production which has higher precedence than it.
///
/// The precedence order is:
/// comma < expression < equality < comparison < term < factor < unary < primary
Parser::Parser(Executor &executor, std::vector<Token> tokens)
    : tokens(std::move(tokens)), executor(executor)
{}

auto Parser::parse() -> std::unique_ptr<Expr>
{
  try
    {
      return comma();
    }
  catch(ParseError &error)
    {
      return nullptr;
    }
}

// comma          → ternary ("," comma)*
auto Parser::comma() -> std::unique_ptr<Expr>
{
  if(match({TokenType::COMMA}))
    {
      throw error(previous(), "Expect expression before COMMA.");
    }

  auto expr = ternary();
  while(match({TokenType::COMMA}))
    {
      auto right = comma();
      expr = std::make_unique<Comma>(std::move(expr), std::move(right));
    }

  return expr;
}

// ternary        → expression "?" ternary ":" ternary | expression ;
auto Parser::ternary() -> std::unique_ptr<Expr>
{
  if(match({TokenType::QUESTION}))
    {
      throw error(previous(), "Expect condition before QUESTION.");
    }
  auto expr = expression();

  if(match({TokenType::COLON}))
    {
      throw error(previous(), "Expect QUESTION before COLON.");
    }

  if(match({TokenType::QUESTION}))
    {
      auto left = ternary();
      if(match({TokenType::COLON}))
        {
          auto right = ternary();
          expr = std::make_unique<Ternary>(std::move(expr), std::move(left),
                                           std::move(right));
        }
      else
        {
          throw error(previous(), "Incomplete Ternary Expression.");
        }
    }

  return expr;
}

// expression     → equality ;
auto Parser::expression() -> std::unique_ptr<Expr> { return equality(); }

// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
auto Parser::equality() -> std::unique_ptr<Expr>
{
  if(match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
      throw error(previous(), "Expect expression before "
                                + tokenTypeString(previous().type) + ".");
    }

  auto expr = comparison();

  while(match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
      Token op = previous();
      auto right = comparison();
      expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

  return expr;
}

// comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
auto Parser::comparison() -> std::unique_ptr<Expr>
{
  if(match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESSER,
            TokenType::LESSER_EQUAL}))
    {
      throw error(previous(), "Expect expression before "
                                + tokenTypeString(previous().type) + ".");
    }
  auto expr = term();

  while(match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESSER,
               TokenType::LESSER_EQUAL}))
    {
      Token op = previous();
      auto right = term();
      expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
  return expr;
}

// term           → factor ( ( "-" | "+" ) factor )* ;
auto Parser::term() -> std::unique_ptr<Expr>
{
  if(match({TokenType::PLUS}))
    {
      throw error(previous(), "Expect expression before "
                                + tokenTypeString(previous().type) + ".");
    }
  auto expr = factor();

  while(match({TokenType::MINUS, TokenType::PLUS}))
    {
      Token op = previous();
      auto right = factor();
      expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

  return expr;
}

// factor         → unary ( ( "/" | "*" ) unary )* ;
auto Parser::factor() -> std::unique_ptr<Expr>
{
  if(match({TokenType::SLASH, TokenType::STAR}))
    {
      throw error(previous(), "Expect expression before "
                                + tokenTypeString(previous().type) + ".");
    }
  auto expr = unary();

  while(match({TokenType::SLASH, TokenType::STAR}))
    {
      Token op = previous();
      auto right = unary();
      expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

  return expr;
}

// unary          → ( "!" | "-" ) unary
//                | primary ;
auto Parser::unary() -> std::unique_ptr<Expr>
{
  if(match({TokenType::BANG, TokenType::MINUS}))
    {
      Token op = previous();
      auto right = unary();
      return std::make_unique<Unary>(op, std::move(right));
    }

  return primary();
}

// primary        → NUMBER | STRING | "true" | "false" | "nil"
//                | "(" expression ")" ;
auto Parser::primary() -> std::unique_ptr<Expr>
{
  if(match({TokenType::FALSE}))
    return std::make_unique<Literal>(false);
  if(match({TokenType::TRUE}))
    return std::make_unique<Literal>(true);
  if(match({TokenType::NIL}))
    return std::make_unique<Literal>(std::monostate{});

  if(match({TokenType::NUMBER, TokenType::STRING}))
    return std::make_unique<Literal>(std::move(previous().literal));

  if(match({TokenType::LEFT_PAREN}))
    {
      auto expr = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_unique<Grouping>(std::move(expr));
    }

  throw error(peek(), "Expect expression.");
}

void Parser::synchronize()
{
  advance();

  while(!isAtEnd())
    {
      if(previous().type == TokenType::SEMICOLON)
        return;

      switch(peek().type)
        {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN: return;
        default: advance(); break;
        }
    }
}

// need to see what initializer_list exactly does
auto Parser::match(std::initializer_list<TokenType> types) -> bool
{
  for(auto type : types)
    {
      if(check(type))
        {
          advance();
          return true;
        }
    }

  return false;
}

auto Parser::check(TokenType type) -> bool
{
  if(isAtEnd())
    return false;
  return peek().type == type;
}

auto Parser::advance() -> Token
{
  if(!isAtEnd())
    current++;
  return previous();
}

auto Parser::isAtEnd() -> bool { return peek().type == TokenType::EOF_TOK; }

[[nodiscard]] auto Parser::peek() const -> const Token &
{
  return tokens.at(current);
}

[[nodiscard]] auto Parser::previous() const -> const Token &
{
  return tokens.at(current - 1);
}

auto Parser::consume(TokenType type, std::string message) -> Token
{
  if(check(type))
    return advance();

  throw error(peek(), message);
}

auto Parser::error(Token token, std::string message) -> Parser::ParseError
{
  executor.error(token, message);
  return ParseError{};
}
