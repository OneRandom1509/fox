#pragma once

#include <memory>
#include <utility>
#include "Token.hpp"
#include "Expr.hpp"
#include <vector>

class Block;
class Expression;
class If;
class Print;
class Var;
class While;
class Break;
class Continue;

template <typename R> class VisitorStmt
{
public:
  virtual auto visitBlockStmt(const class Block &expr) -> R = 0;
  virtual auto visitExpressionStmt(const class Expression &expr) -> R = 0;
  virtual auto visitIfStmt(const class If &expr) -> R = 0;
  virtual auto visitPrintStmt(const class Print &expr) -> R = 0;
  virtual auto visitVarStmt(const class Var &expr) -> R = 0;
  virtual auto visitWhileStmt(const class While &expr) -> R = 0;
  virtual auto visitBreakStmt(const class Break &expr) -> R = 0;
  virtual auto visitContinueStmt(const class Continue &expr) -> R = 0;
  virtual ~VisitorStmt() = default;
};

class Stmt
{
public:
  virtual ~Stmt() = default;
  virtual auto accept(VisitorStmt<void> &visitor) const -> void = 0;
  virtual auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t = 0;
};

class Block : public Stmt
{
public:
  Block(std::vector<std::shared_ptr<Stmt>> statements)
      : statements(std::move(statements))
  {}

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitBlockStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitBlockStmt(*this);
  }

  const std::vector<std::shared_ptr<Stmt>> statements;
};

class Expression : public Stmt
{
public:
  Expression(std::shared_ptr<Expr> expression)
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

  const std::shared_ptr<Expr> expression;
};

class If : public Stmt
{
public:
  If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch,
     std::shared_ptr<Stmt> elseBranch)
      : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch))
  {}

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitIfStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitIfStmt(*this);
  }

  const std::shared_ptr<Expr> condition;
  const std::shared_ptr<Stmt> thenBranch;
  const std::shared_ptr<Stmt> elseBranch;
};

class Print : public Stmt
{
public:
  Print(std::shared_ptr<Expr> expression) : expression(std::move(expression))
  {}

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitPrintStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitPrintStmt(*this);
  }

  const std::shared_ptr<Expr> expression;
};

class Var : public Stmt
{
public:
  Var(Token name, std::shared_ptr<Expr> initializer)
      : name(std::move(name)), initializer(std::move(initializer))
  {}

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitVarStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitVarStmt(*this);
  }

  const Token name;
  const std::shared_ptr<Expr> initializer;
};

class While : public Stmt
{
public:
  While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
      : condition(std::move(condition)), body(std::move(body))
  {}

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitWhileStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitWhileStmt(*this);
  }

  const std::shared_ptr<Expr> condition;
  const std::shared_ptr<Stmt> body;
};

class Break : public Stmt
{
public:
  Break() = default;

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitBreakStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitBreakStmt(*this);
  }
};

class Continue : public Stmt
{
public:
  Continue() = default;

  auto accept(VisitorStmt<void> &visitor) const -> void override
  {
    visitor.visitContinueStmt(*this);
  }

  auto accept(VisitorStmt<literal_t> &visitor) const -> literal_t override
  {
    return visitor.visitContinueStmt(*this);
  }
};
