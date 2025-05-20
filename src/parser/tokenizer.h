#pragma once

#include <iostream>
#include <set>
#include <string>
#include <vector>

enum class TokenType
{
  Identifier,
  Keyword,
  Number,
  String,
  Operator,
  Punctuator,
  EndOfFile,
  Unknown,
  Declaration,
  Character
};

struct Token
{
  TokenType type;
  std::string value;
  int line; // Line number for error reporting
};

class Tokenizer
{
public:
  Tokenizer(const std::string &source) : source(source), position(0), line(1) {}

  std::vector<Token> tokenize();
  Token nextToken();

private:
  std::string source;
  size_t position;
  int line;

  bool isKeyword(const std::string &value);
  bool isOperatorOrPunctuatorStart(char ch);
  bool isOperator(char ch);
  bool isUnknownCharacter(char ch);
  void skipWhitespaceAndComments();
  Token identifierOrKeyword();
  bool isDeclarationKeyword(const std::string &value);
  Token numericLiteral();
  Token stringLiteral();
  char processEscapeSequence(char escapedChar);
  Token characterLiteral();
  Token operatorOrPunctuator();
  bool isPunctuator(char ch);
  bool isMultiCharacterOperator(const std::string &op);
};
