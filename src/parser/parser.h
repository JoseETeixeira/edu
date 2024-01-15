#include "nodes.h"
#include "tokenizer.h"
#include <memory>
#include <stdexcept>
#include <vector>

class Parser {
public:
  Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}

  std::unique_ptr<ProgramNode> parse();

private:
  const std::vector<Token> &tokens;
  std::set<std::string> declaredClasses;
  std::set<std::string> declaredInterfaces;

  int current;

  // Utility methods
  bool match(TokenType type, const std::string &expectedValue) {
    if (check(type, expectedValue)) {
      advance();
      return true;
    }
    return false;
  }

  Token consume(TokenType type, const std::string &expectedValue,
                const std::string &errorMessage) {
    if (check(type, expectedValue)) {
      return advance();
    } else {
      throw std::runtime_error(errorMessage);
    }
  }

  bool check(TokenType type, const std::string &expectedValue) {
    if (isAtEnd())
      return false;
    if (expectedValue != "") {
      return peek().type == type && peek().value == expectedValue;
    } else {
      return peek().type == type;
    }
  }

  Token advance() {
    if (!isAtEnd())
      current++;
    return previous();
  }

  Token peek() const { return tokens[current]; }

  Token previous() { return tokens[current - 1]; }
  bool isAtEnd() const;
  void error(const std::string &message);
  bool isType(const std::string &keyword);
  bool isClassName(const std::string &name);
  bool isInterfaceName(const std::string &name);

  Token peekNext() const {
    if (current + 1 < tokens.size()) {
      return tokens[current + 1];
    }
    return tokens
        .back(); // Return the last token if there are no more tokens ahead
  }

  Token peekNextNext() const {
    if (current + 2 >= tokens.size()) {
      return tokens.back(); // Return the last token if out of range
    }
    return tokens[current + 2];
  }

  // Main Parsing Methods
  std::unique_ptr<ASTNode> parseDeclaration();
  std::unique_ptr<ASTNode> parseStatement();
  std::unique_ptr<ExpressionNode> parseExpression();

  // Declaration Parsing
  std::unique_ptr<ClassNode> parseClassDeclaration();
  std::unique_ptr<FunctionNode> parseFunctionDeclaration();
  std::unique_ptr<VariableDeclarationNode>
  parseVariableDeclaration(std::string type);
  std::unique_ptr<InterfaceNode> parseInterfaceDeclaration();
  std::unique_ptr<TemplateNode> parseTemplateDeclaration();
  std::unique_ptr<FunctionParameterNode> parseFunctionParameter();

  // Statement Parsing
  std::unique_ptr<ASTNode> parseClassMember();
  std::unique_ptr<ASTNode> parsePropertyDeclaration();
  std::unique_ptr<ASTNode> parseConstructorDeclaration();
  std::unique_ptr<ASTNode> parseInterfaceMember();
  std::unique_ptr<NullReferenceNode> parseNullReference();
  std::unique_ptr<ConsoleLogNode> parseConsoleLog();
  std::unique_ptr<InputStatementNode> parseInputStatement();
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
  std::unique_ptr<ExpressionNode> parseLiteral();
  std::unique_ptr<ArrayLiteralNode> parseArrayLiteral();
  std::unique_ptr<ObjectLiteralNode> parseObjectLiteral();
  std::unique_ptr<ExpressionNode> parseAnonymousFunction();

  std::unique_ptr<CallExpressionNode>
  parseCallExpression(std::unique_ptr<VariableExpressionNode> callee);
  std::unique_ptr<MemberAccessExpressionNode>
  parseMemberAccessExpression(std::unique_ptr<ExpressionNode> object);

  // Utility Parsing Methods
  std::unique_ptr<TypeNode> parseType();
  std::vector<std::unique_ptr<FunctionParameterNode>> parseParameters();
  std::unique_ptr<AwaitExpressionNode> parseAwaitExpression();
  std::unique_ptr<CaseClauseNode> parseCaseClause();
  std::unique_ptr<StatementNode> parseExpressionStatement();
};

bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }

void Parser::error(const std::string &message) {
  // Error handling logic
  throw std::runtime_error(message);
}

std::unique_ptr<ProgramNode> Parser::parse() {
  auto program =
      std::make_unique<ProgramNode>(0); // Assuming 0 as the starting line

  while (!isAtEnd()) {
    try {
      program->children.push_back(parseDeclaration());
    } catch (const std::runtime_error &e) {
      throw std::runtime_error(e.what());
    }
  }

  return program;
}

// Main Parsing Methods

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
  if (match(TokenType::Keyword, "export")) {
    auto exportedItem = parseDeclaration();
    std::unique_ptr<ExportNode> node =
        std::make_unique<ExportNode>(previous().line);
    node->exportItem = std::move(exportedItem);
    return node;
  } else if (match(TokenType::Keyword, "template")) {
    return parseTemplateDeclaration();
  } else if (peek().value == "class" && peek().type == TokenType::Declaration) {
    return parseClassDeclaration();
  } else if (peek().type == TokenType::Keyword &&
             peekNext().value == "function") {
    return parseFunctionDeclaration();
  } else if (peek().type == TokenType::Keyword && peek().value == "async" &&
             peekNext().value == "function") {
    return parseFunctionDeclaration();
  } else if (match(TokenType::Keyword, "interface")) {
    return parseInterfaceDeclaration();
  } else if (match(TokenType::Keyword, "if")) {
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
  } else if (match(TokenType::Keyword, "print")) {
    return parseConsoleLog();
  } else if (match(TokenType::Keyword, "await")) {
    return parseAwaitExpression();
  } else if (match(TokenType::Keyword, "input")) {
    return parseInputStatement();
  } else if (match(TokenType::Keyword, "") ||
             match(TokenType::Keyword, "const")) {
    return parseVariableDeclaration(previous().value);
  }
  return parseStatement();
}

std::unique_ptr<FunctionParameterNode> Parser::parseFunctionParameter() {
  std::unique_ptr<TypeNode> paramType;
  if (isType(peek().value)) {
    // Parse parameter type if present
    paramType = parseType();
  }
  std::string paramName =
      consume(TokenType::Identifier, "", "Expected parameter name").value;

  std::unique_ptr<FunctionParameterNode> parameter =
      std::make_unique<FunctionParameterNode>(paramName, previous().line);
  parameter->type = std::move(paramType);
  return parameter;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
  if (peek().type == TokenType::Punctuator && peek().value == "{") {
    return parseBlockStatement();
  }

  return parseExpression();
}

std::unique_ptr<ExpressionNode> Parser::parseExpression() {
  if (check(TokenType::Punctuator, "[")) {
    return parseArrayLiteral();
  } else if (check(TokenType::Punctuator, "{")) {
    return parseObjectLiteral();
  } else {
    return parseAssignmentExpression();
  }
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
      std::unique_ptr<AssignmentExpressionNode> node =
          std::make_unique<AssignmentExpressionNode>(operatorValue,
                                                     previous().line);
      node->left = std::move(left);
      node->right = std::move(right);
      return node;
    } else {
      error("Invalid left-hand side in assignment");
    }
  }

  return left; // If no assignment operator is found, just return the
               // left-hand expression
}

std::unique_ptr<ExpressionNode> Parser::parseOrExpression() {
  auto left = parseAndExpression(); // Start with the next level of
                                    // precedence, which might be logical AND.

  while (match(TokenType::Operator, "||")) {
    std::string operatorValue = previous().value;
    auto right = parseAndExpression(); // Recursively parse the right operand.
    left = std::make_unique<OrExpressionNode>(
        std::move(left), operatorValue, std::move(right), previous().line);
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
    left = std::make_unique<AndExpressionNode>(
        std::move(left), operatorValue, std::move(right), previous().line);
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
        std::move(left), operatorValue, std::move(right), previous().line);
  }

  return left; // If no equality operator is found, just return the left
               // operand expression.
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
        std::move(left), operatorValue, std::move(right), previous().line);
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
      left = std::make_unique<AdditionExpressionNode>(
          std::move(left), "+", std::move(right), previous().line);
    } else if (match(TokenType::Operator, "-")) {
      auto right = parseMultiplicationExpression(); // Parse the right operand.
      left = std::make_unique<SubtractionExpressionNode>(
          std::move(left), "-", std::move(right), previous().line);
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
          std::move(left), "*", std::move(right), previous().line);
    } else if (match(TokenType::Operator, "/")) {
      auto right = parseUnaryExpression(); // Parse the right operand.
      left = std::make_unique<DivisionExpressionNode>(
          std::move(left), "/", std::move(right), previous().line);
    } else {
      break; // No more multiplication or division operators.
    }
  }

  return left; // Return the built expression node.
}

std::unique_ptr<ExpressionNode> Parser::parseUnaryExpression() {
  if (match(TokenType::Identifier, "")) {
    std::string identifier = previous().value;

    if (match(TokenType::Punctuator, "(")) {
      // It's a function call
      return parseCallExpression(std::make_unique<VariableExpressionNode>(
          identifier, previous().line));
    } else if (match(TokenType::Punctuator, ".")) {
      // It's a member access
      return parseMemberAccessExpression(
          std::make_unique<VariableExpressionNode>(identifier,
                                                   previous().line));
    }

    // If it's just an identifier (not a function call or member access)
    return std::make_unique<VariableExpressionNode>(identifier,
                                                    previous().line);
  }

  // Check for unary operators
  if (match(TokenType::Operator, "-") || match(TokenType::Operator, "!")) {
    std::string operatorValue = previous().value; // Get the unary operator
    auto operand = parseUnaryExpression(); // Recursively parse the operand
    std::unique_ptr<UnaryExpressionNode> node =
        std::make_unique<UnaryExpressionNode>(operatorValue, previous().line);
    node->operand = std::move(operand);
    return node;
  }

  // If no unary operator is found, parse the primary expression
  return parsePrimaryExpression();
}

std::unique_ptr<ExpressionNode> Parser::parsePrimaryExpression() {
  if (match(TokenType::Number, "") || match(TokenType::String, "") ||
      match(TokenType::Character, "") || match(TokenType::Keyword, "true") ||
      match(TokenType::Keyword, "false") || match(TokenType::Keyword, "null")) {
    return parseLiteral();
  } else if (match(TokenType::Identifier, "")) {
    std::string identifier = previous().value;

    if (match(TokenType::Punctuator, "(")) {
      // It's a function call
      std::vector<std::unique_ptr<ExpressionNode>> arguments;
      if (!check(TokenType::Punctuator, ")")) {
        do {
          arguments.push_back(parseExpression());
        } while (match(TokenType::Punctuator, ","));
      }
      consume(TokenType::Punctuator, ")", "Expected ')' after arguments");
      std::unique_ptr<CallExpressionNode> callNode =
          std::make_unique<CallExpressionNode>(previous().line);
      callNode->arguments = std::move(arguments);
      callNode->callee =
          std::make_unique<VariableExpressionNode>(identifier, previous().line);
      return callNode;
    } else if (match(TokenType::Punctuator, ".")) {
      // It's a member access
      consume(TokenType::Identifier, "", "Expected member name after '.'");
      std::string memberName = previous().value;
      std::unique_ptr<MemberAccessExpressionNode> memberAccessNode =
          std::make_unique<MemberAccessExpressionNode>(previous().line);
      memberAccessNode->memberName = memberName;
      memberAccessNode->object =
          std::make_unique<VariableExpressionNode>(identifier, previous().line);
      return memberAccessNode;
    }

    // Just an identifier
    return std::make_unique<VariableExpressionNode>(identifier,
                                                    previous().line);
  } else if (match(TokenType::Punctuator, "[")) {
    return parseArrayLiteral();
  } else if (match(TokenType::Punctuator, "{")) {
    return parseObjectLiteral();
  } else if (match(TokenType::Keyword, "function")) {
    return parseAnonymousFunction();
  } else if (peek().value == "new") {
    return parseAnonymousFunction();
  } else if (match(TokenType::Punctuator, "(")) {
    auto expr = parseExpression();
    consume(TokenType::Punctuator, ")", "Expected ')' after expression");
    return expr;
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

  // Parse the expression to be logged
  auto expression = parseExpression();

  // Consume the semicolon at the end of the print statement
  consume(TokenType::Punctuator, ";", "Expected ';' after print statement");

  // Create and return a new ConsoleLogNode
  auto console_log_node = std::make_unique<ConsoleLogNode>(previous().line);
  console_log_node->expression = std::move(expression);
  return console_log_node;
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
            "Expected 'input'"); // Consuming the actual input keyword again
                                 // if the syntax is like 'input()'
    consume(TokenType::Punctuator, "(", "Expected '('");
    consume(TokenType::Punctuator, ")", "Expected ')'");

    variable = std::make_unique<VariableDeclarationNode>(variableName,
                                                         previous().line);
  }

  // Consume the semicolon at the end of the input statement
  consume(TokenType::Punctuator, ";", "Expected ';' after input statement");

  // Create and return a new InputStatementNode
  std::unique_ptr<InputStatementNode> node =
      std::make_unique<InputStatementNode>(previous().line);
  node->variable = std::move(variable);
  return node;
}
std::unique_ptr<ClassNode> Parser::parseClassDeclaration() {
  consume(TokenType::Declaration, "class", "Expected 'class' keyword");
  auto classNameToken =
      consume(TokenType::Identifier, "", "Expected class name");
  std::string className = classNameToken.value;

  std::string baseClassName;
  if (match(TokenType::Keyword, "extends")) {
    baseClassName =
        consume(TokenType::Identifier, "", "Expected base class name").value;
  }

  // Add the class name to declaredClasses
  declaredClasses.insert(className);

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
  std::cout << peek().value << std::endl;
  // Check if the member is a method
  if (match(TokenType::Keyword, "") && isType(previous().value) &&
      (peek().value == "async" || peek().value == "function")) {
    std::cout << "Parsing function declaration" << std::endl;
    return parseFunctionDeclaration();
  }
  // Check if the member is a property
  else if (isType(peek().value)) {
    std::cout << "Parsing property declaration" << std::endl;
    return parsePropertyDeclaration();
  }
  // Handle other types of class members
  // Example: parsing a constructor
  else if (match(TokenType::Keyword, "constructor")) {
    return parseConstructorDeclaration();
  } else if (isType(peek().value)) {
    return parsePropertyDeclaration();
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
                                           std::move(body), previous().line);
}
std::unique_ptr<FunctionNode> Parser::parseFunctionDeclaration() {
  bool isAsync = false;
  std::string returnType;

  // Check for 'async' keyword
  if (peek().type == TokenType::Keyword && peek().value == "async") {
    isAsync = true;
    advance(); // Consume 'async'
  }

  // Check for return type
  if (peek().type == TokenType::Keyword && isType(peek().value)) {
    returnType = peek().value;
    advance(); // Consume return type
  }

  // Consume 'function' keyword
  consume(TokenType::Declaration, "function", "Expected 'function' keyword");

  // Get the function name
  std::string functionName =
      consume(TokenType::Identifier, "", "Expected function name").value;

  // Consume the opening parenthesis '(' of the parameter list
  consume(TokenType::Punctuator, "(", "Expected '(' after function name");

  // Parse function parameters
  std::vector<std::unique_ptr<FunctionParameterNode>> parameters;
  if (!check(TokenType::Punctuator, ")")) {
    do {
      parameters.push_back(parseFunctionParameter());
    } while (match(TokenType::Punctuator, ","));
  }

  // Consume the closing parenthesis ')' of the parameter list
  consume(TokenType::Punctuator, ")", "Expected ')' after parameters");
  // Parse the function body
  auto body = parseBlockStatement();

  // Create and return a new FunctionNode
  auto functionNode =
      std::make_unique<FunctionNode>(functionName, previous().line);
  functionNode->parameters = std::move(parameters);
  functionNode->body = std::move(body);
  functionNode->returnType = returnType;
  functionNode->isAsync = isAsync;

  return functionNode;
}

std::unique_ptr<VariableDeclarationNode>
Parser::parseVariableDeclaration(std::string type) {
  bool isConst = match(TokenType::Keyword, "const");

  // First, correctly parse the type
  auto variableName =
      consume(TokenType::Identifier, "", "Expected identifier").value;
  std::string typeName = type;
  if (!isType(typeName)) {
    throw std::runtime_error("Unknown type: " + typeName);
  }

  // Optional initializer
  std::unique_ptr<ExpressionNode> initializer;
  if (match(TokenType::Operator, "=")) {
    initializer = parseExpression();
  }

  consume(TokenType::Punctuator, ";",
          "Expected ';' after variable declaration");

  std::unique_ptr<VariableDeclarationNode> node =
      std::make_unique<VariableDeclarationNode>(variableName, previous().line);
  node->initializer = std::move(initializer);
  node->isConst = isConst;
  node->typeName = typeName;
  return node;
}

std::unique_ptr<InterfaceNode> Parser::parseInterfaceDeclaration() {
  // Ensure we are starting with 'interface' keyword
  consume(TokenType::Keyword, "interface", "Expected 'interface'");

  // Get the interface name
  std::string interfaceName =
      consume(TokenType::Identifier, "Expected interface name", "").value;

  // Add the class name to declaredClasses
  declaredInterfaces.insert(interfaceName);

  // Create a new InterfaceNode with the parsed name
  auto interfaceNode =
      std::make_unique<InterfaceNode>(interfaceName, previous().line);

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
        propertyName, std::move(propertyType), nullptr,
        previous().line); // You may need to provide initializer if supported

    throw std::runtime_error("Unsupported interface member type");
  }
}

std::unique_ptr<BlockStatementNode> Parser::parseBlockStatement() {
  // Consume the opening brace '{'
  consume(TokenType::Punctuator, "{", "Expected '{' at the start of block");

  // Create a block statement node
  auto block = std::make_unique<BlockStatementNode>(previous().line);

  // Parse statements until the closing brace '}'
  while (!check(TokenType::Punctuator, "}") && !isAtEnd()) {
    try {
      block->statements.push_back(std::unique_ptr<StatementNode>(
          dynamic_cast<StatementNode *>(parseDeclaration().release())));
    } catch (const std::runtime_error &e) {
      error(e.what()); // Handle parsing errors in each statement
    }
  }

  // Consume the closing brace '}'
  consume(TokenType::Punctuator, "}", "Expected '}' at the end of block");

  return block;
}

std::unique_ptr<IfStatementNode> Parser::parseIfStatement() {
  // Consume the opening parenthesis '('
  consume(TokenType::Punctuator, "(", "Expected '(' after 'if'");

  // Parse the condition expression
  auto condition = parseExpression();

  // Consume the closing parenthesis ')'
  consume(TokenType::Punctuator, ")", "Expected ')' after if condition");

  // Parse the 'then' branch (true branch)
  auto thenBranch = parseStatement();

  // Optional 'else' branch
  std::unique_ptr<StatementNode> elseBranch = nullptr;
  if (match(TokenType::Keyword, "else")) {
    auto elseStatement = parseStatement();
    elseBranch = std::unique_ptr<StatementNode>(
        dynamic_cast<StatementNode *>(elseStatement.release()));
    if (!elseBranch) {
      error("Expected a statement for the 'else' branch");
    }
  }

  // Create and return the IfStatementNode
  std::unique_ptr<IfStatementNode> node =
      std::make_unique<IfStatementNode>(previous().line);
  node->condition = std::move(condition);
  node->thenBranch = std::move(thenBranch);
  node->elseBranch = std::move(elseBranch);
  return node;
}

std::unique_ptr<ForStatementNode> Parser::parseForStatement() {
  // Consume the 'for' keyword
  consume(TokenType::Keyword, "for", "Expected 'for'");

  // Consume the opening parenthesis '('
  consume(TokenType::Punctuator, "(", "Expected '(' after 'for'");

  // Parse the initializer
  std::unique_ptr<StatementNode> initializer;
  if (match(TokenType::Keyword, "") || match(TokenType::Keyword, "")) {
    initializer = parseVariableDeclaration(previous().value);
  } else if (!match(TokenType::Punctuator, ";")) {
    initializer = parseExpressionStatement();
  }

  // Parse the condition
  std::unique_ptr<ExpressionNode> condition;
  if (!check(TokenType::Punctuator, ";")) {
    auto expr = parseExpression();
    condition = std::unique_ptr<ExpressionNode>(
        dynamic_cast<ExpressionNode *>(expr.release()));
    if (!condition) {
      error("Expected a valid expression for the condition");
    }
  }
  consume(TokenType::Punctuator, ";", "Expected ';' after for condition");

  // Parse the increment
  std::unique_ptr<ExpressionNode> increment;
  if (!check(TokenType::Punctuator, ")")) {
    auto expr = parseExpression();
    increment = std::unique_ptr<ExpressionNode>(
        dynamic_cast<ExpressionNode *>(expr.release()));
    if (!increment) {
      error("Expected a valid expression for the increment");
    }
  }
  consume(TokenType::Punctuator, ")", "Expected ')' after for clauses");

  // Parse the body
  auto body = parseStatement();

  // Create and return the ForStatementNode
  std::unique_ptr<ForStatementNode> node =
      std::make_unique<ForStatementNode>(previous().line);
  node->body = std::move(body);
  node->condition = std::move(condition);
  node->increment = std::move(increment);
  node->initializer = std::move(initializer);
  return node;
}

std::unique_ptr<StatementNode> Parser::parseExpressionStatement() {
  // Parse the expression
  auto expr = parseExpression();

  // Consume the semicolon at the end of the expression statement
  consume(TokenType::Punctuator, ";", "Expected ';' after expression");

  // Return the expression wrapped in a StatementNode
  // Assuming you have a wrapper node for this purpose, or modify as needed
  return std::make_unique<ExpressionStatementNode>(std::move(expr),
                                                   previous().line);
}

std::unique_ptr<WhileStatementNode> Parser::parseWhileStatement() {
  // Consume the 'while' keyword
  consume(TokenType::Keyword, "while",
          "Expected 'while' keyword in while statement");

  // Consume the opening parenthesis '('
  consume(TokenType::Punctuator, "(", "Expected '(' after 'while'");

  // Parse the condition expression inside the while statement
  auto condition = parseExpression();

  // Consume the closing parenthesis ')'
  consume(TokenType::Punctuator, ")", "Expected ')' after while condition");

  // Parse the statement that forms the body of the while loop
  auto body = parseStatement();

  // Create and return the while statement node
  std::unique_ptr<WhileStatementNode> node =
      std::make_unique<WhileStatementNode>(previous().line);
  node->condition = std::move(condition);
  node->body = std::move(body);
  return node;
}

std::unique_ptr<ReturnStatementNode> Parser::parseReturnStatement() {

  std::unique_ptr<ExpressionNode> returnValue;

  // Check if the next token is not a semicolon, indicating a return value is
  // present
  if (!check(TokenType::Punctuator, ";")) {
    // Parse the return value expression
    auto expr = parseExpression();

    // Cast the ASTNode to ExpressionNode
    returnValue = std::unique_ptr<ExpressionNode>(
        dynamic_cast<ExpressionNode *>(expr.release()));
    if (!returnValue) {
      error("Invalid expression in return statement");
    }
  }

  // Consume the semicolon at the end of the return statement
  consume(TokenType::Punctuator, ";", "Expected ';' after return statement");

  // Create and return the return statement node with the optional return value
  std::unique_ptr<ReturnStatementNode> node =
      std::make_unique<ReturnStatementNode>(previous().line);
  node->expression = std::move(returnValue);
  return node;
}

std::unique_ptr<BreakStatementNode> Parser::parseBreakStatement() {
  // Consume the 'break' keyword
  consume(TokenType::Keyword, "break",
          "Expected 'break' keyword in break statement");

  // Consume the semicolon at the end of the break statement
  consume(TokenType::Punctuator, ";", "Expected ';' after break statement");

  // Create and return the break statement node
  return std::make_unique<BreakStatementNode>(previous().line);
}

std::unique_ptr<ContinueStatementNode> Parser::parseContinueStatement() {
  // Consume the 'continue' keyword
  consume(TokenType::Keyword, "continue",
          "Expected 'continue' keyword in continue statement");

  // Consume the semicolon at the end of the continue statement
  consume(TokenType::Punctuator, ";", "Expected ';' after continue statement");

  // Create and return the continue statement node
  return std::make_unique<ContinueStatementNode>(previous().line);
}

std::unique_ptr<SwitchStatementNode> Parser::parseSwitchStatement() {
  // Consume the 'switch' keyword
  consume(TokenType::Keyword, "switch",
          "Expected 'switch' keyword in switch statement");

  // Consume the opening parenthesis '('
  consume(TokenType::Punctuator, "(", "Expected '(' after 'switch'");

  // Parse the control expression for the switch
  auto controlExpression = parseExpression();

  // Consume the closing parenthesis ')'
  consume(TokenType::Punctuator, ")",
          "Expected ')' after switch control expression");

  // Consume the opening brace '{' for the switch body
  consume(TokenType::Punctuator, "{",
          "Expected '{' at the start of switch body");

  // Prepare a vector to hold the case clauses
  std::vector<std::unique_ptr<CaseClauseNode>> cases;

  // Loop to parse each case clause
  while (!check(TokenType::Punctuator, "}") && !isAtEnd()) {
    cases.push_back(parseCaseClause());
  }

  // Consume the closing brace '}' for the switch body
  consume(TokenType::Punctuator, "}", "Expected '}' at the end of switch body");

  // Create and return the switch statement node
  std::unique_ptr<SwitchStatementNode> node =
      std::make_unique<SwitchStatementNode>(previous().line);
  node->condition = std::move(controlExpression);
  node->cases = std::move(cases);
  return node;
}

std::unique_ptr<TryCatchNode> Parser::parseTryCatchStatement() {
  // Consume the 'try' keyword
  consume(TokenType::Keyword, "try",
          "Expected 'try' keyword in try-catch statement");

  // Parse the try block
  auto tryBlock = parseBlockStatement();

  // Consume the 'catch' keyword
  consume(TokenType::Keyword, "catch",
          "Expected 'catch' keyword after try block");

  // Consume the opening parenthesis '('
  consume(TokenType::Punctuator, "(", "Expected '(' after 'catch'");

  // Parse the error type and variable name
  consume(TokenType::Identifier, "Error",
          "Expected 'Error' type in catch clause");
  std::string errorVarName =
      consume(TokenType::Identifier, "", "Expected error variable name").value;

  // Consume the closing parenthesis ')'
  consume(TokenType::Punctuator, ")", "Expected ')' after catch clause header");

  // Parse the catch block
  auto catchBlock = parseBlockStatement();

  // Create the error object structure (assuming a constructor or similar method
  // exists)
  auto errorObject = std::make_unique<ErrorTypeNode>(
      errorVarName, "message", "errorCode", previous().line);

  // Create and return the try-catch statement node
  std::unique_ptr<TryCatchNode> node =
      std::make_unique<TryCatchNode>(previous().line);
  node->catchBlock = std::move(catchBlock);
  node->tryBlock = std::move(tryBlock);
  return node;
}

std::unique_ptr<ExportNode> Parser::parseExportStatement() {
  // Consume the 'export' keyword
  consume(TokenType::Keyword, "export",
          "Expected 'export' keyword in export statement");

  // Determine what is being exported: class, function, variable, interface,
  // template, or async function
  std::unique_ptr<ASTNode> exportItem;

  if (peek().type == TokenType::Keyword) {
    const std::string &nextTokenValue = peek().value;
    if (nextTokenValue == "class" || nextTokenValue == "function" ||
        nextTokenValue == "interface" || nextTokenValue == "template" ||
        nextTokenValue == "async") {
      // If the next token is a 'class', 'function', 'interface', 'template', or
      // 'async', parse it accordingly
      exportItem = parseDeclaration();
    } else {
      exportItem = parseVariableDeclaration(peek().value);
    }
  } else {
    error("Expected a declaration after 'export'");
  }

  // Ensure that exportItem is not null
  if (!exportItem) {
    error("Expected a valid item to export");
  }

  // Create and return the export node
  std::unique_ptr<ExportNode> node =
      std::make_unique<ExportNode>(previous().line);
  node->exportItem = std::move(exportItem);
  return node;
}

std::unique_ptr<ImportNode> Parser::parseImportStatement() {
  // Consume 'import' keyword
  consume(TokenType::Keyword, "import", "Expected 'import' keyword");

  // Parse the module name (assuming it's a string literal)
  std::string moduleName =
      consume(TokenType::String, "", "Expected module name").value;

  // Optional: Parse imported items
  std::vector<std::string> imports;
  if (match(TokenType::Punctuator, "{")) {
    do {
      std::string importItem =
          consume(TokenType::Identifier, "", "Expected import item").value;
      imports.push_back(importItem);
    } while (match(TokenType::Punctuator, ","));
    consume(TokenType::Punctuator, "}", "Expected '}' after import items");
  }

  // Optional: Consume 'from' keyword
  if (match(TokenType::Keyword, "from")) {
    // Consume the actual module name
    moduleName =
        consume(TokenType::String, "", "Expected module name after 'from'")
            .value;
  }

  // Consume the end of statement token (semicolon)
  consume(TokenType::Punctuator, ";", "Expected ';' after import statement");

  // Create and return the ImportNode
  std::unique_ptr<ImportNode> node =
      std::make_unique<ImportNode>(previous().line);
  node->moduleName = std::move(moduleName);
  node->imports = std::move(imports);
  return node;
}

std::unique_ptr<CallExpressionNode>
Parser::parseCallExpression(std::unique_ptr<VariableExpressionNode> callee) {
  std::vector<std::unique_ptr<ExpressionNode>> arguments;
  if (!check(TokenType::Punctuator, ")")) {
    do {
      arguments.push_back(parseExpression());
    } while (match(TokenType::Punctuator, ","));
  }
  consume(TokenType::Punctuator, ")", "Expected ')' after arguments");
  std::unique_ptr<CallExpressionNode> node =
      std::make_unique<CallExpressionNode>(previous().line);
  node->callee = std::move(callee);
  node->arguments = std::move(arguments);
  return node;
}
std::unique_ptr<MemberAccessExpressionNode>
Parser::parseMemberAccessExpression(std::unique_ptr<ExpressionNode> object) {
  auto memberNameToken =
      consume(TokenType::Identifier, "", "Expected member name after '.'");
  std::string memberName = memberNameToken.value;

  auto node =
      std::make_unique<MemberAccessExpressionNode>(memberNameToken.line);
  node->object = std::move(object);
  node->memberName = std::move(memberName);

  return node;
}

std::unique_ptr<ExpressionNode> Parser::parseAnonymousFunction() {
  // Assuming the 'function' keyword has already been consumed

  consume(TokenType::Punctuator, "(", "Expected '(' after 'function'");

  std::vector<std::unique_ptr<FunctionParameterNode>> parameters;
  if (!check(TokenType::Punctuator, ")")) {
    do {
      std::string paramName =
          consume(TokenType::Identifier, "", "Expected parameter name").value;
      // If your language supports types for parameters, parse the type here
      parameters.push_back(
          std::make_unique<FunctionParameterNode>(paramName, previous().line));
    } while (match(TokenType::Punctuator, ","));
  }

  consume(TokenType::Punctuator, ")", "Expected ')' after parameters");

  auto body = parseBlockStatement(); // Assuming parseBlockStatement() parses a
                                     // block of statements

  // Create a FunctionNode. Since it's anonymous, we pass an empty string for
  // the function name
  auto functionNode = std::make_unique<FunctionNode>("", previous().line);

  // Wrap the FunctionNode in an ExpressionNode if necessary
  return std::make_unique<FunctionExpressionNode>(std::move(functionNode),
                                                  previous().line);
}

std::unique_ptr<ExpressionNode> Parser::parseLiteral() {
  if (previous().type == TokenType::Number) {
    // For numeric literals (integers, floats, doubles)
    std::string value = previous().value;
    if (value.find('.') != std::string::npos) {
      // Contains a decimal point, treat as a floating point or double
      return std::make_unique<FloatingPointLiteralNode>(value, previous().line);
    } else {
      // No decimal point, treat as an integer
      return std::make_unique<IntegerLiteralNode>(value, previous().line);
    }
  } else if (previous().type == TokenType::String) {
    // For string literals
    return std::make_unique<StringLiteralNode>(previous().value,
                                               previous().line);
  } else if ((previous().type == TokenType::Keyword &&
              previous().value == "true") ||
             (previous().type == TokenType::Keyword &&
              previous().value == "false")) {
    // For boolean literals
    bool value = previous().value == "true";
    return std::make_unique<BooleanLiteralNode>(value, previous().line);
  } else if ((previous().type == TokenType::Keyword &&
              previous().value == "null")) {
    // For null literals
    return std::make_unique<NullLiteralNode>(previous().line);
  } else if (previous().type == TokenType::Character) {
    char value = previous().value[0];
    return std::make_unique<CharacterLiteralNode>(value, previous().line);
  }
  // Add more cases as needed for other types of literals

  throw std::runtime_error("Expected literal");
}

std::unique_ptr<ArrayLiteralNode> Parser::parseArrayLiteral() {
  consume(TokenType::Punctuator, "[",
          "Expected '[' at the start of array literal");

  std::vector<std::unique_ptr<ExpressionNode>> elements;
  if (!check(TokenType::Punctuator, "]")) {
    do {
      auto element = parseExpression();
      elements.push_back(std::unique_ptr<ExpressionNode>(
          dynamic_cast<ExpressionNode *>(element.release())));
      if (!elements.back()) {
        throw std::runtime_error("Expected expression in array literal");
      }
    } while (match(TokenType::Punctuator, ","));
  }

  consume(TokenType::Punctuator, "]",
          "Expected ']' at the end of array literal");

  std::unique_ptr<ArrayLiteralNode> node =
      std::make_unique<ArrayLiteralNode>(previous().line);
  node->elements = std::move(elements);
  return node;
}

std::unique_ptr<ObjectLiteralNode> Parser::parseObjectLiteral() {
  consume(TokenType::Punctuator, "{",
          "Expected '{' at the start of object literal");

  std::vector<std::pair<std::string, std::unique_ptr<ExpressionNode>>>
      properties;
  if (!check(TokenType::Punctuator, "}")) {
    do {
      // Parse the key
      std::string key;
      if (match(TokenType::String, "") || match(TokenType::Identifier, "")) {
        key = previous().value;
      } else {
        throw std::runtime_error("Expected string or identifier as object key");
      }

      // Parse the colon separator
      consume(TokenType::Punctuator, ":", "Expected ':' after object key");

      // Parse the value (an expression)
      auto value = parseExpression();

      properties.emplace_back(key, std::move(value));
    } while (match(TokenType::Punctuator, ","));
  }

  consume(TokenType::Punctuator, "}",
          "Expected '}' at the end of object literal");

  std::unique_ptr<ObjectLiteralNode> node =
      std::make_unique<ObjectLiteralNode>(previous().line);
  node->properties = std::move(properties);
  return node;
}

std::unique_ptr<TypeNode> Parser::parseType() {
  auto typeToken = consume(TokenType::Keyword, "", "Expected a type");

  std::string typeName = typeToken.value;
  if (typeName == "bool" || typeName == "char" || typeName == "int" ||
      typeName == "float" || typeName == "double" || typeName == "void" ||
      typeName == "wchar_t" || typeName == "string" || typeName == "Error" ||
      isClassName(typeName) || isInterfaceName(typeName)) {
    return std::make_unique<TypeNode>(typeName, previous().line);
  } else {
    throw std::runtime_error("Unknown type: " + typeName);
  }
}

bool Parser::isClassName(const std::string &name) {
  return declaredClasses.find(name) != declaredClasses.end();
}

bool Parser::isInterfaceName(const std::string &name) {
  return declaredInterfaces.find(name) != declaredInterfaces.end();
}

std::vector<std::unique_ptr<FunctionParameterNode>> Parser::parseParameters() {
  std::vector<std::unique_ptr<FunctionParameterNode>> parameters;

  consume(TokenType::Punctuator, "(",
          "Expected '(' at the start of parameters");

  if (!check(TokenType::Punctuator, ")")) {
    do {
      // Parse the type of the parameter
      auto type = parseType();

      // Parse the name of the parameter
      std::string paramName =
          consume(TokenType::Identifier, "", "Expected parameter name").value;

      // Create a FunctionParameterNode and add it to the parameters vector
      std::unique_ptr<FunctionParameterNode> parameter =
          std::make_unique<FunctionParameterNode>(paramName, previous().line);
      parameter->type = std::move(type);
      parameters.push_back(std::move(parameter));
    } while (match(TokenType::Punctuator, ","));
  }

  consume(TokenType::Punctuator, ")", "Expected ')' after parameters");

  return parameters;
}

std::unique_ptr<AwaitExpressionNode> Parser::parseAwaitExpression() {
  consume(TokenType::Keyword, "await", "Expected 'await'");

  auto expression = parseExpression(); // Parse the expression following 'await'
  std::unique_ptr<AwaitExpressionNode> node =
      std::make_unique<AwaitExpressionNode>(previous().line);
  node->expression = std::move(expression);
  return node;
}

std::unique_ptr<CaseClauseNode> Parser::parseCaseClause() {
  std::unique_ptr<ExpressionNode> caseExpression;
  bool isDefault = false;
  int line = peek().line;

  if (match(TokenType::Keyword, "case")) {
    auto expr = parseExpression();
    caseExpression = std::unique_ptr<ExpressionNode>(
        dynamic_cast<ExpressionNode *>(expr.release()));
    if (!caseExpression) {
      throw std::runtime_error("Expected expression after 'case'");
    }
  } else if (match(TokenType::Keyword, "default")) {
    isDefault = true;
  } else {
    throw std::runtime_error("Expected 'case' or 'default' keyword");
  }

  consume(TokenType::Punctuator, ":", "Expected ':' after case value");

  std::vector<std::unique_ptr<StatementNode>> statements;
  while (!check(TokenType::Keyword, "case") &&
         !check(TokenType::Keyword, "default") && !isAtEnd()) {
    auto astNode = parseStatement();
    StatementNode *statementNode = dynamic_cast<StatementNode *>(astNode.get());
    if (statementNode) {
      statements.push_back(std::unique_ptr<StatementNode>(statementNode));
      astNode.release(); // Release ownership from the original unique_ptr
    } else {
      throw std::runtime_error("Expected a statement node");
    }
  }

  if (isDefault) {
    return std::make_unique<CaseClauseNode>(std::move(statements), line);
  } else {
    return std::make_unique<CaseClauseNode>(std::move(caseExpression),
                                            std::move(statements), line);
  }
}

std::unique_ptr<TemplateNode> Parser::parseTemplateDeclaration() {
  consume(TokenType::Keyword, "template", "Expected 'template' keyword");

  consume(TokenType::Punctuator, "<", "Expected '<' after 'template'");
  std::vector<std::string> templateParams;
  do {
    std::string paramName =
        consume(TokenType::Identifier, "", "Expected template parameter name")
            .value;
    templateParams.push_back(paramName);
  } while (match(TokenType::Punctuator, ","));
  consume(TokenType::Punctuator, ">", "Expected '>' after template parameters");

  auto declaration = parseDeclaration();
  return std::make_unique<TemplateNode>(
      std::move(templateParams), std::move(declaration), previous().line);
}