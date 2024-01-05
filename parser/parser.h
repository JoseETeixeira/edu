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
        } else if (currentToken.value == "if") {
          program->children.push_back(parseIfStatement());
        } else if (currentToken.value == "for") {
          program->children.push_back(parseForStatement());
        } else if (currentToken.value == "while") {
          program->children.push_back(parseWhileStatement());
        }

        // ... other keyword cases ...
        break;
      case TokenType::Identifier:
        program->children.push_back(parseIdentifierExpression());
        break;
      case TokenType::Number:
        program->children.push_back(parseNumberLiteral());
        break;
      case TokenType::String:
        program->children.push_back(parseStringLiteral());
        break;
      case TokenType::Operator:
        program->children.push_back(parseOperatorExpression());
        break;
      case TokenType::Punctuator:
        // Specific handling based on the punctuator
        break;

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

  std::unique_ptr<StatementNode> parseStatement() {
    switch (currentToken.type) {
    case TokenType::Keyword:
      if (currentToken.value == "let") {
        return parseVariableDeclaration();
      }
      // Handle other keywords like 'if', 'while', 'for', etc.
      // ...
      break;
      // Handle other types of statements
      // ...
    }
    throw std::runtime_error("Unexpected token in statement at line " +
                             std::to_string(currentToken.line));
  }

  std::unique_ptr<VariableDeclarationNode> parseVariableDeclaration() {
    // Assuming 'let' starts a variable declaration
    expectToken(TokenType::Keyword); // 'let'
    advanceToken();

    expectToken(TokenType::Identifier);
    std::string varName = currentToken.value;
    advanceToken();

    // Optionally handle type annotations and initializers
    // ...

    return std::make_unique<VariableDeclarationNode>(varName,
                                                     currentToken.line);
  }

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

  std::unique_ptr<IfStatementNode> parseIfStatement() {
    expectToken(TokenType::Keyword); // Expecting 'if'
    int line = currentToken.line;
    advanceToken();

    // Parse the condition
    expectToken(TokenType::Punctuator); // Expecting '(' for the condition
    advanceToken();

    auto condition = parseExpression(); // Implement parseExpression separately

    expectToken(TokenType::Punctuator); // Expecting ')' after the condition
    advanceToken();

    // Parse the 'then' branch, which is typically a block statement
    auto thenBranch = parseStatement(); // Implement parseStatement separately

    std::unique_ptr<StatementNode> elseBranch = nullptr;
    if (currentToken.type == TokenType::Keyword &&
        currentToken.value == "else") {
      advanceToken();
      elseBranch = parseStatement(); // Could be another if or a block statement
    }

    auto ifNode = std::make_unique<IfStatementNode>(line);
    ifNode->condition = std::move(condition);
    ifNode->thenBranch = std::move(thenBranch);
    ifNode->elseBranch = std::move(elseBranch);

    return ifNode;
  }

  std::unique_ptr<ExpressionNode> parseExpression(int precedence = 0) {
    std::unique_ptr<ExpressionNode> left;

    // Handle different starting types of expressions
    switch (currentToken.type) {
    case TokenType::Number:
      left = std::make_unique<NumberLiteralNode>(currentToken.value,
                                                 currentToken.line);
      advanceToken();
      break;
      // Handle other literal types and unary operators
      // ...
    }

    // Now handle binary operations and respect precedence
    while (precedence < getPrecedence(currentToken.type)) {
      Token op = currentToken;
      advanceToken();

      auto right = parseExpression(getPrecedence(op.type));
      left = std::make_unique<BinaryExpressionNode>(op.value, left->getLine());
      static_cast<BinaryExpressionNode *>(left.get())->left = std::move(left);
      static_cast<BinaryExpressionNode *>(left.get())->right = std::move(right);
    }

    return left;
  }

  int getPrecedence(TokenType tokenType) {
    // Define precedence levels for different operators
    switch (tokenType) {
    case TokenType::Operator:
      // Define precedence based on operator
      // For example, '*' and '/' could have higher precedence than '+' and '-'
      return 10;
    // Define other precedence levels
    default:
      return 0;
    }
  }

  std::unique_ptr<WhileStatementNode> parseWhileStatement() {
    expectToken(TokenType::Keyword); // Expecting 'while'
    int line = currentToken.line;
    advanceToken();

    expectToken(TokenType::Punctuator); // Expecting '('
    advanceToken();

    auto condition = parseExpression();

    expectToken(TokenType::Punctuator); // Expecting ')'
    advanceToken();

    // Parse the loop body
    auto body = parseStatement();

    auto whileNode = std::make_unique<WhileStatementNode>(line);
    whileNode->condition = std::move(condition);
    whileNode->body = std::move(body);

    return whileNode;
  }

  std::unique_ptr<ForStatementNode> parseForStatement() {
    expectToken(TokenType::Keyword); // Expecting 'for'
    int line = currentToken.line;
    advanceToken();

    expectToken(TokenType::Punctuator); // Expecting '('
    advanceToken();

    // Parse initializer, condition, and increment, each separated by ';'
    auto initializer = parseStatement(); // This could be a variable declaration
                                         // or an expression
    auto condition = parseExpression();
    expectToken(TokenType::Punctuator); // Expecting ';'
    advanceToken();
    auto increment = parseExpression();

    expectToken(TokenType::Punctuator); // Expecting ')'
    advanceToken();

    // Parse the loop body
    auto body = parseStatement();

    auto forNode = std::make_unique<ForStatementNode>(line);
    forNode->initializer = std::move(initializer);
    forNode->condition = std::move(condition);
    forNode->increment = std::move(increment);
    forNode->body = std::move(body);

    return forNode;
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
