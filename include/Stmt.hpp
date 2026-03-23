#pragma once

#include <memory>
#include <utility>
#include "Token.hpp"
#include "Expr.hpp"

class Expression;
class Print;

template <typename R> class VisitorStmt
{
public:
  virtual auto visitExpressionStmt(const class Expression &expr) -> R = 0;
  virtual auto visitPrintStmt(const class Print &expr) -> R = 0;
  virtual ~VisitorStmt() = default;
};

class Stmt
{
public:
  virtual ~Stmt() = default;
  virtual auto accept(VisitorStmt<void> &visitor) const -> void = 0;
  virtual auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t = 0;
};

class Expression : public Stmt
{
public:
  Expression(std::unique_ptr<Expr> expression)
      : expression(std::move(expression))
  {}

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitExpressionStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitExpressionStmt(*this);
  }

  const std::unique_ptr<Expr> expression;
};

class Print : public Stmt
{
public:
  Print(std::unique_ptr<Expr> expression) : expression(std::move(expression))
  {}

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitPrintStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitPrintStmt(*this);
  }

  const std::unique_ptr<Expr> expression;
};
