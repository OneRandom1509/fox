#include "Executor.hpp"
#include "Token.hpp"
#include <unordered_map>
#include <vector>

class Scanner
{
public:
  Scanner(Executor &executor);
  std::vector<Token> scanTokens();

private:
  const std::string source;
  std::vector<Token> tokens;
  size_t start = 0;
  size_t current = 0;
  int line = 1;
  Executor &executor;
  static const std::unordered_map<std::string, TokenType> keywords;

  inline bool isAtEnd();
  inline char advance();

  void scanToken();

  inline void addToken(TokenType type);
  inline void addToken(TokenType type, literal_t literal);

  // simple one character look-ahead
  inline bool match(char expected);
  inline char peek();
  inline char peekNext();

  void string();
  void number();
  void identifier();

  bool isalpha(char c);
  bool isalnum(char c);
};
