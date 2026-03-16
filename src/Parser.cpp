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
/// expression < equality < comparison < term < factor < unary < primary
Parser::Parser(Executor &executor, std::vector<Token> tokens)
    : tokens(std::move(tokens)), executor(executor)
{}

std::unique_ptr<Expr> Parser::parse()
{
  try
    {
      return expression();
    }
  catch(ParseError error)
    {
      return nullptr;
    }
}

// expression     → equality ;
std::unique_ptr<Expr> Parser::expression() { return equality(); }

// equality       → comparison ( ( "!=" | "==" ) comparison )* ;
std::unique_ptr<Expr> Parser::equality()
{
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
std::unique_ptr<Expr> Parser::comparison()
{
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
std::unique_ptr<Expr> Parser::term()
{
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
std::unique_ptr<Expr> Parser::factor()
{
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
std::unique_ptr<Expr> Parser::unary()
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
std::unique_ptr<Expr> Parser::primary()
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
bool Parser::match(std::initializer_list<TokenType> types)
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

bool Parser::check(TokenType type)
{
  if(isAtEnd())
    return false;
  return peek().type == type;
}

Token Parser::advance()
{
  if(!isAtEnd())
    current++;
  return previous();
}

bool Parser::isAtEnd() { return peek().type == TokenType::EOF_TOK; }

[[nodiscard]] const Token &Parser::peek() const { return tokens.at(current); }

[[nodiscard]] const Token &Parser::previous() const
{
  return tokens.at(current - 1);
}

Token Parser::consume(TokenType type, std::string message)
{
  if(check(type))
    return advance();

  throw error(peek(), message);
}

Parser::ParseError Parser::error(Token token, std::string message)
{
  executor.error(token, message);
  return ParseError{};
}
