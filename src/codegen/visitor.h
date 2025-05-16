#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <map>
#include "visitor_forward.h"

// Forward declarations of all node types
class ProgramNode;
class StatementNode;
class TypeNode;
class FunctionParameterNode;
class BlockStatementNode;
class FunctionNode;
class ExpressionNode;
class ClassNode;
class CaseClauseNode;
class ImportNode;
class ExportNode;
class InterfaceNode;
class ErrorTypeNode;
class ConstructorNode;
class VariableDeclarationNode;
class ReturnStatementNode;
class IfStatementNode;
class ForStatementNode;
class WhileStatementNode;
class BreakStatementNode;
class ContinueStatementNode;
class SwitchStatementNode;
class BinaryExpressionNode;
class LiteralNode;
class UnaryExpressionNode;
class CallExpressionNode;
class AssignmentExpressionNode;
class MemberAccessExpressionNode;
class ConditionalExpressionNode;
class StringLiteralNode;
class NumberLiteralNode;
class BooleanLiteralNode;
class NullLiteralNode;
class ArrayLiteralNode;
class ObjectLiteralNode;
class TemplateLiteralNode;
class TryCatchNode;
class EqualityExpressionNode;
class OrExpressionNode;
class AndExpressionNode;
class VariableExpressionNode;
class AsyncFunctionNode;
class AwaitExpressionNode;
class NullReferenceNode;
class ConsoleLogNode;
class InputStatementNode;
class ComparisonExpressionNode;
class AdditionExpressionNode;
class SubtractionExpressionNode;
class MultiplicationExpressionNode;
class DivisionExpressionNode;
class CharLiteralNode;
class PropertyDeclarationNode;
class ExpressionStatementNode;
class IntegerLiteralNode;
class FloatingPointLiteralNode;
class FunctionExpressionNode;
class TemplateNode;

// Visitor interface
class Visitor
{
public:
    virtual ~Visitor() = default;

    // Visit methods for each node type
    virtual void visit(ProgramNode *node) = 0;
    virtual void visit(TypeNode *node) = 0;
    virtual void visit(FunctionParameterNode *node) = 0;
    virtual void visit(BlockStatementNode *node) = 0;
    virtual void visit(FunctionNode *node) = 0;
    virtual void visit(ClassNode *node) = 0;
    virtual void visit(CaseClauseNode *node) = 0;
    virtual void visit(ImportNode *node) = 0;
    virtual void visit(ExportNode *node) = 0;
    virtual void visit(InterfaceNode *node) = 0;
    virtual void visit(ErrorTypeNode *node) = 0;
    virtual void visit(ConstructorNode *node) = 0;
    virtual void visit(VariableDeclarationNode *node) = 0;
    virtual void visit(ReturnStatementNode *node) = 0;
    virtual void visit(IfStatementNode *node) = 0;
    virtual void visit(ForStatementNode *node) = 0;
    virtual void visit(WhileStatementNode *node) = 0;
    virtual void visit(BreakStatementNode *node) = 0;
    virtual void visit(ContinueStatementNode *node) = 0;
    virtual void visit(SwitchStatementNode *node) = 0;
    virtual void visit(UnaryExpressionNode *node) = 0;
    virtual void visit(CallExpressionNode *node) = 0;
    virtual void visit(AssignmentExpressionNode *node) = 0;
    virtual void visit(MemberAccessExpressionNode *node) = 0;
    virtual void visit(ConditionalExpressionNode *node) = 0;
    virtual void visit(StringLiteralNode *node) = 0;
    virtual void visit(BooleanLiteralNode *node) = 0;
    virtual void visit(NullLiteralNode *node) = 0;
    virtual void visit(ArrayLiteralNode *node) = 0;
    virtual void visit(ObjectLiteralNode *node) = 0;
    virtual void visit(TemplateLiteralNode *node) = 0;
    virtual void visit(TryCatchNode *node) = 0;
    virtual void visit(EqualityExpressionNode *node) = 0;
    virtual void visit(OrExpressionNode *node) = 0;
    virtual void visit(AndExpressionNode *node) = 0;
    virtual void visit(VariableExpressionNode *node) = 0;
    virtual void visit(AsyncFunctionNode *node) = 0;
    virtual void visit(AwaitExpressionNode *node) = 0;
    virtual void visit(NullReferenceNode *node) = 0;
    virtual void visit(ConsoleLogNode *node) = 0;
    virtual void visit(InputStatementNode *node) = 0;
    virtual void visit(ComparisonExpressionNode *node) = 0;
    virtual void visit(AdditionExpressionNode *node) = 0;
    virtual void visit(SubtractionExpressionNode *node) = 0;
    virtual void visit(MultiplicationExpressionNode *node) = 0;
    virtual void visit(DivisionExpressionNode *node) = 0;
    virtual void visit(CharLiteralNode *node) = 0;
    virtual void visit(PropertyDeclarationNode *node) = 0;
    virtual void visit(ExpressionStatementNode *node) = 0;
    virtual void visit(IntegerLiteralNode *node) = 0;
    virtual void visit(FloatingPointLiteralNode *node) = 0;
    virtual void visit(FunctionExpressionNode *node) = 0;
    virtual void visit(TemplateNode *node) = 0;
};

// Now, let's add accept methods to each node class in nodes.h
// This would require modifying the nodes.h file to add:
// virtual void accept(Visitor* visitor) { visitor->visit(this); }
// to each node class.