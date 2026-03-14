#include "Expr.hpp"
#include "Token.hpp"
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "AstPrinter.hpp"

std::string AstPrinter::print(const Expr &expr)
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

std::string AstPrinter::parenthesize(const std::string &name,
                                     std::vector<const Expr *> exprs)
{
  std::stringstream builder;

  builder << "(" << name;

  for(const Expr *expr : exprs)
    {
      builder << " ";
      AstPrinter printer;
      builder << printer.print(*expr);
    }

  builder << ")";

  return builder.str();
}
