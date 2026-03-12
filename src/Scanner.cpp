#include "Executor.hpp"
#include "Token.hpp"
#include "Scanner.hpp"
#include <cctype>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

const std::unordered_map<std::string, TokenType> Scanner::keywords
  = {{"and", TokenType::AND},       {"class", TokenType::CLASS},
     {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
     {"for", TokenType::FOR},       {"fun", TokenType::FUN},
     {"if", TokenType::IF},         {"nil", TokenType::NIL},
     {"or", TokenType::OR},         {"print", TokenType::PRINT},
     {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
     {"this", TokenType::THIS},     {"true", TokenType::TRUE},
     {"var", TokenType::VAR},       {"while", TokenType::WHILE}};

Scanner::Scanner(Executor &executor)
    : source(std::move(executor.source)), executor(executor)
{}

std::vector<Token> Scanner::scanTokens()
{
  while(!isAtEnd())
    {
      // Beginning of the next lexeme
      start = current;
      scanToken();
    }

  tokens.emplace_back(TokenType::EOF_TOK, "", std::monostate{}, line);
  return tokens;
}

bool Scanner::isAtEnd() { return current >= source.length(); }

void Scanner::scanToken()
{
  char c = advance();
  switch(c)
    {
    case '(': addToken(TokenType::LEFT_PAREN); break;
    case ')': addToken(TokenType::RIGHT_PAREN); break;
    case '{': addToken(TokenType::LEFT_BRACE); break;
    case '}': addToken(TokenType::RIGHT_BRACE); break;
    case ',': addToken(TokenType::COMMA); break;
    case '.': addToken(TokenType::DOT); break;
    case '-': addToken(TokenType::MINUS); break;
    case '+': addToken(TokenType::PLUS); break;
    case ';': addToken(TokenType::SEMICOLON); break;
    case '*': addToken(TokenType::STAR); break;

    case '!':
      addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '<':
      addToken(match('=') ? TokenType::LESSER_EQUAL : TokenType::LESSER);
      break;
    case '>':
      addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '/':
      if(match('/'))
        {
          // A comment goes until the end of the line
          while(peek() != '\n' && !isAtEnd())
            advance();
        }
      else
        {
          addToken(TokenType::SLASH);
        }
      break;

    // ignoring whitespaces
    case ' ': break;
    case '\r': break;
    case '\t': break;

    case '\n': line++; break;

    // double quotes
    case '\"': string(); break;

    default:
      if(std::isdigit(c))
        {
          number();
        }
      else if(isalpha(c))
        {
          identifier();
        }
      else
        {
          executor.error(line, "Unexpected character.");
          break;
        }
    }
}

char Scanner::advance() { return source.at(current++); }

void Scanner::addToken(TokenType type) { addToken(type, std::monostate{}); }

void Scanner::addToken(TokenType type, Literal literal)
{
  std::string text = source.substr(start, current - start);
  tokens.emplace_back(type, text, literal, line);
}

// basically a one character look-ahead
bool Scanner::match(char expected)
{
  if(isAtEnd())
    return false;
  if(source.at(current) != expected)
    return false;

  current++;
  return true;
}

char Scanner::peek()
{
  if(isAtEnd())
    return '\0';
  return source.at(current);
}

void Scanner::string()
{
  while(peek() != '\"' && !isAtEnd())
    {
      if(peek() == '\n')
        line++;
      advance();
    }

  if(isAtEnd())
    {
      executor.error(line, "Unterminated string.");
      return;
    }

  advance(); // The closing "

  // Trim the surrounding quotes
  std::string value = source.substr(start + 1, current - start - 1);
  addToken(TokenType::STRING, value);
}

void Scanner::number()
{
  while(std::isdigit(peek()))
    advance();

  // Looking for a fractional part
  if(peek() == '.' && std::isdigit(peekNext()))
    {
      // Consume the '.'
      advance();

      while(std::isdigit(peek()))
        advance();
    }

  addToken(TokenType::NUMBER,
           std::stod(source.substr(start, current - start)));
}

char Scanner::peekNext()
{
  if(current + 1 >= source.length())
    return '\0';
  else
    return source.at(current + 1);
}

void Scanner::identifier()
{
  while(isalnum(peek()))
    advance();

  TokenType type = TokenType::IDENTIFIER;
  std::string text = source.substr(start, current - start);
  if(keywords.contains(text))
    type = keywords.at(text);

  addToken(type);
}

bool Scanner::isalpha(char c)
{
  if(std::isalpha(c) || c == '_')
    return true;
  else
    return false;
}

bool Scanner::isalnum(char c)
{
  if(std::isalnum(c) || c == '_')
    return true;
  else
    return false;
}
