#include "Resolver.hpp"
#include <algorithm>
#include <string>
#include <variant>
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
  resolveList(stmt.statements);
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
  declare(stmt.name, VarEntry::Kind::FUNCTION);
  // eager defining so function can refer to itself in its function body
  define(stmt.name);

  resolve(stmt.body);
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
      if(it != scopes.at(scopes_size - 1).end() && it->second.defined == false)
        {
          Executor::error(
            expr.name,
            "[RESOLVER]: Can't read local variable in its own initializer.");
        }
    }

  int scope_index = resolveLocal(&expr, expr.name);
  if(scope_index >= 0)
    scopes.at(static_cast<size_t>(scope_index)).at(expr.name.lexeme).used
      = true;
}

void Resolver::visitAssignExpr(const Assign &expr)
{
  resolve(expr.value);
  // Assignment alone does not count as "using" the variable; only reads do.
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

  for(const auto &argument : expr.arguments)
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
  // Everything is mandatory in ternary expr
  resolve(expr.condition);
  resolve(expr.left);
  resolve(expr.right);
}

void Resolver::visitCommaExpr(const Comma &expr)
{
  resolve(expr.left);
  resolve(expr.right);
}

void Resolver::visitFunctionBodyExpr(const FunctionBody &expr)
{
  FunctionGuard guard(currentFunction, FunctionType::FUNCTION);

  beginScope();
  for(const auto &param : expr.params)
    {
      declare(param, VarEntry::Kind::PARAMETER);
      define(param);
    }
  resolveList(expr.body);
  endScope();
}

void Resolver::resolve(std::vector<std::shared_ptr<Stmt>> statements)
{
  // A synthetic top-level scope so global declarations are tracked too
  beginScope();
  resolveList(statements);
  endScope();

  flushWarnings();
}

void Resolver::resolveList(const std::vector<std::shared_ptr<Stmt>> &statements)
{
  for(const auto &statement : statements)
    {
      resolve(statement);
    }
}

void Resolver::resolve(std::shared_ptr<Stmt> stmt) { stmt->accept(*this); }

void Resolver::resolve(std::shared_ptr<Expr> expr) { expr->accept(*this); }

auto Resolver::resolveLocal(const Expr *expr, Token name) -> int
{
  for(int i = static_cast<int>(scopes_size) - 1; i >= 0; i--)
    {
      if(scopes.at(static_cast<size_t>(i)).contains(name.lexeme))
        {
          interpreter.resolve(expr, static_cast<int>(scopes_size) - 1 - i);
          return i;
        }
    }
  return -1;
}

void Resolver::beginScope()
{
  // push an empty unordered_map
  scopes.emplace_back();
  scopes_size++;
}

void Resolver::endScope()
{
  // Collect unused-declaration warnings for this scope
  const auto &scope = scopes.back();
  for(const auto &[name, entry] : scope)
    {
      // If the variable/function name starts with '_' we ignore the warnings
      if(entry.used || (!name.empty() && name[0] == '_'))
        continue;

      std::string label;
      switch(entry.kind)
        {
        case VarEntry::Kind::VARIABLE: label = "unused variable"; break;
        case VarEntry::Kind::FUNCTION: label = "unused function"; break;
        case VarEntry::Kind::PARAMETER: label = "unused parameter"; break;
        }

      warnings.push_back(ResolverWarning{
        entry.line, name, "[RESOLVER]: " + label + " '" + name + "'."});
    }

  scopes.pop_back();
  scopes_size--;
}

void Resolver::declare(Token name, VarEntry::Kind kind)
{
  if(scopes.empty())
    return;

  if(scopes.at(scopes_size - 1).contains(name.lexeme))
    {
      Executor::error(
        name, "[RESOLVER]: Already a variable with this name in this scope.");
    }
  scopes.at(scopes_size - 1)
    .insert_or_assign(name.lexeme, VarEntry{name.line, kind, false, false});
}

void Resolver::define(Token name)
{
  if(scopes.empty())
    return;
  auto it = scopes.at(scopes_size - 1).find(name.lexeme);
  if(it != scopes.at(scopes_size - 1).end())
    it->second.defined = true;
}

void Resolver::flushWarnings()
{
  // Warnings are only surfaced in file mode; in the REPL they are noise, so
  // drop them silently.
  if(Executor::repl)
    {
      warnings.clear();
      return;
    }
  std::ranges::sort(
    warnings, [](const ResolverWarning &a, const ResolverWarning &b) -> bool {
      return a.line < b.line;
    });
  for(const ResolverWarning &warning : warnings)
    Executor::warn(Token{TokenType::IDENTIFIER, warning.lexeme,
                         std::monostate{}, warning.line},
                   warning.message);
  warnings.clear();
}
