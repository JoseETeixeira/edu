#pragma once

#include <string>
#include <sstream>

// Forward declarations
class OrExpressionNode;
class IfStatementNode;
class VariableExpressionNode;
class BinaryExpressionNode;
class IntegerLiteralNode;
class ComparisonExpressionNode;

// This file contains fixes for the binary expression generation in the code generator

class BinaryExpressionFix
{
public:
    static std::string fixOrExpression(OrExpressionNode *node);
    static std::string fixIfCondition(IfStatementNode *node);
};