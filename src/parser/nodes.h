#include <map>
#include <memory>
#include <string>
#include <vector>

class ASTNode {
public:
  ASTNode(int line) : line(line) {}
  virtual ~ASTNode() = default;

  int getLine() const { return line; }

private:
  int line; // Line number in the source code
};

class ProgramNode : public ASTNode {
public:
  ProgramNode(int line) : ASTNode(line) {}

  std::vector<std::unique_ptr<ASTNode>> children;
};

class StatementNode : public ASTNode {
public:
  StatementNode(int line) : ASTNode(line) {}
};
class TypeNode : public ASTNode {
public:
  TypeNode(const std::string &typeName, int line)
      : ASTNode(line), typeName(typeName) {}

  std::string typeName;
};

class FunctionParameterNode : public ASTNode {
public:
  FunctionParameterNode(const std::string &name, int line)
      : ASTNode(line), name(name) {}

  std::string name;
  std::unique_ptr<TypeNode> type; // If your language supports type annotations
};

class BlockStatementNode : public StatementNode {
public:
  BlockStatementNode(int line) : StatementNode(line) {}

  std::vector<std::unique_ptr<StatementNode>> statements;
};

class FunctionNode : public ASTNode {
public:
  FunctionNode(const std::string &name, int line) : ASTNode(line), name(name) {}

  std::string name;
  std::vector<std::unique_ptr<FunctionParameterNode>> parameters;
  std::string returnType;
  bool isAsync;
  std::unique_ptr<BlockStatementNode> body;
};

class ExpressionNode : public ASTNode {
public:
  ExpressionNode(int line) : ASTNode(line) {}
};

class ClassNode : public ASTNode {
public:
  ClassNode(const std::string &name, int line) : ASTNode(line), name(name) {}

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> members;
};

class CaseClauseNode : public ASTNode {
public:
  // Constructor for a case clause with an expression
  CaseClauseNode(std::unique_ptr<ExpressionNode> caseExpression,
                 std::vector<std::unique_ptr<StatementNode>> statements,
                 int line)
      : ASTNode(line), caseExpression(std::move(caseExpression)),
        statements(std::move(statements)), isDefault(false) {}

  // Constructor for a default case clause
  CaseClauseNode(std::vector<std::unique_ptr<StatementNode>> statements,
                 int line)
      : ASTNode(line), caseExpression(nullptr),
        statements(std::move(statements)), isDefault(true) {}

  std::unique_ptr<ExpressionNode> caseExpression;
  std::vector<std::unique_ptr<StatementNode>> statements;
  bool isDefault;
};

class ImportNode : public ASTNode {
public:
  ImportNode(int line) : ASTNode(line) {}

  std::string moduleName;
  std::vector<std::string> imports;
};

class ExportNode : public ASTNode {
public:
  ExportNode(int line) : ASTNode(line) {}

  std::unique_ptr<ASTNode> exportItem;
};

class InterfaceNode : public ASTNode {
public:
  InterfaceNode(const std::string &name, int line)
      : ASTNode(line), name(name) {}

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> members;
};

class ErrorTypeNode : public ASTNode {
public:
  // Constructor: takes the variable name, message, error code, and line number.
  ErrorTypeNode(const std::string &varName, const std::string &message,
                const std::string &errorCode, int line)
      : ASTNode(line), varName(varName), message(message),
        errorCode(errorCode) {}

  // Accept method for the visitor pattern.

  std::string varName;
  std::string message;
  std::string errorCode;
};

class ConstructorNode : public ASTNode {
public:
  ConstructorNode(
      std::vector<std::unique_ptr<FunctionParameterNode>> parameters,
      std::unique_ptr<BlockStatementNode> body, int line)
      : ASTNode(line), parameters(std::move(parameters)),
        body(std::move(body)) {}

  std::vector<std::unique_ptr<FunctionParameterNode>> parameters;
  std::unique_ptr<BlockStatementNode> body;
};

class VariableDeclarationNode : public StatementNode {
public:
  VariableDeclarationNode(const std::string &name, int line)
      : StatementNode(line), name(name) {}
  std::string name;
  std::unique_ptr<ExpressionNode> initializer;
  std::string typeName;
  bool isConst;
};

class ReturnStatementNode : public StatementNode {
public:
  ReturnStatementNode(int line) : StatementNode(line) {}

  std::unique_ptr<ExpressionNode> expression;
};

class IfStatementNode : public StatementNode {
public:
  IfStatementNode(int line) : StatementNode(line) {}

  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<ASTNode> thenBranch;
  std::unique_ptr<StatementNode> elseBranch;
};

class ForStatementNode : public StatementNode {
public:
  ForStatementNode(int line) : StatementNode(line) {}

  std::unique_ptr<StatementNode> initializer;
  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<ExpressionNode> increment;
  std::unique_ptr<ASTNode> body;
};

class WhileStatementNode : public StatementNode {
public:
  WhileStatementNode(int line) : StatementNode(line) {}

  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<ASTNode> body;
};

class BreakStatementNode : public StatementNode {
public:
  BreakStatementNode(int line) : StatementNode(line) {}
};

class ContinueStatementNode : public StatementNode {
public:
  ContinueStatementNode(int line) : StatementNode(line) {}
};

class SwitchStatementNode : public StatementNode {
public:
  SwitchStatementNode(int line) : StatementNode(line) {}

  std::unique_ptr<ExpressionNode> condition;
  std::vector<std::unique_ptr<CaseClauseNode>> cases;
};

class BinaryExpressionNode : public ExpressionNode {
public:
  BinaryExpressionNode(const std::string &op, int line)
      : ExpressionNode(line), op(op) {}
  std::unique_ptr<ExpressionNode> left;
  std::unique_ptr<ExpressionNode> right;
  std::string op; // Operator
};

class LiteralNode : public ExpressionNode {
public:
  LiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(value) {}

  std::string value;
};

class UnaryExpressionNode : public ExpressionNode {
public:
  UnaryExpressionNode(const std::string &op, int line)
      : ExpressionNode(line), op(op) {}

  std::string op; // Operator, e.g., "-", "!"
  std::unique_ptr<ExpressionNode> operand;
};

class CallExpressionNode : public ExpressionNode {
public:
  CallExpressionNode(int line) : ExpressionNode(line) {}

  std::unique_ptr<ExpressionNode> callee;
  std::vector<std::unique_ptr<ExpressionNode>> arguments;
};

class AssignmentExpressionNode : public ExpressionNode {
public:
  AssignmentExpressionNode(const std::string &op, int line)
      : ExpressionNode(line), op(op) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, e.g., "=", "+=", etc.
  std::unique_ptr<ExpressionNode> right;
};

class MemberAccessExpressionNode : public ExpressionNode {
public:
  MemberAccessExpressionNode(int line) : ExpressionNode(line) {}

  std::unique_ptr<ExpressionNode> object;
  std::string memberName;
};

class ConditionalExpressionNode : public ExpressionNode {
public:
  ConditionalExpressionNode(int line) : ExpressionNode(line) {}

  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<ExpressionNode> trueExpr;
  std::unique_ptr<ExpressionNode> falseExpr;
};

class StringLiteralNode : public ExpressionNode {
public:
  StringLiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(value) {}

  std::string value;
};

class NumberLiteralNode : public ExpressionNode {
public:
  NumberLiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(value) {}

  std::string value; // Representing the numeric value as a string
};

class BooleanLiteralNode : public ExpressionNode {
public:
  BooleanLiteralNode(bool value, int line)
      : ExpressionNode(line), value(value) {}

  bool value;
};

class NullLiteralNode : public ExpressionNode {
public:
  NullLiteralNode(int line) : ExpressionNode(line) {}
};

class ArrayLiteralNode : public ExpressionNode {
public:
  ArrayLiteralNode(int line) : ExpressionNode(line) {}

  std::vector<std::unique_ptr<ExpressionNode>> elements;
};

class ObjectLiteralNode : public ExpressionNode {
public:
  ObjectLiteralNode(int line) : ExpressionNode(line) {}

  std::vector<std::pair<std::string, std::unique_ptr<ExpressionNode>>>
      properties;
};

class TemplateLiteralNode : public ExpressionNode {
public:
  TemplateLiteralNode(int line) : ExpressionNode(line) {}

  std::vector<std::unique_ptr<ExpressionNode>>
      parts; // Could be string literals and expressions
};

class TryCatchNode : public StatementNode {
public:
  TryCatchNode(int line) : StatementNode(line) {}

  std::unique_ptr<StatementNode> tryBlock;
  std::unique_ptr<ErrorTypeNode> catchVariable;
  std::unique_ptr<BlockStatementNode> catchBlock;
};

class EqualityExpressionNode : public ExpressionNode {
public:
  EqualityExpressionNode(std::unique_ptr<ExpressionNode> left,
                         const std::string &op,
                         std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, e.g., "==" or "!="
  std::unique_ptr<ExpressionNode> right;

private:
};

class OrExpressionNode : public ExpressionNode {
public:
  OrExpressionNode(std::unique_ptr<ExpressionNode> left, const std::string &op,
                   std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, e.g., "||"
  std::unique_ptr<ExpressionNode> right;
};

class AndExpressionNode : public ExpressionNode {
public:
  AndExpressionNode(std::unique_ptr<ExpressionNode> left, const std::string &op,
                    std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, e.g., "&&"
  std::unique_ptr<ExpressionNode> right;

private:
};

class VariableExpressionNode : public ExpressionNode {
public:
  VariableExpressionNode(const std::string &name, int line)
      : ExpressionNode(line), name(name) {}

  std::string name; // The name of the variable
};

class AsyncFunctionNode : public FunctionNode {
public:
  AsyncFunctionNode(const std::string &name, int line)
      : FunctionNode(name, line) {}
};

class AwaitExpressionNode : public ExpressionNode {
public:
  AwaitExpressionNode(int line) : ExpressionNode(line) {}

  std::unique_ptr<ExpressionNode> expression;
};

class NullReferenceNode : public ExpressionNode {
public:
  NullReferenceNode(int line) : ExpressionNode(line) {}
};

class ConsoleLogNode : public StatementNode {
public:
  ConsoleLogNode(int line) : StatementNode(line) {}

  std::unique_ptr<ExpressionNode> expression;
};

class InputStatementNode : public StatementNode {
public:
  InputStatementNode(int line) : StatementNode(line) {}

  std::unique_ptr<VariableDeclarationNode> variable;
};

class ComparisonExpressionNode : public ExpressionNode {
public:
  ComparisonExpressionNode(std::unique_ptr<ExpressionNode> left,
                           const std::string &op,
                           std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, e.g., "<", ">", "<=", ">="
  std::unique_ptr<ExpressionNode> right;
};

class AdditionExpressionNode : public ExpressionNode {
public:
  AdditionExpressionNode(std::unique_ptr<ExpressionNode> left,
                         const std::string &op,
                         std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, expected to be "+"
  std::unique_ptr<ExpressionNode> right;
};

class SubtractionExpressionNode : public ExpressionNode {
public:
  SubtractionExpressionNode(std::unique_ptr<ExpressionNode> left,
                            const std::string &op,
                            std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, expected to be "-"
  std::unique_ptr<ExpressionNode> right;
};

class MultiplicationExpressionNode : public ExpressionNode {
public:
  MultiplicationExpressionNode(std::unique_ptr<ExpressionNode> left,
                               const std::string &op,
                               std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, expected to be "*"
  std::unique_ptr<ExpressionNode> right;
};

class DivisionExpressionNode : public ExpressionNode {
public:
  DivisionExpressionNode(std::unique_ptr<ExpressionNode> left,
                         const std::string &op,
                         std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, expected to be "/"
  std::unique_ptr<ExpressionNode> right;
};
class CharLiteralNode : public ExpressionNode {
public:
  // Constructor: takes the character value and the line number.
  CharLiteralNode(char value, int line) : ExpressionNode(line), value(value) {}

  // Accept method for the visitor pattern.
  char value; // The character value of the literal.
};

class PropertyDeclarationNode : public StatementNode {
public:
  PropertyDeclarationNode(const std::string &name,
                          std::unique_ptr<TypeNode> type,
                          std::unique_ptr<ExpressionNode> initializer, int line)
      : StatementNode(line), name(name), type(std::move(type)),
        initializer(std::move(initializer)) {}

  std::string name;
  std::unique_ptr<TypeNode> type;
  std::unique_ptr<ExpressionNode> initializer;
};

class ExpressionStatementNode : public StatementNode {
public:
  ExpressionStatementNode(std::unique_ptr<ExpressionNode> expression, int line)
      : StatementNode(line), expression(std::move(expression)) {}

  std::unique_ptr<ExpressionNode> expression;
};

class FunctionExpressionNode : public ExpressionNode {
public:
  FunctionExpressionNode(std::unique_ptr<FunctionNode> function, int line)
      : ExpressionNode(line), function(std::move(function)) {}

  std::unique_ptr<FunctionNode> function;
};
class FloatingPointLiteralNode : public ExpressionNode {
public:
  FloatingPointLiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(std::stof(value)) {}

  float value;
};

class IntegerLiteralNode : public ExpressionNode {
public:
  IntegerLiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(std::stoi(value)) {}

  int value;
};

class CharacterLiteralNode : public ExpressionNode {
public:
  CharacterLiteralNode(char value, int line)
      : ExpressionNode(line), value(value) {}

  char value;
};
class TemplateNode : public ASTNode {
public:
  TemplateNode(std::vector<std::string> params,
               std::unique_ptr<ASTNode> declaration, int line)
      : ASTNode(line), // Pass the line number to the ASTNode constructor
        params(std::move(params)), declaration(std::move(declaration)) {}

  std::vector<std::string> params;
  std::unique_ptr<ASTNode> declaration;
};
