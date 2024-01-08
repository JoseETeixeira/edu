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
  bool match(TokenType type);
  Token consume(TokenType type, const std::string &message);
  bool check(TokenType type) const;
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

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

Token Parser::consume(TokenType type, const std::string &message) {
  if (check(type))
    return advance();
  throw std::runtime_error(message);
}

bool Parser::check(TokenType type) const {
  if (isAtEnd())
    return false;
  return peek().type == type;
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
  if (match(TokenType::Declaration)) {
    std::string keyword = previous().value;

    if (keyword == "class") {
      return parseClassDeclaration();
    } else if (keyword == "function" || keyword == "async") {
      return parseFunctionDeclaration();
    } else if (keyword == "const") {
      return parseVariableDeclaration();
    } else if (keyword == "interface") {
      return parseInterfaceDeclaration();
    }
  }
  return parseStatement();
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
  if (match(TokenType::Punctuator) && previous().value == "{") {
    return parseBlockStatement();
  }
  if (match(TokenType::Keyword)) {
    std::string keyword = previous().value;

    if (keyword == "if") {
      return parseIfStatement();
    } else if (keyword == "for") {
      return parseForStatement();
    } else if (keyword == "while") {
      return parseWhileStatement();
    } else if (keyword == "return") {
      return parseReturnStatement();
    } else if (keyword == "break") {
      return parseBreakStatement();
    } else if (keyword == "continue") {
      return parseContinueStatement();
    } else if (keyword == "switch") {
      return parseSwitchStatement();
    } else if (keyword == "try") {
      return parseTryCatchStatement();
    } else if (keyword == "export") {
      return parseExportStatement();
    } else if (keyword == "import") {
      return parseImportStatement();
    } else if (keyword == "null") {
      return parseNullReference();
    } else if (keyword == "console.log") {
      return parseConsoleLog();
    } else if (keyword == "input") {
      return parseInputStatement();
    }
    // Handle other keyword-based statements here.
  }

  // Handle other types of statements (e.g., expression statements) based on
  // your language's syntax.
  return parseExpression(); // Example: Return the parsed expression for
                            // expression statements.
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
  return parseAssignmentExpression();
}

std::unique_ptr<ExpressionNode> Parser::parseAssignmentExpression() {
  auto left = parseOrExpression(); // Start with the next level of precedence,
                                   // which might be logical OR.

  if (match(TokenType::Operator)) {
    std::string operatorValue = previous().value;

    // Check if the operator is an assignment operator
    if (operatorValue == "=" || operatorValue == "+=" ||
        operatorValue == "-=" /* ... other assignment operators ... */) {
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
  }

  return left; // If no assignment operator is found, just return the left-hand
               // expression
}

std::unique_ptr<ExpressionNode> Parser::parseOrExpression() {
  auto left = parseAndExpression(); // Start with the next level of precedence,
                                    // which might be logical AND.

  while (match(TokenType::Operator) && previous().value == "||") {
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

  while (match(TokenType::Operator) && previous().value == "&&") {
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

  while (match(TokenType::Operator) &&
         (previous().value == "==" || previous().value == "!=")) {
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

  while (match(TokenType::Operator) &&
         (previous().value == "<" || previous().value == ">" ||
          previous().value == "<=" || previous().value == ">=")) {
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
    if (match(TokenType::Operator) && previous().value == "+") {
      auto right = parseMultiplicationExpression(); // Parse the right operand.
      left = std::make_unique<AdditionExpressionNode>(std::move(left), "+",
                                                      std::move(right));
    } else if (match(TokenType::Operator) && previous().value == "-") {
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
    if (match(TokenType::Operator) && previous().value == "*") {
      auto right = parseUnaryExpression(); // Parse the right operand.
      left = std::make_unique<MultiplicationExpressionNode>(
          std::move(left), "*", std::move(right));
    } else if (match(TokenType::Operator) && previous().value == "/") {
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
  if (match(TokenType::Operator) &&
      (peek().value == "-" || peek().value == "!")) {
    std::string operatorValue = previous().value; // Get the unary operator
    auto operand = parseUnaryExpression(); // Recursively parse the operand
    return std::make_unique<UnaryExpressionNode>(operatorValue,
                                                 std::move(operand));
  }

  // If no unary operator is found, parse the primary expression
  return parsePrimaryExpression();
}

std::unique_ptr<ExpressionNode> Parser::parsePrimaryExpression() {
  if (match(TokenType::Number)) {
    return std::make_unique<NumberLiteralNode>(previous().value);
  } else if (match(TokenType::String)) {
    return std::make_unique<StringLiteralNode>(previous().value);
  } else if (match(TokenType::Identifier)) {
    return std::make_unique<VariableExpressionNode>(previous().value);
  } else if (match(TokenType::Char)) {
    return std::make_unique<CharLiteralNode>(previous().value);
  } else if (match(TokenType::Punctuator) && previous().value == "(") {
    auto expr = parseExpression(); // Returns std::unique_ptr<ASTNode>
    consume(TokenType::Punctuator, ")");

    // Downcast from ASTNode to ExpressionNode
    return std::unique_ptr<ExpressionNode>(
        static_cast<ExpressionNode *>(expr.release()));
  } else {
    throw std::runtime_error("Unexpected token in primary expression");
  }
}

std::unique_ptr<NullReferenceNode> Parser::parseNullReference() {
  // Ensure that the current token is 'null'
  if (!match(TokenType::Keyword) || previous().value != "null") {
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
  consume(TokenType::Keyword, "Expected 'console.log'");

  // Parse the expression to be logged
  auto expression = parseExpression();

  // Consume the semicolon at the end of the console.log statement
  consume(TokenType::Punctuator, "Expected ';' after console.log statement");

  // Create and return a new ConsoleLogNode
  return std::make_unique<ConsoleLogNode>(std::move(expression));
}

std::unique_ptr<InputStatementNode> Parser::parseInputStatement() {
  // Ensure the current token is the 'input' keyword
  consume(TokenType::Keyword, "Expected 'input'");

  // Optionally, parse a variable declaration if it's part of the input syntax
  std::unique_ptr<VariableDeclarationNode> variable;
  if (match(TokenType::Identifier)) {
    std::string variableName = previous().value;
    consume(TokenType::Operator,
            "="); // Assuming the syntax is 'input variableName = input();'

    // Parse the input function call (assuming it's represented as a call)
    consume(TokenType::Keyword,
            "input"); // Consuming the actual input keyword again if the syntax
                      // is like 'input()'
    consume(TokenType::Punctuator, "(");
    consume(TokenType::Punctuator, ")");

    variable = std::make_unique<VariableDeclarationNode>(variableName);
  }

  // Consume the semicolon at the end of the input statement
  consume(TokenType::Punctuator, "Expected ';' after input statement");

  // Create and return a new InputStatementNode
  return std::make_unique<InputStatementNode>(std::move(variable));
}

std::unique_ptr<ClassNode> Parser::parseClassDeclaration() {
  // Ensure we are starting with 'class' keyword
  consume(TokenType::Keyword, "Expected 'class' keyword");

  // Get the class name
  std::string className =
      consume(TokenType::Identifier, "Expected class name").value;

  // Create a new ClassNode with the parsed name
  auto classNode = std::make_unique<ClassNode>(className, previous().line);

  // Consume the opening brace '{' of the class body
  consume(TokenType::Punctuator, "Expected '{' after class name");

  // Parse class members until closing brace '}'
  while (!check(TokenType::Punctuator) && !isAtEnd()) {
    classNode->members.push_back(parseClassMember());
  }

  // Consume the closing brace '}'
  consume(TokenType::Punctuator, "Expected '}' at end of class body");

  return classNode;
}

std::unique_ptr<ASTNode> Parser::parseClassMember() {
  // Check if the member is a method
  if (match(TokenType::Keyword) &&
      (previous().value == "function" || previous().value == "async")) {
    return parseFunctionDeclaration();
  }
  // Check if the member is a property
  else if (match(TokenType::Keyword) && isType(previous().value)) {
    return parsePropertyDeclaration();
  }
  // Handle other types of class members
  // Example: parsing a constructor
  else if (match(TokenType::Keyword) && previous().value == "constructor") {
    return parseConstructorDeclaration();
  }

  throw std::runtime_error("Unsupported class member type");
}

std::unique_ptr<ASTNode> Parser::parsePropertyDeclaration() {
  // Assuming properties are declared like variables
  std::string propertyName =
      consume(TokenType::Identifier, "Expected property name").value;

  // Optional: Parse property type if it follows the property name
  std::unique_ptr<TypeNode> propertyType;
  if (peek().type == TokenType::Identifier) {
    // Assuming next token is type if it's an identifier
    propertyType = parseType();
  }

  // Check for '=' and parse the initializer expression if present
  std::unique_ptr<ExpressionNode> initializer;
  if (match(TokenType::Operator) && previous().value == "=") {
    auto expr = parseExpression(); // Returns std::unique_ptr<ASTNode>
    initializer = std::unique_ptr<ExpressionNode>(
        dynamic_cast<ExpressionNode *>(expr.release()));
    if (!initializer) {
      error("Expected expression for initializer");
    }
  }

  // Consume the semicolon at the end of the property declaration
  consume(TokenType::Punctuator, "Expected ';' after property declaration");

  // Create and return a property declaration node
  return std::make_unique<PropertyDeclarationNode>(
      propertyName, std::move(propertyType), std::move(initializer),
      previous().line);
}

std::unique_ptr<ASTNode> Parser::parseConstructorDeclaration() {
  // Assuming a constructor is like a function but without a return type
  consume(TokenType::Punctuator, "Expected '(' after 'constructor'");
  auto parameters = parseParameters();
  consume(TokenType::Punctuator, "Expected ')' after constructor parameters");

  auto body = parseBlockStatement(); // Parse the constructor's body

  return std::make_unique<ConstructorNode>(std::move(parameters),
                                           std::move(body));
}
