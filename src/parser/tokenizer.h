#include <iostream>
#include <set>
#include <string>
#include <vector>

enum class TokenType {
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

struct Token {
  TokenType type;
  std::string value;
  int line; // Line number for error reporting
};

class Tokenizer {
public:
  Tokenizer(const std::string &source) : source(source), position(0), line(1) {}

  std::vector<Token> tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
      token = nextToken();
      tokens.push_back(token);
    } while (token.type != TokenType::EndOfFile);
    return tokens;
  }

  Token nextToken() {
    skipWhitespaceAndComments();

    if (position >= source.length()) {
      return {TokenType::EndOfFile, "", line};
    }

    char currentChar = source[position];

    // Check for identifier or keyword
    if (isalpha(currentChar) || currentChar == '_') {
      return identifierOrKeyword();
    }

    // Check for numeric literal
    if (isdigit(currentChar)) {
      return numericLiteral();
    }

    // Check for string literal
    if (currentChar == '"') {
      return stringLiteral();
    }

    if (currentChar == '\'') {
      return characterLiteral();
    }

    // Check for operators and punctuators
    if (isOperatorOrPunctuatorStart(currentChar)) {
      return operatorOrPunctuator();
    }

    if (isUnknownCharacter(currentChar)) {
      std::cerr << "Error: Unknown character '" << currentChar << "' at line "
                << line << std::endl;
      return {TokenType::Unknown, std::string(1, currentChar), line};
    }

    return {TokenType::Unknown, std::string(1, currentChar), line};
  }

private:
  std::string source;
  size_t position;
  int line;

  bool isKeyword(const std::string &value) {
    static const std::set<std::string> keywords = {
        "bool",     "char",   "int",     "float", "double",    "void",
        "wchar_t",  "string", "Error",   "class", "interface", "function",
        "const",    "export", "extends", "async", "await",     "null",
        "true",     "false",  "try",     "catch", "import",    "from",
        "template", "copy",   "for",     "while", "if",        "else",
        "switch",   "case",   "default", "break", "continue",  "return",
        "try",      "catch",  "throw"
        // Add other keywords specific to your language
    };

    return keywords.find(value) != keywords.end();
  }

  Token characterLiteral() {
    position++; // Skip opening quote
    if (position >= source.length()) {
      std::cerr << "Error: Unterminated character literal at line " << line
                << std::endl;
      return {TokenType::Unknown, "", line};
    }

    char charValue = source[position++];

    // Handle escape sequences in char literals if needed
    if (charValue == '\\' && position < source.length()) {
      charValue = processEscapeSequence(source[position++]);
    }

    if (position >= source.length() || source[position] != '\'') {
      std::cerr << "Error: Unterminated character literal at line " << line
                << std::endl;
      return {TokenType::Unknown, "", line};
    }

    position++; // Skip closing quote

    // Convert charValue to a string
    return {TokenType::Character, std::string(1, charValue), line};
  }

  bool isOperatorOrPunctuatorStart(char ch) {
    return isOperator(ch) || isPunctuator(ch);
  }

  bool isOperator(char ch) {
    static const std::string opStartChars = "+-*/%=&|<>!.";
    return opStartChars.find(ch) != std::string::npos;
  }

  bool isUnknownCharacter(char ch) {
    static const std::string validChars =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        "+-*/%=&|<>!.,;()[]{}: \t\n" // Include ':' for generics syntax
        "'\"\\`"; // Include backtick (`) if template strings are supported

    return validChars.find(ch) == std::string::npos;
  }

  void skipWhitespaceAndComments() {
    bool inComment = false;
    while (position < source.length() &&
           (isspace(source[position]) || inComment)) {
      if (source[position] == '/' && position + 1 < source.length()) {
        if (source[position + 1] == '/') { // Single-line comment
          position += 2;
          while (position < source.length() && source[position] != '\n') {
            position++;
          }
        } else if (source[position + 1] == '*') { // Multi-line comment
          position += 2;
          inComment = true;
        }
      } else if (inComment && source[position] == '*' &&
                 position + 1 < source.length() &&
                 source[position + 1] == '/') {
        position += 2;
        inComment = false;
      }

      if (!inComment) {
        if (source[position] == '\n') {
          line++;
        }
        position++;
      }
    }
  }

  Token identifierOrKeyword() {
    size_t start = position++;
    while (position < source.length() &&
           (isalnum(source[position]) || source[position] == '_')) {
      position++;
    }

    std::string value = source.substr(start, position - start);

    if (isDeclarationKeyword(value)) {
      return {TokenType::Declaration, value, line};
    }

    if (isKeyword(value)) {
      return {TokenType::Keyword, value, line};
    }

    return {TokenType::Identifier, value, line};
  }

  bool isDeclarationKeyword(const std::string &value) {
    static const std::set<std::string> declarationKeywords = {
        "class", "function", "const", "interface",
        "async" // Add other declaration keywords
    };

    return declarationKeywords.find(value) != declarationKeywords.end();
  }

  Token numericLiteral() {
    size_t start = position++;
    bool hasDecimalPoint = false;

    while (position < source.length() &&
           (isdigit(source[position]) || source[position] == '.')) {
      if (source[position] == '.') {
        if (hasDecimalPoint)
          break; // Avoid multiple decimal points
        hasDecimalPoint = true;
      }
      position++;
    }

    return {TokenType::Number, source.substr(start, position - start), line};
  }

  Token stringLiteral() {
    char quoteType = source[position++];
    size_t start = position;
    std::string value;

    while (position < source.length() && source[position] != quoteType) {
      if (source[position] == '\\' && position + 1 < source.length()) {
        position++;
        value += processEscapeSequence(source[position]);
      } else {
        value += source[position];
      }
      position++;
    }

    if (position == source.length()) {
      std::cerr << "Error: Unterminated string literal at line " << line
                << std::endl;
      return {TokenType::Unknown, "",
              line}; // Or handle this error as appropriate
    }

    position++; // Skip the closing quote
    return {TokenType::String, value, line};
  }

  char processEscapeSequence(char escapedChar) {
    switch (escapedChar) {
    case 'n':
      return '\n';
    case 't':
      return '\t';
    case '\\':
      return '\\';
    case '"':
      return '\"';
    case '\'':
      return '\'';
    // Add other cases as needed
    default:
      return escapedChar;
    }
  }

  Token operatorOrPunctuator() {
    char currentChar = source[position];
    size_t start = position++;

    if (isPunctuator(currentChar)) {
      return {TokenType::Punctuator, std::string(1, currentChar), line};
    } else {
      // Handle multi-character operators
      if (position < source.length()) {
        std::string potentialOperator = source.substr(start, 2);
        if (isMultiCharacterOperator(potentialOperator)) {
          position++;
          return {TokenType::Operator, potentialOperator, line};
        }
      }
      return {TokenType::Operator, std::string(1, currentChar), line};
    }
  }

  bool isPunctuator(char ch) {
    static const std::string punctuators = ";,(){}[]<>"; // Added '<' and '>'
    return punctuators.find(ch) != std::string::npos;
  }

  bool isMultiCharacterOperator(const std::string &op) {
    static const std::set<std::string> multiCharOperators = {
        "&&", "||", "==", "!=", "<=", ">=", "+=", "-=", "*=", "/="};
    return multiCharOperators.find(op) != multiCharOperators.end();
  }
};
