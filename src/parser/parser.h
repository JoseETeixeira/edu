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

  std::unique_ptr<NullReferenceNode> Parser::parseNullReference();

  std::unique_ptr<ConsoleLogNode> Parser::parseConsoleLog();

  std::unique_ptr<InputStatementNode> Parser::parseInputStatement();

  // Declaration Parsing
  std::unique_ptr<ClassNode> parseClassDeclaration();
  std::unique_ptr<FunctionNode> parseFunctionDeclaration();
  std::unique_ptr<VariableDeclarationNode> parseVariableDeclaration();
  std::unique_ptr<InterfaceNode> parseInterfaceDeclaration();

  // Statement Parsing
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

bool Parser::isType(const std::string &keyword) {
  // Check if the keyword is a valid type
  static const std::set<std::string> validTypes = {
      "bool", "char",    "int",    "float", "double",
      "void", "wchar_t", "string", "Error"};
  return validTypes.find(keyword) != validTypes.end();
}