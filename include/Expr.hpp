#pragma once

#include <memory>
#include <utility>
#include "Token.hpp"

class Binary;
class Grouping;
class Literal;
class Unary;
class Comma;

class Visitor
{
public:
  virtual void visitBinaryExpr(const Binary &expr) = 0;
  virtual void visitGroupingExpr(const Grouping &expr) = 0;
  virtual void visitLiteralExpr(const Literal &expr) = 0;
  virtual void visitUnaryExpr(const Unary &expr) = 0;
  virtual void visitCommaExpr(const Comma &expr) = 0;
  virtual ~Visitor() = default;
};

// Abstract class for expressions
class Expr
{
public:
  virtual ~Expr() = default;
  virtual void accept(Visitor &visitor) const = 0;
};

class Binary : public Expr
{
public:
  Binary(std::unique_ptr<Expr> left, Token operator_,
         std::unique_ptr<Expr> right)
      : left(std::move(left)), operator_(std::move(operator_)),
        right(std::move(right))
  {}

  void accept(Visitor &visitor) const override
  {
    visitor.visitBinaryExpr(*this);
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

  void accept(Visitor &visitor) const override
  {
    visitor.visitGroupingExpr(*this);
  }

  const std::unique_ptr<Expr> expression;
};

class Literal : public Expr
{
public:
  Literal(literal_t value) : value(std::move(value)) {}

  void accept(Visitor &visitor) const override
  {
    visitor.visitLiteralExpr(*this);
  }

  const literal_t value;
};

class Unary : public Expr
{
public:
  Unary(Token operator_, std::unique_ptr<Expr> right)
      : operator_(std::move(operator_)), right(std::move(right))
  {}

  void accept(Visitor &visitor) const override
  {
    visitor.visitUnaryExpr(*this);
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

  void accept(Visitor &visitor) const override
  {
    visitor.visitCommaExpr(*this);
  }

  const std::unique_ptr<Expr> left;
  const std::unique_ptr<Expr> right;
};
