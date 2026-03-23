#pragma once

#include <memory>
#include <utility>
#include "Token.hpp"

class Binary;
class Grouping;
class Literal;
class Unary;
class Comma;
class Ternary;

template <typename R> class VisitorExpr
{
public:
  virtual auto visitBinaryExpr(const class Binary &expr) -> R = 0;
  virtual auto visitGroupingExpr(const class Grouping &expr) -> R = 0;
  virtual auto visitLiteralExpr(const class Literal &expr) -> R = 0;
  virtual auto visitUnaryExpr(const class Unary &expr) -> R = 0;
  virtual auto visitCommaExpr(const class Comma &expr) -> R = 0;
  virtual auto visitTernaryExpr(const class Ternary &expr) -> R = 0;
  virtual ~VisitorExpr() = default;
};

class Expr
{
public:
  virtual ~Expr() = default;
  virtual auto accept(VisitorExpr<void> &visitor) const -> void = 0;
  virtual auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t = 0;
};

class Binary : public Expr
{
public:
  Binary(std::unique_ptr<Expr> left, Token operator_,
         std::unique_ptr<Expr> right)
      : left(std::move(left)), operator_(std::move(operator_)),
        right(std::move(right))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitBinaryExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitBinaryExpr(*this);
  }

  const std::unique_ptr<Expr> left;
  const Token operator_;
  const std::unique_ptr<Expr> right;
};

class Grouping : public Expr
{
public:
  Grouping(std::unique_ptr<Expr> expression)
      : expression(std::move(expression))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitGroupingExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitGroupingExpr(*this);
  }

  const std::unique_ptr<Expr> expression;
};

class Literal : public Expr
{
public:
  Literal(literal_t value) : value(std::move(value)) {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitLiteralExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitLiteralExpr(*this);
  }

  const literal_t value;
};

class Unary : public Expr
{
public:
  Unary(Token operator_, std::unique_ptr<Expr> right)
      : operator_(std::move(operator_)), right(std::move(right))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitUnaryExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitUnaryExpr(*this);
  }

  const Token operator_;
  const std::unique_ptr<Expr> right;
};

class Comma : public Expr
{
public:
  Comma(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
      : left(std::move(left)), right(std::move(right))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitCommaExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitCommaExpr(*this);
  }

  const std::unique_ptr<Expr> left;
  const std::unique_ptr<Expr> right;
};

class Ternary : public Expr
{
public:
  Ternary(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> left,
          std::unique_ptr<Expr> right)
      : condition(std::move(condition)), left(std::move(left)),
        right(std::move(right))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitTernaryExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitTernaryExpr(*this);
  }

  const std::unique_ptr<Expr> condition;
  const std::unique_ptr<Expr> left;
  const std::unique_ptr<Expr> right;
};
