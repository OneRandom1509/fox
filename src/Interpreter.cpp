#include "Executor.hpp"
#include "Expr.hpp"
#include "Token.hpp"
#include "RuntimeError.hpp"
#include "Interpreter.hpp"
#include <iostream>
#include <string>
#include <variant>

Interpreter::Interpreter() = default;

void Interpreter::interpret(Expr &expr)
{
  try
    {
      literal_t value = evaluate(expr);
      std::cout << stringify(value) << std::endl;
    }
  catch(RuntimeError &err)
    {
      Executor::runtimeError(err);
    }
}

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

void Interpreter::checkNumberOperands(Token op, literal_t left,
                                      literal_t right) const
{
  if(std::holds_alternative<double>(left)
     && std::holds_alternative<double>(right))
    return;
  throw RuntimeError(op, "Operands must be numbers");
}

void Interpreter::checkNumberOperand(Token op, literal_t operand) const
{
  if(std::holds_alternative<double>(operand))
    return;
  throw RuntimeError(op, "Operand must be a number");
}

auto Interpreter::visitBinaryExpr(const Binary &expr) -> literal_t
{
  literal_t left = evaluate(*expr.left);
  literal_t right = evaluate(*expr.right);

  switch(expr.operator_.type)
    {
    case TokenType::GREATER:
      checkNumberOperands(expr.operator_, left, right);
      return std::get<double>(left) > std::get<double>(right);
    case TokenType::GREATER_EQUAL:
      checkNumberOperands(expr.operator_, left, right);
      return std::get<double>(left) >= std::get<double>(right);
    case TokenType::LESSER:
      checkNumberOperands(expr.operator_, left, right);
      return std::get<double>(left) < std::get<double>(right);
    case TokenType::LESSER_EQUAL:
      checkNumberOperands(expr.operator_, left, right);
      return std::get<double>(left) <= std::get<double>(right);

    case TokenType::MINUS:
      checkNumberOperands(expr.operator_, left, right);
      return std::get<double>(left) - std::get<double>(right);

    case TokenType::PLUS:
      if(std::holds_alternative<std::string>(left)
         && std::holds_alternative<std::string>(right))
        return std::get<std::string>(left) + std::get<std::string>(right);
      if(std::holds_alternative<double>(left)
         && std::holds_alternative<double>(right))
        return std::get<double>(left) + std::get<double>(right);
      throw RuntimeError(expr.operator_,
                         "Operands must be either two strings or two numbers");

    case TokenType::SLASH:
      checkNumberOperands(expr.operator_, left, right);
      return std::get<double>(left) / std::get<double>(right);
    case TokenType::STAR:
      checkNumberOperands(expr.operator_, left, right);
      return std::get<double>(left) * std::get<double>(right);

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
      return std::get<double>(right) * -1;
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
  // TODO
  return std::monostate{};
}

auto Interpreter::visitTernaryExpr(const Ternary &expr) -> literal_t
{
  literal_t condition = evaluate(*expr.condition);
  if(isTruthy(condition))
    return evaluate(*expr.left);

  return evaluate(*expr.right);
}
