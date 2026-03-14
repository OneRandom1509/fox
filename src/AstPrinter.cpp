#include "Expr.hpp"
#include "Token.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

class AstPrinter : public Visitor
{
public:
  std::string print(const Expr &expr)
  {
    expr.accept(*this);
    return result;
  }

  void visitBinaryExpr(const Binary &expr) override
  {
    result = paranthesize(expr.operator_.lexeme,
                          {expr.left.get(), expr.right.get()});
  }

  void visitGroupingExpr(const Grouping &expr) override
  {
    result = paranthesize("group", {expr.expression.get()});
  }

  void visitLiteralExpr(const Literal &expr) override
  {
    result = literalToString(expr.value);
  }

  void visitUnaryExpr(const Unary &expr) override
  {
    result = paranthesize(expr.operator_.lexeme, {expr.right.get()});
  }

private:
  std::string result;

  std::string
  paranthesize(const std::string &name, std::vector<const Expr *> exprs)
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
};
