#pragma once

#include "Expr.hpp"
#include <string>
#include <vector>

class AstPrinter : public Visitor
{
public:
  std::string print(const Expr &expr);

  void visitBinaryExpr(const Binary &expr) override;
  void visitGroupingExpr(const Grouping &expr) override;
  void visitLiteralExpr(const Literal &expr) override;
  void visitUnaryExpr(const Unary &expr) override;

private:
  std::string result;

  std::string
  parenthesize(const std::string &name, std::vector<const Expr *> exprs);
};
