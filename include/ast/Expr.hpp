#pragma once

#include <memory>
#include <utility>
#include <vector>
#include "Token.hpp"

class Stmt;
class Assign;
class Binary;
class Call;
class Comma;
class FunctionBody;
class Grouping;
class Literal;
class Logical;
class Ternary;
class Unary;
class Variable;

template <typename R> class VisitorExpr
{
public:
  virtual auto visitAssignExpr(const class Assign &expr) -> R = 0;
  virtual auto visitBinaryExpr(const class Binary &expr) -> R = 0;
  virtual auto visitCallExpr(const class Call &expr) -> R = 0;
  virtual auto visitCommaExpr(const class Comma &expr) -> R = 0;
  virtual auto visitFunctionBodyExpr(const class FunctionBody &expr) -> R = 0;
  virtual auto visitGroupingExpr(const class Grouping &expr) -> R = 0;
  virtual auto visitLiteralExpr(const class Literal &expr) -> R = 0;
  virtual auto visitLogicalExpr(const class Logical &expr) -> R = 0;
  virtual auto visitTernaryExpr(const class Ternary &expr) -> R = 0;
  virtual auto visitUnaryExpr(const class Unary &expr) -> R = 0;
  virtual auto visitVariableExpr(const class Variable &expr) -> R = 0;
  virtual ~VisitorExpr() = default;
};

class Expr
{
public:
  virtual ~Expr() = default;
  virtual auto accept(VisitorExpr<void> &visitor) const -> void = 0;
  virtual auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t = 0;
};

class Assign : public Expr
{
public:
  Assign(Token name, std::shared_ptr<Expr> value)
      : name(std::move(name)), value(std::move(value))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitAssignExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitAssignExpr(*this);
  }

  const Token name;
  const std::shared_ptr<Expr> value;
};

class Binary : public Expr
{
public:
  Binary(std::shared_ptr<Expr> left, Token operator_,
         std::shared_ptr<Expr> right)
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

  const std::shared_ptr<Expr> left;
  const Token operator_;
  const std::shared_ptr<Expr> right;
};

class Call : public Expr
{
public:
  Call(std::shared_ptr<Expr> callee, Token paren,
       std::vector<std::shared_ptr<Expr>> arguments)
      : callee(std::move(callee)), paren(std::move(paren)),
        arguments(std::move(arguments))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitCallExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitCallExpr(*this);
  }

  const std::shared_ptr<Expr> callee;
  const Token paren;
  const std::vector<std::shared_ptr<Expr>> arguments;
};

class Comma : public Expr
{
public:
  Comma(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
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

  const std::shared_ptr<Expr> left;
  const std::shared_ptr<Expr> right;
};

class FunctionBody : public Expr
{
public:
  FunctionBody(std::vector<Token> params,
               std::vector<std::shared_ptr<Stmt>> body)
      : params(std::move(params)), body(std::move(body))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitFunctionBodyExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitFunctionBodyExpr(*this);
  }

  const std::vector<Token> params;
  const std::vector<std::shared_ptr<Stmt>> body;
};

class Grouping : public Expr
{
public:
  Grouping(std::shared_ptr<Expr> expression)
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

  const std::shared_ptr<Expr> expression;
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

class Logical : public Expr
{
public:
  Logical(std::shared_ptr<Expr> left, Token operator_,
          std::shared_ptr<Expr> right)
      : left(std::move(left)), operator_(std::move(operator_)),
        right(std::move(right))
  {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitLogicalExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitLogicalExpr(*this);
  }

  const std::shared_ptr<Expr> left;
  const Token operator_;
  const std::shared_ptr<Expr> right;
};

class Ternary : public Expr
{
public:
  Ternary(std::shared_ptr<Expr> condition, std::shared_ptr<Expr> left,
          std::shared_ptr<Expr> right)
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

  const std::shared_ptr<Expr> condition;
  const std::shared_ptr<Expr> left;
  const std::shared_ptr<Expr> right;
};

class Unary : public Expr
{
public:
  Unary(Token operator_, std::shared_ptr<Expr> right)
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
  const std::shared_ptr<Expr> right;
};

class Variable : public Expr
{
public:
  Variable(Token name) : name(std::move(name)) {}

  auto accept(VisitorExpr<void> &visitor) const -> void override
  {
    visitor.visitVariableExpr(*this);
  }

  auto accept(VisitorExpr<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitVariableExpr(*this);
  }

  const Token name;
};
