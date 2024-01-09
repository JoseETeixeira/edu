#include <map>
#include <memory>
#include <string>
#include <vector>

class NodeVisitor {
public:
  virtual void visit(BinaryExpressionNode &node) = 0;
  virtual void visit(ProgramNode &node) = 0;
  virtual void visit(ClassNode &node) = 0;
  virtual void visit(FunctionNode &node) = 0;
  virtual void visit(VariableDeclarationNode &node) = 0;
  virtual void visit(StatementNode &node) = 0;
  virtual void visit(ExpressionNode &node) = 0;
  virtual void visit(ReturnStatementNode &node) = 0;
  virtual void visit(IfStatementNode &node) = 0;
  virtual void visit(ForStatementNode &node) = 0;
  virtual void visit(WhileStatementNode &node) = 0;
  virtual void visit(BreakStatementNode &node) = 0;
  virtual void visit(ContinueStatementNode &node) = 0;
  virtual void visit(BlockStatementNode &node) = 0;
  virtual void visit(CaseClauseNode &node) = 0;
  virtual void visit(SwitchStatementNode &node) = 0;
  virtual void visit(LiteralNode &node) = 0;
  virtual void visit(UnaryExpressionNode &node) = 0;
  virtual void visit(CallExpressionNode &node) = 0;
  virtual void visit(AssignmentExpressionNode &node) = 0;
  virtual void visit(MemberAccessExpressionNode &node) = 0;
  virtual void visit(ConditionalExpressionNode &node) = 0;
  virtual void visit(StringLiteralNode &node) = 0;
  virtual void visit(NumberLiteralNode &node) = 0;
  virtual void visit(BooleanLiteralNode &node) = 0;
  virtual void visit(NullLiteralNode &node) = 0;
  virtual void visit(ArrayLiteralNode &node) = 0;
  virtual void visit(ObjectLiteralNode &node) = 0;
  virtual void visit(TemplateLiteralNode &node) = 0;
  virtual void visit(TypeNode &node) = 0;
  virtual void visit(FunctionParameterNode &node) = 0;
  virtual void visit(ImportNode &node) = 0;
  virtual void visit(ExportNode &node) = 0;
  virtual void visit(TryCatchNode &node) = 0;
  virtual void visit(AsyncFunctionNode &node) = 0;
  virtual void visit(AwaitExpressionNode &node) = 0;
  virtual void visit(InterfaceNode &node) = 0;
  virtual void visit(NullReferenceNode &node) = 0;
  virtual void visit(ConsoleLogNode &node) = 0;
  virtual void visit(InputStatementNode &node) = 0;
  virtual void visit(VariableExpressionNode &node) = 0;
  virtual void visit(AndExpressionNode &node) = 0;
  virtual void visit(EqualityExpressionNode &node) = 0;
  virtual void visit(ComparisonExpressionNode &node) = 0;
  virtual void visit(AdditionExpressionNode &node) = 0;
  virtual void visit(SubtractionExpressionNode &node) = 0;
  virtual void visit(MultiplicationExpressionNode &node) = 0;
  virtual void visit(DivisionExpressionNode &node) = 0;
  virtual void visit(CharLiteralNode &node) = 0;
  virtual void visit(PropertyDeclarationNode &node) = 0;
  virtual void visit(ConstructorNode &node) = 0;
  virtual void visit(ExpressionStatementNode &node) = 0;
  virtual void visit(ErrorTypeNode &node) = 0;
  virtual void visit(FunctionExpressionNode &node) = 0;
  virtual void visit(FloatingPointLiteralNode &node) = 0;
  virtual void visit(IntegerLiteralNode &node) = 0;
  virtual void visit(CharacterLiteralNode &node) = 0;
};

class ASTNode {
public:
  ASTNode(int line) : line(line) {}
  virtual ~ASTNode() = default;

  int getLine() const { return line; }

  virtual void accept(NodeVisitor &visitor) = 0;

private:
  int line; // Line number in the source code
};

class ProgramNode : public ASTNode {
public:
  ProgramNode(int line) : ASTNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::vector<std::unique_ptr<ASTNode>> children;
};

class BlockStatementNode : public StatementNode {
public:
  BlockStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::vector<std::unique_ptr<StatementNode>> statements;
};

class ClassNode : public ASTNode {
public:
  ClassNode(const std::string &name, int line) : ASTNode(line), name(name) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> members;
};

class VariableDeclarationNode : public StatementNode {
public:
  VariableDeclarationNode(const std::string &name, int line)
      : StatementNode(line), name(name) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
  std::string name;
  std::unique_ptr<ExpressionNode> initializer;
};

class FunctionNode : public ASTNode {
public:
  FunctionNode(const std::string &name, int line) : ASTNode(line), name(name) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> parameters;
  std::unique_ptr<ASTNode> body;
};

class StatementNode : public ASTNode {
public:
  StatementNode(int line) : ASTNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
};

class ExpressionNode : public ASTNode {
public:
  ExpressionNode(int line) : ASTNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
};

class ReturnStatementNode : public StatementNode {
public:
  ReturnStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> expression;
};

class IfStatementNode : public StatementNode {
public:
  IfStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<StatementNode> thenBranch;
  std::unique_ptr<StatementNode> elseBranch;
};

class ForStatementNode : public StatementNode {
public:
  ForStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<StatementNode> initializer;
  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<ExpressionNode> increment;
  std::unique_ptr<StatementNode> body;
};

class WhileStatementNode : public StatementNode {
public:
  WhileStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<StatementNode> body;
};

class BreakStatementNode : public StatementNode {
public:
  BreakStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
};

class ContinueStatementNode : public StatementNode {
public:
  ContinueStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
};

class CaseClauseNode : public ASTNode {
public:
  CaseClauseNode(int line) : ASTNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> expression; // null for 'default' case
  std::vector<std::unique_ptr<StatementNode>> statements;
};

class SwitchStatementNode : public StatementNode {
public:
  SwitchStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> condition;
  std::vector<std::unique_ptr<CaseClauseNode>> cases;
};

class BinaryExpressionNode : public ExpressionNode {
public:
  BinaryExpressionNode(const std::string &op, int line)
      : ExpressionNode(line), op(op) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
  std::unique_ptr<ExpressionNode> left;
  std::unique_ptr<ExpressionNode> right;
  std::string op; // Operator
};

class LiteralNode : public ExpressionNode {
public:
  LiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(value) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string value;
};

class UnaryExpressionNode : public ExpressionNode {
public:
  UnaryExpressionNode(const std::string &op, int line)
      : ExpressionNode(line), op(op) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string op; // Operator, e.g., "-", "!"
  std::unique_ptr<ExpressionNode> operand;
};

class CallExpressionNode : public ExpressionNode {
public:
  CallExpressionNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> callee;
  std::vector<std::unique_ptr<ExpressionNode>> arguments;
};

class AssignmentExpressionNode : public ExpressionNode {
public:
  AssignmentExpressionNode(const std::string &op, int line)
      : ExpressionNode(line), op(op) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, e.g., "=", "+=", etc.
  std::unique_ptr<ExpressionNode> right;
};

class MemberAccessExpressionNode : public ExpressionNode {
public:
  MemberAccessExpressionNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> object;
  std::string memberName;
};

class ConditionalExpressionNode : public ExpressionNode {
public:
  ConditionalExpressionNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<ExpressionNode> trueExpr;
  std::unique_ptr<ExpressionNode> falseExpr;
};

class StringLiteralNode : public ExpressionNode {
public:
  StringLiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(value) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string value;
};

class NumberLiteralNode : public ExpressionNode {
public:
  NumberLiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(value) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string value; // Representing the numeric value as a string
};

class BooleanLiteralNode : public ExpressionNode {
public:
  BooleanLiteralNode(bool value, int line)
      : ExpressionNode(line), value(value) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  bool value;
};

class NullLiteralNode : public ExpressionNode {
public:
  NullLiteralNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
};

class ArrayLiteralNode : public ExpressionNode {
public:
  ArrayLiteralNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::vector<std::unique_ptr<ExpressionNode>> elements;
};

class ObjectLiteralNode : public ExpressionNode {
public:
  ObjectLiteralNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::map<std::string, std::unique_ptr<ExpressionNode>> properties;
};

class TemplateLiteralNode : public ExpressionNode {
public:
  TemplateLiteralNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::vector<std::unique_ptr<ExpressionNode>>
      parts; // Could be string literals and expressions
};

class TypeNode : public ASTNode {
public:
  TypeNode(const std::string &typeName, int line)
      : ASTNode(line), typeName(typeName) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string typeName;
};

class FunctionParameterNode : public ASTNode {
public:
  FunctionParameterNode(const std::string &name, int line)
      : ASTNode(line), name(name) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string name;
  std::unique_ptr<TypeNode> type; // If your language supports type annotations
};

class ImportNode : public ASTNode {
public:
  ImportNode(int line) : ASTNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string moduleName;
  std::vector<std::string> imports;
};

class ExportNode : public ASTNode {
public:
  ExportNode(int line) : ASTNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ASTNode> exportItem;
};

class TryCatchNode : public StatementNode {
public:
  TryCatchNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<StatementNode> tryBlock;
  std::string catchVariable;
  std::unique_ptr<StatementNode> catchBlock;
};

class EqualityExpressionNode : public ExpressionNode {
public:
  EqualityExpressionNode(std::unique_ptr<ExpressionNode> left,
                         const std::string &op,
                         std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

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

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

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

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, e.g., "&&"
  std::unique_ptr<ExpressionNode> right;

private:
};

class VariableExpressionNode : public ExpressionNode {
public:
  VariableExpressionNode(const std::string &name, int line)
      : ExpressionNode(line), name(name) {}

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string name; // The name of the variable
};

class AsyncFunctionNode : public FunctionNode {
public:
  AsyncFunctionNode(const std::string &name, int line)
      : FunctionNode(name, line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
};

class AwaitExpressionNode : public ExpressionNode {
public:
  AwaitExpressionNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> expression;
};

class InterfaceNode : public ASTNode {
public:
  InterfaceNode(const std::string &name, int line)
      : ASTNode(line), name(name) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string name;
  std::vector<std::unique_ptr<ASTNode>> members;
};

class NullReferenceNode : public ExpressionNode {
public:
  NullReferenceNode(int line) : ExpressionNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }
};

class ConsoleLogNode : public StatementNode {
public:
  ConsoleLogNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> expression;
};

class InputStatementNode : public StatementNode {
public:
  InputStatementNode(int line) : StatementNode(line) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<VariableDeclarationNode> variable;
};

class ComparisonExpressionNode : public ExpressionNode {
public:
  ComparisonExpressionNode(std::unique_ptr<ExpressionNode> left,
                           const std::string &op,
                           std::unique_ptr<ExpressionNode> right, int line)
      : ExpressionNode(line), left(std::move(left)), op(op),
        right(std::move(right)) {}

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

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

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

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

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

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

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

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

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<ExpressionNode> left;
  std::string op; // Operator, expected to be "/"
  std::unique_ptr<ExpressionNode> right;
};
class CharLiteralNode : public ExpressionNode {
public:
  // Constructor: takes the character value and the line number.
  CharLiteralNode(char value, int line) : ExpressionNode(line), value(value) {}

  // Accept method for the visitor pattern.
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  char value; // The character value of the literal.
};

class PropertyDeclarationNode : public StatementNode {
public:
  PropertyDeclarationNode(const std::string &name,
                          std::unique_ptr<TypeNode> type,
                          std::unique_ptr<ExpressionNode> initializer, int line)
      : StatementNode(line), name(name), type(std::move(type)),
        initializer(std::move(initializer)) {}

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string name;
  std::unique_ptr<TypeNode> type;
  std::unique_ptr<ExpressionNode> initializer;
};

class ConstructorNode : public ASTNode {
public:
  ConstructorNode(
      std::vector<std::unique_ptr<FunctionParameterNode>> parameters,
      std::unique_ptr<BlockStatementNode> body, int line)
      : ASTNode(line), parameters(std::move(parameters)),
        body(std::move(body)) {}

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::vector<std::unique_ptr<FunctionParameterNode>> parameters;
  std::unique_ptr<BlockStatementNode> body;
};

class ExpressionStatementNode : public StatementNode {
public:
  ExpressionStatementNode(std::unique_ptr<ExpressionNode> expression, int line)
      : StatementNode(line), expression(std::move(expression)) {}

  std::unique_ptr<ExpressionNode> expression;
};

class ErrorTypeNode : public ASTNode {
public:
  // Constructor: takes the variable name, message, error code, and line number.
  ErrorTypeNode(const std::string &varName, const std::string &message,
                const std::string &errorCode, int line)
      : ASTNode(line), varName(varName), message(message),
        errorCode(errorCode) {}

  // Accept method for the visitor pattern.
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::string varName;
  std::string message;
  std::string errorCode;
};

class FunctionExpressionNode : public ExpressionNode {
public:
  FunctionExpressionNode(std::unique_ptr<FunctionNode> function, int line)
      : ExpressionNode(line), function(std::move(function)) {}

  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  std::unique_ptr<FunctionNode> function;
};
class FloatingPointLiteralNode : public ExpressionNode {
public:
  FloatingPointLiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(std::stof(value)) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  float value;
};

class IntegerLiteralNode : public ExpressionNode {
public:
  IntegerLiteralNode(const std::string &value, int line)
      : ExpressionNode(line), value(std::stoi(value)) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  int value;
};

class CharacterLiteralNode : public ExpressionNode {
public:
  CharacterLiteralNode(char value, int line)
      : ExpressionNode(line), value(value) {}
  void accept(NodeVisitor &visitor) override { visitor.visit(*this); }

  char value;
};
