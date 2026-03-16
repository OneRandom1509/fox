#pragma once

#include "Expr.hpp"
#include <initializer_list>
#include <string>

class AstPrinter : public Visitor
{
public:
  auto print(const Expr &expr) -> std::string;

  void visitBinaryExpr(const Binary &expr) override;
  void visitGroupingExpr(const Grouping &expr) override;
  void visitLiteralExpr(const Literal &expr) override;
  void visitUnaryExpr(const Unary &expr) override;
  void visitCommaExpr(const Comma &expr) override;

private:
  std::string result;

  auto parenthesize(const std::string &name,
                    std::initializer_list<const Expr *> exprs) -> std::string;
};
