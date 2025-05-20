#include "nodes.h"
#include "tokenizer.h"
#include <memory>
#include <stdexcept>
#include <vector>
#include "../debug.h"

class Parser
{
public:
  Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}

  std::unique_ptr<ProgramNode> parse();

private:
  const std::vector<Token> &tokens;
  std::set<std::string> declaredClasses;
  std::set<std::string> declaredInterfaces;

  int current;

  // Utility methods
  bool match(TokenType type, const std::string &expectedValue)
  {
    if (check(type, expectedValue))
    {
      advance();
      return true;
    }
    return false;
  }

  Token consume(TokenType type, const std::string &expectedValue,
                const std::string &errorMessage)
  {
    if (check(type, expectedValue))
    {
      return advance();
    }
    else
    {
      std::cout << errorMessage << std::endl;
      throw std::runtime_error(errorMessage);
    }
  }

  bool check(TokenType type, const std::string &expectedValue)
  {
    if (isAtEnd())
      return false;

    DEBUG_LOG("check() - Checking token: type=", static_cast<int>(peek().type),
              " value='", peek().value, "' against expected type=", static_cast<int>(type),
              " value='", expectedValue, "'");

    if (expectedValue != "")
    {
      bool typeMatch = peek().type == type;
      bool valueMatch = peek().value == expectedValue;
      DEBUG_LOG("check() - Type match: ", typeMatch, " Value match: ", valueMatch);
      return typeMatch && valueMatch;
    }
    else
    {
      bool typeMatch = peek().type == type;
      DEBUG_LOG("check() - Type match only: ", typeMatch);
      return typeMatch;
    }
  }

  Token advance()
  {
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

  Token peekNext() const
  {
    if (current + 1 < tokens.size())
    {
      return tokens[current + 1];
    }
    return tokens
        .back(); // Return the last token if there are no more tokens ahead
  }

  Token peekNextNext() const
  {
    if (current + 2 >= tokens.size())
    {
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
