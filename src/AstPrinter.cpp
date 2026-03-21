#include "Expr.hpp"
#include "Token.hpp"
#include <initializer_list>
#include <memory>
#include <sstream>
#include <string>
#include "AstPrinter.hpp"

auto AstPrinter::print(const Expr &expr) -> std::string
{
  expr.accept(*this);
  return result;
}

void AstPrinter::visitBinaryExpr(const Binary &expr)
{
  result
    = parenthesize(expr.operator_.lexeme, {expr.left.get(), expr.right.get()});
}

void AstPrinter::visitGroupingExpr(const Grouping &expr)
{
  result = parenthesize("group", {expr.expression.get()});
}

void AstPrinter::visitLiteralExpr(const Literal &expr)
{
  result = literalToString(expr.value);
}

void AstPrinter::visitUnaryExpr(const Unary &expr)
{
  result = parenthesize(expr.operator_.lexeme, {expr.right.get()});
}

void AstPrinter::visitCommaExpr(const Comma &expr)
{
  result = parenthesize(",", {expr.left.get(), expr.right.get()});
}

void AstPrinter::visitTernaryExpr(const Ternary &expr)
{
  result = parenthesize(
    "?:", {expr.condition.get(), expr.left.get(), expr.right.get()});
}

auto AstPrinter::parenthesize(const std::string &name,
                              std::initializer_list<const Expr *> exprs)
  -> std::string
{
  std::stringstream builder;

  builder << "(" << name;

  for(const Expr *expr : exprs)
    {
      builder << " ";
      builder << this->print(*expr);
    }

  builder << ")";

  return builder.str();
}
