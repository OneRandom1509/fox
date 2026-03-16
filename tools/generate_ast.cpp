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

void defineVisitor(std::ofstream &writer, const std::string &baseName,
                   const std::vector<TypeDef> &types)
{
  writer << "class Visitor {\n";
  writer << "public:\n";

  for(const auto &type : types)
    {
      writer << "  virtual void visit" << type.name << baseName << "(const "
             << type.name << "& expr) = 0;\n";
    }

  writer << "  virtual ~Visitor() = default;\n";
  writer << "};\n\n";
}

void defineType(std::ofstream &writer, const std::string &baseName,
                const TypeDef &type)
{
  auto fields = split(type.fields, ',');

  writer << "class " << type.name << " : public " << baseName << " {\n";
  writer << "public:\n";

  // Constructor
  writer << "  " << type.name << "(" << type.fields << ")";

  writer << " : ";

  bool first = true;
  for(auto &field : fields)
    {
      std::stringstream fs(field);
      std::string typeName, varName;
      fs >> typeName >> varName;

      if(!first)
        writer << ", ";

      writer << varName << "(std::move(" << varName << "))";
      first = false;
    }

  writer << " {}\n\n";

  // Visitor accept
  writer << "  void accept(Visitor& visitor) const override {\n";
  writer << "    visitor.visit" << type.name << baseName << "(*this);\n";
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

  writer << "#pragma once\n\n";
  writer << "#include <memory>\n";
  writer << "#include <utility>\n";
  writer << "#include \"Token.hpp\"\n\n";

  // Forward declarations
  for(const auto &type : types)
    {
      writer << "class " << type.name << ";\n";
    }

  writer << "\n";

  defineVisitor(writer, baseName, types);

  writer << "// Abstract class for expressions\n";
  writer << "class " << baseName << " {\n";
  writer << "public:\n";
  writer << "  virtual ~" << baseName << "() = default;\n";
  writer << "  virtual void accept(Visitor& visitor) const = 0;\n";
  writer << "};\n\n";

  for(const auto &type : types)
    {
      defineType(writer, baseName, type);
    }
}

auto main(int argc, char *argv[]) -> int
{
  if(argc != 2)
    {
      std::cerr << "Usage: generate_ast <output directory>\n";
      exit(EX_USAGE);
    }

  std::string outputDir = argv[1];

  std::vector<TypeDef> exprTypes
    = {{"Binary", "std::unique_ptr<Expr> left, Token operator_, "
                  "std::unique_ptr<Expr> right"},
       {"Grouping", "std::unique_ptr<Expr> expression"},
       {"Literal", "literal_t value"},
       {"Unary", "Token operator_, std::unique_ptr<Expr> right"},
       {"Comma", "std::unique_ptr<Expr> left, std::unique_ptr<Expr> right"},
       {"Ternary", "std::unique_ptr<Expr> condition, std::unique_ptr<Expr> "
                   "left, std::unique_ptr<Expr> right"}};

  defineAst(outputDir, "Expr", exprTypes);

  return 0;
}
