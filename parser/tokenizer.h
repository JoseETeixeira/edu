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
  Char
};

struct Token {
  TokenType type;
  std::string value;
  int line; // Line number for error reporting
};

class Tokenizer {
public:
  Tokenizer(const std::string &source) : source(source), position(0), line(1) {}

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
    if (currentChar == '"' || currentChar == '\'') {
      return stringLiteral();
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
    TokenType type =
        isKeyword(value) ? TokenType::Keyword : TokenType::Identifier;

    return {type, value, line};
  }

  bool isKeyword(const std::string &value) {
    // Define all language's keywords
    static const std::set<std::string> keywords = {
        "class",  "interface", "function", "let",      "const", "if",
        "else",   "for",       "while",    "return",   "break", "continue",
        "switch", "case",      "default",  "try",      "catch", "throw",
        "import", "from",      "export",   "extends",  "async", "await",
        "null",   "true",      "false",    "template", "copy"};

    return keywords.find(value) != keywords.end();
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

  std::string processEscapeSequence(char escapedChar) {
    switch (escapedChar) {
    case 'n':
      return "\n";
    case 't':
      return "\t";
    case '\\':
      return "\\";
    case '"':
      return "\"";
    case '\'':
      return "\'";
    // You can add more cases here based on the escape sequences your language
    // supports.
    default:
      return std::string(1, escapedChar);
    }
  }

  bool isOperatorOrPunctuatorStart(char ch) {
    static const std::string opStartChars = "+-*/%=&|<>!.,;()[]{}";
    return opStartChars.find(ch) != std::string::npos;
  }

  Token operatorOrPunctuator() {
    size_t start = position++;
    // Extend this to handle two or more character operators
    if (position < source.length()) {
      std::string potentialOperator = source.substr(start, 2);
      if (isMultiCharacterOperator(potentialOperator)) {
        position++;
        return {TokenType::Operator, potentialOperator, line};
      }
    }

    return {TokenType::Operator, source.substr(start, 1), line};
  }

  bool isMultiCharacterOperator(const std::string &op) {
    static const std::set<std::string> multiCharOperators = {
        "&&", "||", "==", "!=", "<=", ">=", "+=", "-=", "*=", "/="};
    return multiCharOperators.find(op) != multiCharOperators.end();
  }

  bool isUnknownCharacter(char ch) {
    // Include all valid characters for identifiers, numbers, operators, string
    // and template literals, and whitespace
    static const std::string validChars =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        "+-*/%=&|<>!.,;()[]{} \t\n" // operators and punctuators
        "'\"\\`"; // characters for string and template literals including
                  // escape character and backticks

    return validChars.find(ch) == std::string::npos;
  }

  Token characterLiteral() {
    position++; // Skip opening quote
    char charValue = source[position++];
    position++; // Skip closing quote

    return {TokenType::Char, std::string(1, charValue), line};
  }
};
