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

// Parsers for each type of node
