#include "Executor.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <memory>
#include <utility>
#include <vector>
#include "Parser.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move((tokens))) {}

auto Parser::parse() -> std::vector<std::shared_ptr<Stmt>>
{
  std::vector<std::shared_ptr<Stmt>> statements;
  while(!isAtEnd())
    {
      statements.push_back(declaration());
    }

  return statements;
}

auto Parser::declaration() -> std::shared_ptr<Stmt>
{
  try
    {
      if(match({TokenType::LET}))
        return varDeclaration();

      return statement();
    }
  catch(ParseError &err)
    {
      synchronize();
      return nullptr;
    }
}

auto Parser::varDeclaration() -> std::shared_ptr<Stmt>
{
  Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

  std::shared_ptr<Expr> initializer = nullptr;
  if(match({TokenType::EQUAL}))
    {
      initializer = expression();
    }

  consume(TokenType::SEMICOLON, "Expect ; after value.");
  return std::make_shared<Var>(name, initializer);
}

auto Parser::statement() -> std::shared_ptr<Stmt>
{
  if(match({TokenType::IF}))
    return ifStatement();
  if(match({TokenType::PRINT}))
    return printStatement();
  if(match({TokenType::LEFT_BRACE}))
    return std::make_shared<Block>(block());

  return expressionStatement();
}

auto Parser::block() -> std::vector<std::shared_ptr<Stmt>>
{
  std::vector<std::shared_ptr<Stmt>> statements;

  while(!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
      statements.push_back(declaration());
    }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

auto Parser::printStatement() -> std::shared_ptr<Stmt>
{
  auto value = expression();
  consume(TokenType::SEMICOLON, "Expect ; after value.");
  return std::make_shared<Print>(value);
}

auto Parser::expressionStatement() -> std::shared_ptr<Stmt>
{
  auto value = expression();
  consume(TokenType::SEMICOLON, "Expect ; after value.");
  return std::make_shared<Expression>(value);
}

auto Parser::ifStatement() -> std::shared_ptr<Stmt>
{
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  auto condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  auto thenBranch = statement();
  std::shared_ptr<Stmt> elseBranch = nullptr;

  if(match({TokenType::ELSE}))
    {
      elseBranch = statement();
    }

  return std::make_shared<If>(condition, thenBranch, elseBranch);
}

auto Parser::expression() -> std::shared_ptr<Expr> { return assignment(); }

auto Parser::assignment() -> std::shared_ptr<Expr>
{
  auto expr = comma();

  if(match({TokenType::EQUAL}))
    {
      Token equals = previous();
      // since assignment is a right-associative operation, we have to parse
      // the right side first
      auto value = assignment();

      if(auto *var = dynamic_cast<Variable *>(expr.get()))
        {
          Token name = var->name;
          return std::make_shared<Assign>(name, value);
        }

      error(equals, "Invalid assignment target.");
    }

  return expr;
}

auto Parser::comma() -> std::shared_ptr<Expr>
{
  if(match({TokenType::COMMA}))
    {
      throw error(previous(), "Expect expression before COMMA.");
    }

  auto expr = ternary();
  while(match({TokenType::COMMA}))
    {
      auto right = comma();
      expr = std::make_shared<Comma>(expr, right);
    }

  return expr;
}

// TODO: catch only colon operator used error
auto Parser::ternary() -> std::shared_ptr<Expr>
{
  if(match({TokenType::QUESTION}))
    {
      throw error(previous(), "Expect condition before QUESTION.");
    }
  auto expr = logical();

  if(match({TokenType::QUESTION}))
    {
      auto left = ternary();
      if(match({TokenType::COLON}))
        {
          auto right = ternary();
          expr = std::make_shared<Ternary>(expr, left, right);
        }
      else
        {
          throw error(previous(), "Incomplete Ternary Expression.");
        }
    }

  return expr;
}

auto Parser::logical() -> std::shared_ptr<Expr>
{
  if(match({TokenType::AND, TokenType::OR}))
    {
      throw error(previous(), "Expect expression before "
                                + tokenTypeString(previous().type) + ".");
    }

  auto expr = equality();

  while(match({TokenType::AND, TokenType::OR}))
    {
      Token op = previous();
      auto right = equality();
      expr = std::make_shared<Binary>(expr, op, right);
    }

  return expr;
}

auto Parser::equality() -> std::shared_ptr<Expr>
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
      expr = std::make_shared<Binary>(expr, op, right);
    }

  return expr;
}

auto Parser::comparison() -> std::shared_ptr<Expr>
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
      expr = std::make_shared<Binary>(expr, op, right);
    }
  return expr;
}

auto Parser::term() -> std::shared_ptr<Expr>
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
      expr = std::make_shared<Binary>(expr, op, right);
    }

  return expr;
}

auto Parser::factor() -> std::shared_ptr<Expr>
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
      expr = std::make_shared<Binary>(expr, op, right);
    }

  return expr;
}

auto Parser::unary() -> std::shared_ptr<Expr>
{
  if(match({TokenType::BANG, TokenType::MINUS}))
    {
      Token op = previous();
      auto right = unary();
      return std::make_shared<Unary>(op, (right));
    }

  return primary();
}

auto Parser::primary() -> std::shared_ptr<Expr>
{
  if(match({TokenType::FALSE}))
    return std::make_shared<Literal>(false);
  if(match({TokenType::TRUE}))
    return std::make_shared<Literal>(true);
  if(match({TokenType::NIL}))
    return std::make_shared<Literal>(std::monostate{});

  if(match({TokenType::NUMBER, TokenType::STRING}))
    return std::make_shared<Literal>(previous().literal);

  if(match({TokenType::LEFT_PAREN}))
    {
      auto expr = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_shared<Grouping>(expr);
    }

  if(match({TokenType::IDENTIFIER}))
    return std::make_shared<Variable>(previous());

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
        case TokenType::LET:
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
  Executor::error(token, message);
  return ParseError{};
}
