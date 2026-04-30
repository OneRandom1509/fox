#include "Executor.hpp"
#include "Expr.hpp"
#include "Token.hpp"
#include "RuntimeError.hpp"
#include "Interpreter.hpp"
#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include "Environment.hpp"

Interpreter::Interpreter() = default;

void Interpreter::interpret(std::vector<std::shared_ptr<Stmt>> &statements)
{
  try
    {
      for(const auto &statement : statements)
        {
          execute(*statement);
        }
    }
  catch(RuntimeError &err)
    {
      Executor::runtimeError(err);
    }
}

void Interpreter::execute(Stmt &stmt) { stmt.accept(*this); }

auto Interpreter::stringify(literal_t literal) const -> std::string
{
  if(std::holds_alternative<std::monostate>(literal))
    return "nil";
  if(std::holds_alternative<double>(literal))
    // 6 digit precision
    return std::to_string(std::get<double>(literal));
  if(std::holds_alternative<bool>(literal))
    {
      if(std::get<bool>(literal) == true)
        return "true";
      else
        return "false";
    }
  return std::get<std::string>(literal);
}

auto Interpreter::checkNumberOperands(literal_t left, literal_t right) const
  -> bool
{
  if((std::holds_alternative<double>(left)
      || std::holds_alternative<bool>(left))
     && (std::holds_alternative<double>(right)
         || std::holds_alternative<bool>(right)))
    return true;
  return false;
}

void Interpreter::checkNumberOperands(Token op, literal_t left,
                                      literal_t right) const
{
  if((std::holds_alternative<double>(left)
      || std::holds_alternative<bool>(left))
     && (std::holds_alternative<double>(right)
         || std::holds_alternative<bool>(right)))
    return;
  throw RuntimeError(op, "Operands must be numbers");
}

void Interpreter::checkNumberOperand(Token op, literal_t operand) const
{
  if(std::holds_alternative<double>(operand)
     || std::holds_alternative<bool>(operand))
    return;
  throw RuntimeError(op, "Operand must be a number");
}

auto Interpreter::asDouble(const literal_t &value) const -> double
{
  if(std::holds_alternative<double>(value))
    {
      return std::get<double>(value);
    }
  // Convert bool: true -> 1.0, false -> 0.0
  return std::get<bool>(value) ? 1.0 : 0.0;
}

auto Interpreter::visitBinaryExpr(const Binary &expr) -> literal_t
{
  literal_t left = evaluate(*expr.left);
  literal_t right = evaluate(*expr.right);

  switch(expr.operator_.type)
    {
    case TokenType::GREATER:
      checkNumberOperands(expr.operator_, left, right);
      return asDouble(left) > asDouble(right);
    case TokenType::GREATER_EQUAL:
      checkNumberOperands(expr.operator_, left, right);
      return asDouble(left) >= asDouble(right);
    case TokenType::LESSER:
      checkNumberOperands(expr.operator_, left, right);
      return asDouble(left) < asDouble(right);
    case TokenType::LESSER_EQUAL:
      checkNumberOperands(expr.operator_, left, right);
      return asDouble(left) <= asDouble(right);

    case TokenType::MINUS:
      checkNumberOperands(expr.operator_, left, right);
      return asDouble(left) - asDouble(right);

    case TokenType::PLUS:
      if(checkNumberOperands(left, right))
        return asDouble(left) + asDouble(right);
      return stringify(left) + stringify(right);

    case TokenType::SLASH:
      checkNumberOperands(expr.operator_, left, right);
      if(asDouble(right) == 0)
        throw RuntimeError(
          expr.operator_, "Division by 0 will result in undefined behaviour.");
      return asDouble(left) / asDouble(right);

    case TokenType::STAR:
      checkNumberOperands(expr.operator_, left, right);
      return asDouble(left) * asDouble(right);

    case TokenType::BANG_EQUAL: return !isEqual(left, right);
    case TokenType::EQUAL_EQUAL: return isEqual(left, right);

    default: break;
    }

  return std::monostate{};
}

auto Interpreter::isEqual(literal_t a, literal_t b) const -> bool
{
  if(std::holds_alternative<std::monostate>(a)
     && std::holds_alternative<std::monostate>(b))
    return true;
  if(std::holds_alternative<std::monostate>(a))
    return false;
  if(checkNumberOperands(a, b))
    return asDouble(a) == asDouble(b);

  return a == b;
}

auto Interpreter::evaluate(Expr &expr) -> literal_t
{
  return expr.accept(*this);
}

auto Interpreter::isTruthy(literal_t literal) const -> bool
{
  if(std::holds_alternative<std::monostate>(literal))
    return false;
  if(std::holds_alternative<bool>(literal))
    return std::get<bool>(literal);
  if(std::holds_alternative<double>(literal))
    {
      if(std::get<double>(literal) == 0)
        return false;
    }

  return true;
}

auto Interpreter::visitUnaryExpr(const Unary &expr) -> literal_t
{
  literal_t right = evaluate(*expr.right);

  switch(expr.operator_.type)
    {
    case TokenType::BANG: return isTruthy(right);
    case TokenType::MINUS:
      checkNumberOperand(expr.operator_, right);
      return asDouble(right) * -1;
    default: break;
    }

  return std::monostate{};
}

auto Interpreter::visitGroupingExpr(const Grouping &expr) -> literal_t
{
  return evaluate(*expr.expression);
}

auto Interpreter::visitLiteralExpr(const Literal &expr) -> literal_t
{
  return expr.value;
}

auto Interpreter::visitCommaExpr(const Comma &expr) -> literal_t
{
  literal_t left = evaluate(*expr.left);
  literal_t right = evaluate(*expr.right);

  return right;
}

auto Interpreter::visitTernaryExpr(const Ternary &expr) -> literal_t
{
  literal_t condition = evaluate(*expr.condition);
  if(isTruthy(condition))
    return evaluate(*expr.left);

  return evaluate(*expr.right);
}

auto Interpreter::visitVariableExpr(const Variable &expr) -> literal_t
{
  return environment->get(expr.name);
}

auto Interpreter::visitAssignExpr(const Assign &expr) -> literal_t
{
  literal_t value = evaluate(*expr.value);
  // true is passed as it is obvious that the variable is (being) initialized
  environment->assign(expr.name, {true, value});
  return value;
}

auto Interpreter::visitLogicalExpr(const Logical &expr) -> literal_t
{
  literal_t left = evaluate(*expr.left);

  if(expr.operator_.type == TokenType::OR)
    {
      if(isTruthy(left))
        return left;
    }
  else
    {
      if(!isTruthy(left))
        return left;
    }

  return evaluate(*expr.right);
}

void Interpreter::visitExpressionStmt(const Expression &stmt)
{
  auto value = evaluate(*stmt.expression);
  // if it's in REPL mode, output the expression's value directly
  if(Executor::repl)
    std::cout << stringify(value) << std::endl;
  return;
}

void Interpreter::visitPrintStmt(const Print &stmt)
{
  literal_t value = evaluate(*stmt.expression);
  std::cout << stringify(value) << std::endl;
  return;
}

void Interpreter::visitVarStmt(const Var &stmt)
{
  literal_t value = std::monostate{};
  bool initialized = false;
  if(stmt.initializer != nullptr)
    {
      initialized = true;
      value = evaluate(*stmt.initializer);
    }

  environment->define(stmt.name.lexeme, {initialized, value});
  return;
}

void Interpreter::visitBlockStmt(const Block &stmt)
{
  executeBlock(stmt.statements,
               std::make_shared<Environment>(this->environment));
  return;
}

void Interpreter::visitIfStmt(const If &stmt)
{
  if(isTruthy(evaluate(*stmt.condition)))
    {
      execute(*stmt.thenBranch);
    }
  else if(stmt.elseBranch != nullptr)
    {
      execute(*stmt.elseBranch);
    }

  return;
}

void Interpreter::visitWhileStmt(const While &stmt)
{
  while(isTruthy(evaluate(*stmt.condition)))
    {
      try
        {
          execute(*stmt.body);
        }

      // Catching and executing Control Flow signals
      catch(BreakFlow &e)
        {
          break;
        }
      catch(ContinueFlow &e)
        {
          continue;
        }
    }
  return;
}

void Interpreter::executeBlock(
  const std::vector<std::shared_ptr<Stmt>> &statements,
  std::shared_ptr<Environment> environment)
{
  auto previous = this->environment;
  try
    {
      this->environment = environment;

      for(const auto &statement : statements)
        {
          execute(*statement);
        }
      this->environment = previous;
    }
  catch(...)
    {
      this->environment = previous;
      throw;
    }
}

// stmt argument is not used
void Interpreter::visitBreakStmt(const Break &stmt) { throw BreakFlow{}; }

// stmt argument is not used
void Interpreter::visitContinueStmt(const Continue &stmt)
{
  throw ContinueFlow{};
}
