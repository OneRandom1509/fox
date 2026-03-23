#pragma once

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <string>
#include <vector>

class Interpreter : public VisitorStmt<void>, public VisitorExpr<literal_t>
{
public:
  Interpreter();
  void visitExpressionStmt(const Expression &stmt) override;
  void visitPrintStmt(const Print &stmt) override;

  auto visitLiteralExpr(const Literal &expr) -> literal_t override;
  auto visitGroupingExpr(const Grouping &expr) -> literal_t override;
  auto visitUnaryExpr(const Unary &expr) -> literal_t override;
  auto visitBinaryExpr(const Binary &expr) -> literal_t override;
  auto visitCommaExpr(const Comma &expr) -> literal_t override;
  auto visitTernaryExpr(const Ternary &expr) -> literal_t override;

  void interpret(std::vector<std::unique_ptr<Stmt>> &statements);

private:
  literal_t result;

  inline void execute(Stmt &stmt);
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
