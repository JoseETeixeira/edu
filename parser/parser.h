#include "nodes.h"
#include "tokenizer.h"
#include <memory>
#include <stdexcept>

class Parser {
public:
  Parser(Tokenizer &tokenizer) : tokenizer(tokenizer) {
    // Initialize by reading the first token
    currentToken = tokenizer.nextToken();
  }

  std::unique_ptr<ProgramNode> parseProgram() {
    auto program = std::make_unique<ProgramNode>(currentToken.line);
    while (currentToken.type != TokenType::EndOfFile) {
      switch (currentToken.type) {
      case TokenType::Keyword:
        if (currentToken.value == "class") {
          program->children.push_back(parseClassDeclaration());
        } else if (currentToken.value == "function") {
          program->children.push_back(parseFunctionDeclaration());
        } else if (currentToken.value == "import") {
          program->children.push_back(parseImportStatement());
        } else if (currentToken.value == "interface") {
          program->children.push_back(parseInterfaceDeclaration());
        }
        // ... other keyword cases ...
        break;
      // ... other token type cases ...
      default:
        throw std::runtime_error("Syntax error at line " +
                                 std::to_string(currentToken.line));
      }

      advanceToken();
    }
    return program;
  }

private:
  Tokenizer &tokenizer;
  Token currentToken; // Current token being processed
                      // private methods.

  void advanceToken() { currentToken = tokenizer.nextToken(); }

  std::unique_ptr<ClassNode> parseClassDeclaration() {
    // Validate and consume 'class' keyword
    expectToken(TokenType::Keyword);
    advanceToken();

    // Validate and consume class name (identifier)
    expectToken(TokenType::Identifier);
    std::string className = currentToken.value;
    advanceToken();

    // Parse class members, etc.
    // ...

    return std::make_unique<ClassNode>(className, currentToken.line);
  }

  std::unique_ptr<FunctionNode> parseFunctionDeclaration() {
    // Validate and consume 'function' keyword
    expectToken(TokenType::Keyword);
    advanceToken();

    // Validate and consume function name (identifier)
    expectToken(TokenType::Identifier);
    std::string functionName = currentToken.value;
    advanceToken();

    // Parse function parameters, body, etc.
    // ...

    return std::make_unique<FunctionNode>(functionName, currentToken.line);
  }

  void expectToken(TokenType expectedType) {
    if (currentToken.type != expectedType) {
      throw std::runtime_error("Expected different token type at line " +
                               std::to_string(currentToken.line));
    }
  }

  std::unique_ptr<ImportNode> parseImportStatement() {
    expectToken(TokenType::Keyword); // Expecting 'import'
    advanceToken();

    expectToken(TokenType::Identifier);
    std::string moduleName = currentToken.value;
    advanceToken();

    expectToken(TokenType::Keyword); // Expecting 'from'
    advanceToken();

    expectToken(TokenType::String);
    std::string path = currentToken.value;
    advanceToken();

    expectToken(TokenType::Punctuator); // Expecting ';'
    advanceToken();

    auto importNode = std::make_unique<ImportNode>(currentToken.line);
    importNode->moduleName = moduleName;
    // Set other properties of importNode as necessary

    return importNode;
  }

  std::unique_ptr<InterfaceNode> parseInterfaceDeclaration() {
    expectToken(TokenType::Keyword); // Expecting 'interface'
    advanceToken();

    expectToken(TokenType::Identifier);
    std::string interfaceName = currentToken.value;
    advanceToken();

    expectToken(TokenType::Punctuator); // Expecting '{'
    advanceToken();

    auto interfaceNode =
        std::make_unique<InterfaceNode>(interfaceName, currentToken.line);

    while (currentToken.type != TokenType::Punctuator ||
           currentToken.value != "}") {
      // Parse interface members
      // This will depend on your language's rules for what constitutes an
      // interface member
      advanceToken();
    }

    expectToken(TokenType::Punctuator); // Expecting '}'
    advanceToken();

    return interfaceNode;
  }
};
