#include "Resolver.hpp"
#include <utility>
#include <vector>
#include "Executor.hpp"
#include "Expr.hpp"
#include "Interpreter.hpp"
#include "Stmt.hpp"
#include "Token.hpp"

Resolver::Resolver(Interpreter &interpreter) : interpreter(interpreter) {}

void Resolver::visitBlockStmt(const Block &stmt)
{
  beginScope();
  resolve(stmt.statements);
  endScope();
  return;
}

void Resolver::visitVarStmt(const Var &stmt)
{
  declare(stmt.name);
  if(stmt.initializer != nullptr)
    resolve(stmt.initializer);
  define(stmt.name);
  return;
}

void Resolver::visitFunctionStmt(const Function &stmt)
{
  declare(stmt.name);
  // eager defining so function can refer to itself in its function body
  define(stmt.name);

  resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visitExpressionStmt(const Expression &stmt)
{ resolve(stmt.expression); }

void Resolver::visitIfStmt(const If &stmt)
{
  // Resolves expression and both branches
  resolve(stmt.condition);
  resolve(stmt.thenBranch);
  if(stmt.elseBranch != nullptr)
    resolve(stmt.elseBranch);
}

void Resolver::visitPrintStmt(const Print &stmt) { resolve(stmt.expression); }

void Resolver::visitReturnStmt(const Return &stmt)
{
  if(currentFunction == FunctionType::NONE)
    {
      Executor::error(stmt.keyword,
                      "[RESOLVER]: Can't return from top-level code.");
    }
  if(stmt.value != nullptr)
    resolve(stmt.value);
}

void Resolver::visitWhileStmt(const While &stmt)
{
  resolve(stmt.condition);
  resolve(stmt.body);
}

void Resolver::visitContinueStmt([[maybe_unused]] const Continue &stmt) {}

void Resolver::visitBreakStmt([[maybe_unused]] const Break &stmt) {}

void Resolver::visitVariableExpr(const Variable &expr)
{
  if(!scopes.empty())
    {
      auto it = scopes.at(scopes_size - 1).find(expr.name.lexeme);
      if(it != scopes.at(scopes_size - 1).end() && it->second == false)
        {
          Executor::error(
            expr.name,
            "[RESOLVER]:Can't read local variable in its own initializer.");
        }
    }
  resolveLocal(&expr, expr.name);
}

void Resolver::visitAssignExpr(const Assign &expr)
{
  resolve(expr.value);
  resolveLocal(&expr, expr.name);
}

void Resolver::visitBinaryExpr(const Binary &expr)
{
  resolve(expr.left);
  resolve(expr.right);
}

void Resolver::visitCallExpr(const Call &expr)
{
  resolve(expr.callee);

  for(auto argument : expr.arguments)
    {
      resolve(argument);
    }
}

void Resolver::visitGroupingExpr(const Grouping &expr)
{ resolve(expr.expression); }

void Resolver::visitLiteralExpr([[maybe_unused]] const Literal &expr) {}

void Resolver::visitLogicalExpr(const Logical &expr)
{
  resolve(expr.left);
  resolve(expr.right);
}

void Resolver::visitUnaryExpr(const Unary &expr) { resolve(expr.right); }

void Resolver::visitTernaryExpr(const Ternary &expr)
{
  resolve(expr.condition);
  resolve(expr.left);
  if(expr.right != nullptr)
    {
      resolve(expr.right);
    }
}

void Resolver::visitCommaExpr(const Comma &expr)
{
  resolve(expr.left);
  resolve(expr.right);
}

void Resolver::visitFunctionBodyExpr([[maybe_unused]] const FunctionBody &expr)
{}

void Resolver::resolve(std::vector<std::shared_ptr<Stmt>> statements)
{
  for(auto statement : statements)
    {
      resolve(statement);
    }
}

void Resolver::resolve(std::shared_ptr<Stmt> stmt) { stmt->accept(*this); }

void Resolver::resolve(std::shared_ptr<Expr> expr) { expr->accept(*this); }

void Resolver::resolveLocal(const Expr *expr, Token name)
{
  for(int i = static_cast<int>(scopes_size) - 1; i >= 0; i--)
    {
      if(scopes.at(i).contains(name.lexeme))
        {
          interpreter.resolve(expr, static_cast<int>(scopes_size) - 1 - i);
          return;
        }
    }
}

void Resolver::resolveFunction(const Function &function, FunctionType type)
{
  FunctionGuard guard(currentFunction, type);

  beginScope();
  auto params = std::dynamic_pointer_cast<FunctionBody>(function.body)->params;
  for(auto &param : params)
    {
      declare(param);
      define(param);
    }
  resolve(function.body);
  endScope();
}

void Resolver::beginScope()
{
  // push an empty unordered_map
  scopes.emplace_back();
  scopes_size++;
}

void Resolver::endScope()
{
  scopes.pop_back();
  scopes_size--;
}

void Resolver::declare(Token name)
{
  if(scopes.empty())
    return;

  if(scopes.at(scopes_size - 1).contains(name.lexeme))
    {
      Executor::error(
        name, "[RESOLVER]: Already a variable with this name in this scope.");
    }
  scopes.at(scopes_size - 1).insert_or_assign(name.lexeme, false);
}

void Resolver::define(Token name)
{
  if(scopes.empty())
    return;
  scopes.at(scopes_size - 1).insert_or_assign(name.lexeme, true);
}
