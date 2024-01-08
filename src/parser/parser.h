#include "nodes.h"
#include "tokenizer.h"
#include <memory>
#include <stdexcept>
#include <vector>

class Parser {
public:
  explicit Parser(const std::vector<Token> &tokens)
      : tokens(tokens), current(0) {}

  std::unique_ptr<ProgramNode> parse();

private:
  const std::vector<Token> &tokens;
  int current;

  // Utility methods (match, consume, check, advance, peek, previous, isAtEnd,
  // error)
  const std::vector<Token> &tokens;
  int current;

  // Utility methods
  bool match(TokenType type, const std::string &expectedValue);
  Token consume(TokenType type, const std::string &expectedValue,
                const std::string &errorMessage);
  bool check(TokenType type, const std::string &expectedValue) const;
  Token advance();
  Token peek() const;
  Token previous() const;
  bool isAtEnd() const;
  void error(const std::string &message);
  bool isType(const std::string &keyword);

  // Main Parsing Methods
  std::unique_ptr<ASTNode> parseDeclaration();
  std::unique_ptr<ASTNode> parseStatement();
  std::unique_ptr<ASTNode> parseExpression();

  // Declaration Parsing
  std::unique_ptr<ClassNode> parseClassDeclaration();
  std::unique_ptr<FunctionNode> parseFunctionDeclaration();
  std::unique_ptr<VariableDeclarationNode> parseVariableDeclaration();
  std::unique_ptr<InterfaceNode> parseInterfaceDeclaration();

  // Statement Parsing
  std::unique_ptr<ASTNode> parseClassMember();
  std::unique_ptr<ASTNode> parsePropertyDeclaration();
  std::unique_ptr<ASTNode> parseConstructorDeclaration();
  std::unique_ptr<ASTNode> parseInterfaceMember();
  std::unique_ptr<NullReferenceNode> parseNullReference();
  std::unique_ptr<ConsoleLogNode> parseConsoleLog();
  std::unique_ptr<InputStatementNode> parseInputStatement(); // FINISHED HERE
  std::unique_ptr<BlockStatementNode> parseBlockStatement();
  std::unique_ptr<IfStatementNode> parseIfStatement();
  std::unique_ptr<ForStatementNode> parseForStatement();
  std::unique_ptr<WhileStatementNode> parseWhileStatement();
  std::unique_ptr<ReturnStatementNode> parseReturnStatement();
  std::unique_ptr<BreakStatementNode> parseBreakStatement();
  std::unique_ptr<ContinueStatementNode> parseContinueStatement();
  std::unique_ptr<SwitchStatementNode> parseSwitchStatement();
  std::unique_ptr<TryCatchNode> parseTryCatchStatement();
  std::unique_ptr<ExportNode> parseExportStatement();
  std::unique_ptr<ImportNode> parseImportStatement();

  // Expression Parsing
  std::unique_ptr<ExpressionNode> parseAssignmentExpression();
  std::unique_ptr<ExpressionNode> parseOrExpression();
  std::unique_ptr<ExpressionNode> parseAndExpression();
  std::unique_ptr<ExpressionNode> parseEqualityExpression();
  std::unique_ptr<ExpressionNode> parseComparisonExpression();
  std::unique_ptr<ExpressionNode> parseAdditionExpression();
  std::unique_ptr<ExpressionNode> parseMultiplicationExpression();
  std::unique_ptr<ExpressionNode> parseUnaryExpression();
  std::unique_ptr<ExpressionNode> parsePrimaryExpression();

  // Primary Expressions
  std::unique_ptr<LiteralNode> parseLiteral();
  std::unique_ptr<ArrayLiteralNode> parseArrayLiteral();
  std::unique_ptr<ObjectLiteralNode> parseObjectLiteral();
  std::unique_ptr<FunctionNode> parseAnonymousFunction();
  std::unique_ptr<CallExpressionNode> parseCallExpression();
  std::unique_ptr<MemberAccessExpressionNode> parseMemberAccessExpression();

  // Utility Parsing Methods
  std::unique_ptr<TypeNode> parseType();
  std::unique_ptr<FunctionParameterNode> parseFunctionParameter();
  std::vector<std::unique_ptr<ExpressionNode>> parseArguments();
  std::vector<std::unique_ptr<FunctionParameterNode>> parseParameters();
  std::vector<std::unique_ptr<CaseClauseNode>> parseCaseClauses();

  std::unique_ptr<AwaitExpressionNode> Parser::parseAwaitExpression();
  std::unique_ptr<CaseClauseNode> Parser::parseCaseClause();
};

bool Parser::match(TokenType type, const std::string &expectedValue) {
  if (check(type, expectedValue)) {
    advance();
    return true;
  }
  return false;
}

Token Parser::consume(TokenType type, const std::string &expectedValue,
                      const std::string &errorMessage) {
  if (check(type, expectedValue)) {
    return advance();
  } else {
    throw std::runtime_error(errorMessage);
  }
}

bool Parser::check(TokenType type, const std::string &expectedValue) const {
  if (isAtEnd())
    return false;
  if (expectedValue != "") {
    return peek().type == type && peek().value == expectedValue;
  } else {
    return peek().type == type;
  }
}

Token Parser::advance() {
  if (!isAtEnd())
    current++;
  return previous();
}

Token Parser::peek() const { return tokens[current]; }

Token Parser::previous() const { return tokens[current - 1]; }

bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }

void Parser::error(const std::string &message) {
  // Error handling logic
  throw std::runtime_error(message);
}

// Main Parsing Methods

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
  if (match(TokenType::Keyword, "class")) {
    return parseClassDeclaration();
  } else if (match(TokenType::Keyword, "function")) {
    return parseFunctionDeclaration();
  } else if (match(TokenType::Keyword, "async")) {
    // Handle async functions
    return parseFunctionDeclaration();
  } else if (match(TokenType::Keyword, "const")) {
    return parseVariableDeclaration();
  } else if (match(TokenType::Keyword, "interface")) {
    return parseInterfaceDeclaration();
  }
  return parseStatement();
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
  if (match(TokenType::Punctuator, "{")) {
    return parseBlockStatement();
  }
  if (match(TokenType::Keyword, "if")) {
    return parseIfStatement();
  } else if (match(TokenType::Keyword, "for")) {
    return parseForStatement();
  } else if (match(TokenType::Keyword, "while")) {
    return parseWhileStatement();
  } else if (match(TokenType::Keyword, "return")) {
    return parseReturnStatement();
  } else if (match(TokenType::Keyword, "break")) {
    return parseBreakStatement();
  } else if (match(TokenType::Keyword, "continue")) {
    return parseContinueStatement();
  } else if (match(TokenType::Keyword, "switch")) {
    return parseSwitchStatement();
  } else if (match(TokenType::Keyword, "try")) {
    return parseTryCatchStatement();
  } else if (match(TokenType::Keyword, "export")) {
    return parseExportStatement();
  } else if (match(TokenType::Keyword, "import")) {
    return parseImportStatement();
  } else if (match(TokenType::Keyword, "null")) {
    return parseNullReference();
  } else if (match(TokenType::Keyword, "console.log")) {
    return parseConsoleLog();
  } else if (match(TokenType::Keyword, "input")) {
    return parseInputStatement();
  }
  // If none of the above keywords match, default to parsing an expression
  return parseExpression();
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
  return parseAssignmentExpression();
}

std::unique_ptr<ExpressionNode> Parser::parseAssignmentExpression() {
  auto left = parseOrExpression(); // Start with the next level of precedence,
                                   // which might be logical OR.

  if (match(TokenType::Operator, "=") || match(TokenType::Operator, "+=") ||
      match(TokenType::Operator,
            "-=") /* ... other assignment operators ... */) {
    std::string operatorValue = previous().value;
    auto right =
        parseAssignmentExpression(); // Recursively parse the right-hand side

    // Ensure the left-hand side is a valid lvalue (this is language specific
    // and may involve more checks)
    if (dynamic_cast<VariableExpressionNode *>(left.get()) ||
        dynamic_cast<MemberAccessExpressionNode *>(left.get())) {
      return std::make_unique<AssignmentExpressionNode>(
          std::move(left), operatorValue, std::move(right));
    } else {
      error("Invalid left-hand side in assignment");
    }
  }

  return left; // If no assignment operator is found, just return the left-hand
               // expression
}

std::unique_ptr<ExpressionNode> Parser::parseOrExpression() {
  auto left = parseAndExpression(); // Start with the next level of precedence,
                                    // which might be logical AND.

  while (match(TokenType::Operator, "||")) {
    std::string operatorValue = previous().value;
    auto right = parseAndExpression(); // Recursively parse the right operand.
    left = std::make_unique<OrExpressionNode>(std::move(left), operatorValue,
                                              std::move(right));
  }

  return left; // If no OR operator is found, just return the left operand
               // expression.
}

std::unique_ptr<ExpressionNode> Parser::parseAndExpression() {
  auto left =
      parseEqualityExpression(); // Start with the next level of precedence,
                                 // which might be equality expressions.

  while (match(TokenType::Operator, "&&")) {
    std::string operatorValue = previous().value;
    auto right =
        parseEqualityExpression(); // Recursively parse the right operand.
    left = std::make_unique<AndExpressionNode>(std::move(left), operatorValue,
                                               std::move(right));
  }

  return left; // If no AND operator is found, just return the left operand
               // expression.
}

std::unique_ptr<ExpressionNode> Parser::parseEqualityExpression() {
  auto left =
      parseComparisonExpression(); // Start with the next level of precedence.

  while (match(TokenType::Operator, "==") || match(TokenType::Operator, "!=")) {
    std::string operatorValue = previous().value;
    auto right =
        parseComparisonExpression(); // Recursively parse the right operand.
    left = std::make_unique<EqualityExpressionNode>(
        std::move(left), operatorValue, std::move(right));
  }

  return left; // If no equality operator is found, just return the left operand
               // expression.
}
std::unique_ptr<ExpressionNode> Parser::parseComparisonExpression() {
  auto left =
      parseAdditionExpression(); // Start with the next level of precedence.

  while (match(TokenType::Operator, "<") || match(TokenType::Operator, ">") ||
         match(TokenType::Operator, "<=") || match(TokenType::Operator, ">=")) {
    std::string operatorValue = previous().value;
    auto right =
        parseAdditionExpression(); // Recursively parse the right operand.
    left = std::make_unique<ComparisonExpressionNode>(
        std::move(left), operatorValue, std::move(right));
  }

  return left; // If no comparison operator is found, just return the left
               // operand expression.
}

std::unique_ptr<ExpressionNode> Parser::parseAdditionExpression() {
  auto left = parseMultiplicationExpression(); // Start with the next level of
                                               // precedence.

  while (true) {
    if (match(TokenType::Operator, "+")) {
      auto right = parseMultiplicationExpression(); // Parse the right operand.
      left = std::make_unique<AdditionExpressionNode>(std::move(left), "+",
                                                      std::move(right));
    } else if (match(TokenType::Operator, "-")) {
      auto right = parseMultiplicationExpression(); // Parse the right operand.
      left = std::make_unique<SubtractionExpressionNode>(std::move(left), "-",
                                                         std::move(right));
    } else {
      break; // No more addition or subtraction operators.
    }
  }

  return left; // Return the built expression node.
}

std::unique_ptr<ExpressionNode> Parser::parseMultiplicationExpression() {
  auto left =
      parseUnaryExpression(); // Start with the next level of precedence.

  while (true) {
    if (match(TokenType::Operator, "*")) {
      auto right = parseUnaryExpression(); // Parse the right operand.
      left = std::make_unique<MultiplicationExpressionNode>(
          std::move(left), "*", std::move(right));
    } else if (match(TokenType::Operator, "/")) {
      auto right = parseUnaryExpression(); // Parse the right operand.
      left = std::make_unique<DivisionExpressionNode>(std::move(left), "/",
                                                      std::move(right));
    } else {
      break; // No more multiplication or division operators.
    }
  }

  return left; // Return the built expression node.
}

std::unique_ptr<ExpressionNode> Parser::parseUnaryExpression() {
  // Check for unary operators
  if (match(TokenType::Operator, "-") || match(TokenType::Operator, "!")) {
    std::string operatorValue = previous().value; // Get the unary operator
    auto operand = parseUnaryExpression(); // Recursively parse the operand
    return std::make_unique<UnaryExpressionNode>(operatorValue,
                                                 std::move(operand));
  }

  // If no unary operator is found, parse the primary expression
  return parsePrimaryExpression();
}

std::unique_ptr<ExpressionNode> Parser::parsePrimaryExpression() {
  if (match(TokenType::Number, "")) {
    return std::make_unique<NumberLiteralNode>(previous().value);
  } else if (match(TokenType::String, "")) {
    return std::make_unique<StringLiteralNode>(previous().value);
  } else if (match(TokenType::Identifier, "")) {
    return std::make_unique<VariableExpressionNode>(previous().value);
  } else if (match(TokenType::Char, "")) {
    return std::make_unique<CharLiteralNode>(previous().value);
  } else if (match(TokenType::Punctuator, "(")) {
    auto expr = parseExpression(); // Returns std::unique_ptr<ASTNode>
    consume(TokenType::Punctuator, ")",
            "Expected ) at the end of the expression");

    // Downcast from ASTNode to ExpressionNode
    return std::unique_ptr<ExpressionNode>(
        static_cast<ExpressionNode *>(expr.release()));
  } else {
    throw std::runtime_error("Unexpected token in primary expression");
  }
}

std::unique_ptr<NullReferenceNode> Parser::parseNullReference() {
  // Ensure that the current token is 'null'
  if (!match(TokenType::Keyword, "null")) {
    error("Expected 'null'");
  }

  // Create a NullReferenceNode
  auto nullNode = std::make_unique<NullReferenceNode>(previous().line);

  // Return the created null reference node
  return nullNode;
}

bool Parser::isType(const std::string &keyword) {
  // Check if the keyword is a valid type
  static const std::set<std::string> validTypes = {
      "bool", "char",    "int",    "float", "double",
      "void", "wchar_t", "string", "Error"};
  return validTypes.find(keyword) != validTypes.end();
}

std::unique_ptr<ConsoleLogNode> Parser::parseConsoleLog() {
  // Ensure the current token is the 'console.log' keyword
  consume(TokenType::Keyword, "console.log", "Expected 'console.log'");

  // Parse the expression to be logged
  auto expression = parseExpression();

  // Consume the semicolon at the end of the console.log statement
  consume(TokenType::Punctuator, ";",
          "Expected ';' after console.log statement");

  // Create and return a new ConsoleLogNode
  return std::make_unique<ConsoleLogNode>(std::move(expression));
}

std::unique_ptr<InputStatementNode> Parser::parseInputStatement() {
  // Ensure the current token is the 'input' keyword
  consume(TokenType::Keyword, "input", "Expected 'input'");

  // Optionally, parse a variable declaration if it's part of the input syntax
  std::unique_ptr<VariableDeclarationNode> variable;
  if (match(TokenType::Identifier, "input")) {
    std::string variableName = previous().value;
    consume(TokenType::Operator, "=",
            "Expected '='"); // Assuming the syntax is 'input variableName =
                             // input();'

    // Parse the input function call (assuming it's represented as a call)
    consume(TokenType::Keyword, "input",
            "Expected 'input'"); // Consuming the actual input keyword again if
                                 // the syntax is like 'input()'
    consume(TokenType::Punctuator, "(", "Expected '('");
    consume(TokenType::Punctuator, ")", "Expected ')'");

    variable = std::make_unique<VariableDeclarationNode>(variableName);
  }

  // Consume the semicolon at the end of the input statement
  consume(TokenType::Punctuator, ";", "Expected ';' after input statement");

  // Create and return a new InputStatementNode
  return std::make_unique<InputStatementNode>(std::move(variable));
}
std::unique_ptr<ClassNode> Parser::parseClassDeclaration() {
  // Ensure we are starting with the 'class' keyword
  consume(TokenType::Keyword, "class", "Expected 'class' keyword");

  // Get the class name token
  Token classNameToken =
      consume(TokenType::Identifier, "", "Expected class name");

  // Extract the class name from the token
  std::string className = classNameToken.value;

  // Create a new ClassNode with the parsed name
  auto classNode = std::make_unique<ClassNode>(className, classNameToken.line);

  // Consume the opening brace '{' of the class body
  consume(TokenType::Punctuator, "{", "Expected '{' after class name");

  // Parse class members until the closing brace '}'
  while (!check(TokenType::Punctuator, "}") && !isAtEnd()) {
    classNode->members.push_back(parseClassMember());
  }

  // Consume the closing brace '}'
  consume(TokenType::Punctuator, "}", "Expected '}' at the end of class body");

  return classNode;
}

std::unique_ptr<ASTNode> Parser::parseClassMember() {
  // Check if the member is a method
  if (match(TokenType::Keyword, "function") ||
      match(TokenType::Keyword, "async")) {
    return parseFunctionDeclaration();
  }
  // Check if the member is a property
  else if (match(TokenType::Keyword, "") && isType(previous().value)) {
    return parsePropertyDeclaration();
  }
  // Handle other types of class members
  // Example: parsing a constructor
  else if (match(TokenType::Keyword, "constructor")) {
    return parseConstructorDeclaration();
  }

  throw std::runtime_error("Unsupported class member type");
}

std::unique_ptr<ASTNode> Parser::parsePropertyDeclaration() {
  // Assuming properties are declared like variables
  std::string propertyName =
      consume(TokenType::Identifier, "", "Expected property name").value;

  // Optional: Parse property type if it follows the property name
  std::unique_ptr<TypeNode> propertyType;
  if (peek().type == TokenType::Identifier) {
    // Assuming next token is type if it's an identifier
    propertyType = parseType();
  }

  // Check for '=' and parse the initializer expression if present
  std::unique_ptr<ExpressionNode> initializer;
  if (match(TokenType::Operator, "=")) {
    auto expr = parseExpression(); // Returns std::unique_ptr<ASTNode>
    initializer = std::unique_ptr<ExpressionNode>(
        dynamic_cast<ExpressionNode *>(expr.release()));
    if (!initializer) {
      throw std::runtime_error("Expected expression for initializer");
    }
  }

  // Consume the semicolon at the end of the property declaration
  consume(TokenType::Punctuator, ";",
          "Expected ';' after property declaration");

  // Create and return a property declaration node
  return std::make_unique<PropertyDeclarationNode>(
      propertyName, std::move(propertyType), std::move(initializer),
      previous().line);
}

std::unique_ptr<ASTNode> Parser::parseConstructorDeclaration() {
  // Assuming a constructor is like a function but without a return type
  consume(TokenType::Punctuator, "(", "Expected '(' after 'constructor'");
  auto parameters = parseParameters();
  consume(TokenType::Punctuator, ")",
          "Expected ')' after constructor parameters");

  auto body = parseBlockStatement(); // Parse the constructor's body

  return std::make_unique<ConstructorNode>(std::move(parameters),
                                           std::move(body));
}

std::unique_ptr<FunctionNode> Parser::parseFunctionDeclaration() {
  // Check if the function is asynchronous
  bool isAsync = match(TokenType::Keyword, "async");

  // Consume 'function' keyword
  consume(TokenType::Keyword, "function", "Expected 'function' keyword");

  // Get the function name
  std::string functionName =
      consume(TokenType::Identifier, "Expected function name", "").value;

  // Consume the opening parenthesis '(' of the parameter list
  consume(TokenType::Punctuator, "(", "Expected '(' after function name");

  // Parse function parameters
  std::vector<std::unique_ptr<FunctionParameterNode>> parameters;
  if (!check(TokenType::Punctuator, ")")) {
    do {
      std::string paramName =
          consume(TokenType::Identifier, "Expected parameter name", "").value;
      std::unique_ptr<TypeNode> paramType;
      if (peek().type == TokenType::Identifier) {
        // Parse parameter type if present
        paramType = parseType();
      }
      parameters.push_back(std::make_unique<FunctionParameterNode>(
          paramName, std::move(paramType)));
    } while (match(TokenType::Punctuator, ","));
  }

  // Consume the closing parenthesis ')' of the parameter list
  consume(TokenType::Punctuator, ")", "Expected ')' after parameters");

  // Parse the function body
  auto body = parseBlockStatement();

  // Create and return a new FunctionNode
  return std::make_unique<FunctionNode>(functionName, std::move(parameters),
                                        std::move(body), isAsync);
}

std::unique_ptr<VariableDeclarationNode> Parser::parseVariableDeclaration() {
  // Optional: Parse the type if present
  std::unique_ptr<TypeNode> type;
  if (isType(peek().value)) {
    type = parseType();
  }

  // Get the variable name
  std::string variableName =
      consume(TokenType::Identifier, "", "Expected variable name").value;

  // Optional initializer
  std::unique_ptr<ExpressionNode> initializer;
  if (match(TokenType::Operator, "=")) {
    auto expr = parseExpression();
    initializer = std::unique_ptr<ExpressionNode>(
        dynamic_cast<ExpressionNode *>(expr.release()));
    if (!initializer) {
      error("Expected expression for initializer");
    }
  }

  // Consume the semicolon at the end of the variable declaration
  consume(TokenType::Punctuator, ";",
          "Expected ';' after variable declaration");

  // Create and return a new VariableDeclarationNode
  return std::make_unique<VariableDeclarationNode>(
      variableName, std::move(type), std::move(initializer));
}

std::unique_ptr<InterfaceNode> Parser::parseInterfaceDeclaration() {
  // Ensure we are starting with 'interface' keyword
  consume(TokenType::Keyword, "interface", "Expected 'interface'");

  // Get the interface name
  std::string interfaceName =
      consume(TokenType::Identifier, "Expected interface name", "").value;

  // Create a new InterfaceNode with the parsed name
  auto interfaceNode = std::make_unique<InterfaceNode>(interfaceName);

  // Consume the opening brace '{' of the interface body
  consume(TokenType::Punctuator, "{", "Expected '{' after interface name");

  // Parse interface members until closing brace '}'
  while (!check(TokenType::Punctuator, "}") && !isAtEnd()) {
    if (peek().type == TokenType::Identifier) {
      // Parse a property or method declaration
      interfaceNode->members.push_back(parseInterfaceMember());
    } else {
      error("Expected interface member declaration");
    }
  }

  // Consume the closing brace '}'
  consume(TokenType::Punctuator, "}", "Expected '}' at end of interface body");

  return interfaceNode;
}

std::unique_ptr<ASTNode> Parser::parseInterfaceMember() {
  // Check if the member is a method
  if (match(TokenType::Keyword, "function")) {
    return parseFunctionDeclaration();
  }
  // Handle other types of interface members (e.g., properties)
  else {

    std::string propertyName =
        consume(TokenType::Identifier, "Expected property name", "").value;

    // Optional: Parse property type if it follows the property name
    std::unique_ptr<TypeNode> propertyType;
    if (peek().type == TokenType::Identifier) {
      // Assuming next token is type if it's an identifier
      propertyType = parseType();
    }

    // Consume the semicolon at the end of the property declaration
    consume(TokenType::Punctuator, ";",
            "Expected ';' after property declaration");

    // Create and return a property declaration node
    return std::make_unique<PropertyDeclarationNode>(
        propertyName, std::move(propertyType),
        nullptr); // You may need to provide initializer if supported

    throw std::runtime_error("Unsupported interface member type");
  }
}
