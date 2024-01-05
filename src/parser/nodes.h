#include <map>
#include <memory>
#include <string>
#include <vector>

class ASTNode {
public:
  ASTNode(int line) : line(line) {}
  virtual ~ASTNode() = default;

  int getLine() const { return line; }

  // You can add more common methods here, if necessary.

private:
  int line; // Line number in the source code
};

class ProgramNode : public ASTNode {
public:
  ProgramNode(int line) : ASTNode(line) {}

  std::vector<std::unique_ptr<ASTNode>> children;
};

class ClassNode : public ASTNode {
public:
  ClassNode(const std::string &name, int line) : ASTNode(line), name(name) {}

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> members;
};

class FunctionNode : public ASTNode {
public:
  FunctionNode(const std::string &name, int line) : ASTNode(line), name(name) {}

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> parameters;
  std::unique_ptr<ASTNode> body;
};

class StatementNode : public ASTNode {
public:
  StatementNode(int line) : ASTNode(line) {}
};

class ExpressionNode : public ASTNode {
public:
  ExpressionNode(int line) : ASTNode(line) {}
};

class VariableDeclarationNode : public StatementNode {
public:
  VariableDeclarationNode(const std::string &name, int line)
      : StatementNode(line), name(name) {}

  std::string name;
  std::unique_ptr<ExpressionNode> initializer;
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
  std::unique_ptr<StatementNode> thenBranch;
  std::unique_ptr<StatementNode> elseBranch;
};

class ForStatementNode : public StatementNode {
public:
  ForStatementNode(int line) : StatementNode(line) {}

  std::unique_ptr<StatementNode> initializer;
  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<ExpressionNode> increment;
  std::unique_ptr<StatementNode> body;
};

class WhileStatementNode : public StatementNode {
public:
  WhileStatementNode(int line) : StatementNode(line) {}

  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<StatementNode> body;
};

class BreakStatementNode : public StatementNode {
public:
  BreakStatementNode(int line) : StatementNode(line) {}
};

class ContinueStatementNode : public StatementNode {
public:
  ContinueStatementNode(int line) : StatementNode(line) {}
};

class CaseClauseNode : public ASTNode {
public:
  CaseClauseNode(int line) : ASTNode(line) {}

  std::unique_ptr<ExpressionNode> expression; // null for 'default' case
  std::vector<std::unique_ptr<StatementNode>> statements;
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
  std::map<std::string, std::unique_ptr<ExpressionNode>> properties;
};

class TemplateLiteralNode : public ExpressionNode {
public:
  TemplateLiteralNode(int line) : ExpressionNode(line) {}
  std::vector<std::unique_ptr<ExpressionNode>>
      parts; // Could be string literals and expressions
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

class TryCatchNode : public StatementNode {
public:
  TryCatchNode(int line) : StatementNode(line) {}
  std::unique_ptr<StatementNode> tryBlock;
  std::string catchVariable;
  std::unique_ptr<StatementNode> catchBlock;
};

class AsyncFunctionNode : public FunctionNode {
public:
  AsyncFunctionNode(const std::string &name, int line)
      : FunctionNode(name, line) {}

  // Additional properties or methods can be added here.
  // For example, you might want to track whether this function uses the 'await'
  // keyword, or you might want to store information about how the function
  // handles concurrency.
};

class AwaitExpressionNode : public ExpressionNode {
public:
  AwaitExpressionNode(int line) : ExpressionNode(line) {}
  std::unique_ptr<ExpressionNode> expression;
};

class InterfaceNode : public ASTNode {
public:
  InterfaceNode(const std::string &name, int line)
      : ASTNode(line), name(name) {}

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> members;
};
