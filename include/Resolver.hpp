#pragma once

#include "Expr.hpp"
#include "Interpreter.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

enum class FunctionType
{
  NONE,
  FUNCTION
};

struct VarEntry
{
  enum class Kind
  {
    VARIABLE,
    FUNCTION,
    PARAMETER
  };

  bool defined = false;
  bool used = false;
  int line = 0;
  Kind kind = Kind::VARIABLE;

  VarEntry() = default;
  VarEntry(int line, Kind kind, bool defined = false, bool used = false)
      : defined(defined), used(used), line(line), kind(kind)
  {}
};

struct ResolverWarning
{
  int line;
  std::string lexeme;
  std::string message;
};

// Just like LoopGuard
struct FunctionGuard
{
  FunctionType &curr;
  FunctionType prev;

  FunctionGuard(FunctionType &target, FunctionType next)
      : curr(target), prev(target)
  { curr = next; }

  ~FunctionGuard() { curr = prev; }
};

/// This resolver class makes a single pass through the AST nodes to resolve
/// the variable declarations and bind it properly
class Resolver : public VisitorExpr<void>, public VisitorStmt<void>
{
public:
  Resolver(Interpreter &interpreter);
  void visitBlockStmt(const Block &stmt) override;
  void visitVarStmt(const Var &stmt) override;
  void visitFunctionStmt(const Function &stmt) override;
  void visitExpressionStmt(const Expression &stmt) override;
  void visitIfStmt(const If &stmt) override;
  void visitPrintStmt(const Print &stmt) override;
  void visitReturnStmt(const Return &stmt) override;
  void visitWhileStmt(const While &stmt) override;
  void visitContinueStmt(const Continue &stmt) override;
  void visitBreakStmt(const Break &stmt) override;

  void visitVariableExpr(const Variable &expr) override;
  void visitAssignExpr(const Assign &expr) override;
  void visitBinaryExpr(const Binary &expr) override;
  void visitCallExpr(const Call &expr) override;
  void visitGroupingExpr(const Grouping &expr) override;
  void visitLiteralExpr(const Literal &expr) override;
  void visitLogicalExpr(const Logical &expr) override;
  void visitUnaryExpr(const Unary &expr) override;
  void visitTernaryExpr(const Ternary &expr) override;
  void visitCommaExpr(const Comma &expr) override;
  void visitFunctionBodyExpr(const FunctionBody &expr) override;

  void resolve(std::vector<std::shared_ptr<Stmt>> statements);

private:
  Interpreter &interpreter;
  std::vector<std::unordered_map<std::string, VarEntry>> scopes;
  std::vector<ResolverWarning> warnings;
  FunctionType currentFunction = FunctionType::NONE;
  size_t scopes_size = 0;

  inline void resolve(std::shared_ptr<Stmt>);
  inline void resolve(std::shared_ptr<Expr>);
  void resolveList(const std::vector<std::shared_ptr<Stmt>> &statements);
  auto resolveLocal(const Expr *expr, Token name) -> int;
  void resolveFunction(const Function &function, FunctionType type);

  inline void beginScope();
  inline void endScope();
  void flushWarnings();

  void declare(Token name, VarEntry::Kind kind = VarEntry::Kind::VARIABLE);
  void define(Token name);
};
