#pragma once

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <string>
#include <vector>
#include "Environment.hpp"

class Interpreter : public VisitorStmt<void>, public VisitorExpr<literal_t>
{
public:
  Interpreter();
  void visitExpressionStmt(const Expression &stmt) override;
  void visitPrintStmt(const Print &stmt) override;
  void visitVarStmt(const Var &stmt) override;
  void visitBlockStmt(const Block &stmt) override;
  void visitIfStmt(const If &stmt) override;

  auto visitLiteralExpr(const Literal &expr) -> literal_t override;
  auto visitGroupingExpr(const Grouping &expr) -> literal_t override;
  auto visitUnaryExpr(const Unary &expr) -> literal_t override;
  auto visitBinaryExpr(const Binary &expr) -> literal_t override;
  auto visitLogicalExpr(const Logical &expr) -> literal_t override;
  auto visitCommaExpr(const Comma &expr) -> literal_t override;
  auto visitTernaryExpr(const Ternary &expr) -> literal_t override;
  auto visitVariableExpr(const Variable &expr) -> literal_t override;
  auto visitAssignExpr(const Assign &expr) -> literal_t override;

  void interpret(std::vector<std::shared_ptr<Stmt>> &statements);

private:
  literal_t result;
  Environment environment = Environment();

  inline void execute(Stmt &stmt);
  void executeBlock(const std::vector<std::shared_ptr<Stmt>> &statements,
                    std::shared_ptr<Environment> environment);
  auto inline evaluate(Expr &expr) -> literal_t;
  [[nodiscard]] auto inline isTruthy(literal_t literal) const -> bool;
  [[nodiscard]] auto inline isEqual(literal_t a, literal_t b) const -> bool;
  inline void checkNumberOperand(Token op, literal_t operand) const;
  [[nodiscard]] auto inline checkNumberOperands(literal_t left,
                                                literal_t right) const -> bool;
  inline void
  checkNumberOperands(Token op, literal_t left, literal_t right) const;
  [[nodiscard]] auto inline stringify(literal_t literal) const -> std::string;
  [[nodiscard]] auto asDouble(const literal_t &value) const -> double;
};
