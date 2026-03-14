#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sysexits.h>
#include <vector>

void defineType(std::ofstream &writer, const std::string &baseName,
                const std::string &className, const std::string &fieldList)
{
  writer << "class " << className << " : public " << baseName << " {\n";
  writer << "public:\n";

  // Constructor signature
  writer << "  " << className << "(" << fieldList << ")";

  std::vector<std::string> fields;
  std::stringstream ss(fieldList);
  std::string field;

  while(std::getline(ss, field, ','))
    {
      fields.push_back(field);
    }

  // Initializer list
  writer << " : ";

  bool first = true;
  for(auto &f : fields)
    {
      std::stringstream fs(f);
      std::string type, name;
      fs >> type >> name;

      if(!first)
        writer << ", ";
      writer << name << "(std::move(" << name << "))";
      first = false;
    }

  writer << " {}\n\n";

  // Fields
  for(auto &f : fields)
    {
      writer << "  const " << f << ";\n";
    }

  writer << "};\n\n";
}

void defineAst(const std::string &outputDir, const std::string &baseName,
               const std::vector<std::string> &types)
{
  std::string path = outputDir + "/" + baseName + ".hpp";
  std::ofstream writer(path);

  writer << "#pragma once\n\n";
  writer << "#include <memory>\n";
  writer << "#include <utility>\n";
  writer << "#include \"Token.hpp\"\n\n";

  writer << "class " << baseName << " {\n";
  writer << "public:\n";
  writer << "  virtual ~" << baseName << "() = default;\n";
  writer << "};\n\n";

  for(const auto &type : types)
    {
      auto pos = type.find(':');
      std::string className = type.substr(0, pos);
      std::string fields = type.substr(pos + 1);

      // trim spaces
      while(!className.empty() && className.back() == ' ')
        className.pop_back();
      while(!fields.empty() && fields.front() == ' ')
        fields.erase(fields.begin());

      defineType(writer, baseName, className, fields);
    }
}

int main(int argc, char *argv[])
{
  if(argc != 2)
    {
      std::cerr << "Usage: generate_ast <output directory>\n";
      exit(EX_USAGE);
    }

  std::string outputDir = argv[1];

  defineAst(outputDir, "Expr",
            {"Binary : std::shared_ptr<Expr> left, Token operator_, "
             "std::shared_ptr<Expr> right",
             "Grouping : std::shared_ptr<Expr> expression",
             "Literal : std::string value",
             "Unary : Token operator_, std::shared_ptr<Expr> right"});

  return 0;
}
