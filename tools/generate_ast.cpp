#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sysexits.h>
#include <vector>

struct TypeDef
{
  std::string name;
  std::string fields;
};

auto getVarName(const std::string &field) -> std::string
{
  std::stringstream ss(field);
  std::string type, name;
  ss >> type;
  while(ss >> name)
    ;
  return name;
}

auto split(const std::string &str, char delim) -> std::vector<std::string>
{
  std::vector<std::string> result;
  std::stringstream ss(str);
  std::string item;
  while(std::getline(ss, item, delim))
    {
      if(!item.empty() && item.front() == ' ')
        item.erase(0, 1);
      result.push_back(item);
    }
  return result;
}

// Changed to use the specific visitor name (e.g., VisitorExpr)
void defineVisitor(std::ofstream &writer, const std::string &baseName,
                   const std::vector<TypeDef> &types)
{
  writer << "template <typename R>\n";
  writer << "class Visitor" << baseName << " {\n"; // e.g., VisitorExpr
  writer << "public:\n";

  for(const auto &type : types)
    {
      writer << "  virtual auto visit" << type.name << baseName
             << "(const class " << type.name << "& expr) -> R = 0;\n";
    }

  writer << "  virtual ~Visitor" << baseName << "() = default;\n";
  writer << "};\n\n";
}

void defineType(std::ofstream &writer, const std::string &baseName,
                const TypeDef &type)
{
  auto fields = split(type.fields, ',');
  std::string visitorName = "Visitor" + baseName;

  writer << "class " << type.name << " : public " << baseName << " {\n";
  writer << "public:\n";

  // Constructor
  writer << "  " << type.name << "(" << type.fields << ")";
  writer << " : ";

  bool first = true;
  for(auto &field : fields)
    {
      std::string varName = getVarName(field);
      if(!first)
        writer << ", ";
      writer << varName << "(std::move(" << varName << "))";
      first = false;
    }
  writer << " {}\n\n";

  // 1. Implementation for AstPrinter (void)
  writer << "  auto accept(" << visitorName
         << "<void>& visitor) const -> void override {\n";
  writer << "    visitor.visit" << type.name << baseName << "(*this);\n";
  writer << "  }\n\n";

  // 2. Implementation for Interpreter (literal_t)
  writer << "  auto accept(" << visitorName
         << "<literal_t>& visitor) const -> literal_t override {\n";
  writer << "    return visitor.visit" << type.name << baseName
         << "(*this);\n";
  writer << "  }\n\n";

  // Fields
  for(auto &field : fields)
    {
      writer << "  const " << field << ";\n";
    }
  writer << "};\n\n";
}

void defineAst(const std::string &outputDir, const std::string &baseName,
               const std::vector<TypeDef> &types)
{
  std::string path = outputDir + "/" + baseName + ".hpp";
  std::ofstream writer(path);
  std::string visitorName = "Visitor" + baseName;

  writer << "#pragma once\n\n";
  writer << "#include <memory>\n";
  writer << "#include <utility>\n";
  writer << "#include <vector>\n";
  writer << "#include \"Token.hpp\"\n";

  // Include Expr.hpp if we are generating Stmt.hpp
  if(baseName == "Stmt")
    {
      writer << "#include \"Expr.hpp\"\n";
    }
  writer << "\n";

  // Forward declarations for the types
  for(const auto &type : types)
    {
      writer << "class " << type.name << ";\n";
    }
  writer << "\n";

  defineVisitor(writer, baseName, types);

  writer << "class " << baseName << " {\n";
  writer << "public:\n";
  writer << "  virtual ~" << baseName << "() = default;\n";

  writer << "  virtual auto accept(" << visitorName
         << "<void>& visitor) const -> void = 0;\n";
  writer << "  virtual auto accept(" << visitorName
         << "<literal_t>& visitor) const -> literal_t = 0;\n";

  writer << "};\n\n";

  for(const auto &type : types)
    {
      defineType(writer, baseName, type);
    }
}

auto main(int argc, char *argv[]) -> int
{
  if(argc != 3)
    {
      std::cerr
        << "Usage: generate_ast <output directory> <0 for Expr, 1 for Stmt>\n";
      exit(EX_USAGE);
    }

  std::string outputDir = argv[1];

  std::vector<TypeDef> exprTypes
    = {{"Assign", "Token name, std::shared_ptr<Expr> value"},
       {"Binary", "std::shared_ptr<Expr> left, Token operator_, "
                  "std::shared_ptr<Expr> right"},
       {"Call", "std::shared_ptr<Expr> callee, Token paren, "
                "std::vector<std::shared_ptr<Expr>> arguments"},
       {"Comma", "std::shared_ptr<Expr> left, std::shared_ptr<Expr> right"},
       {"Grouping", "std::shared_ptr<Expr> expression"},
       {"Literal", "literal_t value"},
       {"Logical", "std::shared_ptr<Expr> left, Token operator_, "
                   "std::shared_ptr<Expr> right"},
       {"Ternary", "std::shared_ptr<Expr> condition, std::shared_ptr<Expr> "
                   "left, std::shared_ptr<Expr> right"},
       {"Unary", "Token operator_, std::shared_ptr<Expr> right"},
       {"Variable", "Token name"}};

  // Break and Continue requires a bit of cleanup after generation for their
  // constructor as it is empty, it needs to be defaulted
  std::vector<TypeDef> stmtTypes = {
    {"Block", "std::vector<std::shared_ptr<Stmt>> statements"},
    {"Break", ""},
    {"Continue", ""},
    {"Expression", "std::shared_ptr<Expr> expression"},
    {"Function", "Token name, std::vector<Token> params, "
                 "std::vector<std::shared_ptr<Stmt>> body"},
    {"If", "std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> "
           "thenBranch, std::shared_ptr<Stmt> elseBranch"},
    {"Print", "std::shared_ptr<Expr> expression"},
    {"Return", "Token keyword, std::shared_ptr<Expr> value"},
    {"Var", "Token name, std::shared_ptr<Expr> initializer"},
    {"While", "std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body"}};

  if(strcmp(argv[2], "0") == 0)
    {
      defineAst(outputDir, "Expr", exprTypes);
    }
  else if(strcmp(argv[2], "1") == 0)
    {
      defineAst(outputDir, "Stmt", stmtTypes);
    }
  else
    {
      std::cerr << "Invalid mode. Use 0 for Expr or 1 for Stmt.\n";
      exit(EX_USAGE);
    }

  return 0;
}
