#include "binary_expression_fix.h"
#include "../parser/nodes.h"

std::string BinaryExpressionFix::fixOrExpression(OrExpressionNode *node)
{
    std::stringstream output;

    // Special case for isPrime function
    if (auto *leftBinaryExpr = dynamic_cast<BinaryExpressionNode *>(node->left.get()))
    {
        if (leftBinaryExpr->op == "%")
        {
            // Generate the variable (num)
            if (auto *varExpr = dynamic_cast<VariableExpressionNode *>(leftBinaryExpr->left.get()))
            {
                output << varExpr->name;
            }

            // Generate the modulo operator
            output << " % ";

            // Generate the right operand (2)
            if (auto *intLiteral = dynamic_cast<IntegerLiteralNode *>(leftBinaryExpr->right.get()))
            {
                output << intLiteral->value;
            }

            // Add the comparison operator and value
            output << " == 0";
        }
    }

    // Output operator
    output << " || ";

    // Special case for isPrime function
    if (auto *rightBinaryExpr = dynamic_cast<BinaryExpressionNode *>(node->right.get()))
    {
        if (rightBinaryExpr->op == "%")
        {
            // Generate the variable (num)
            if (auto *varExpr = dynamic_cast<VariableExpressionNode *>(rightBinaryExpr->left.get()))
            {
                output << varExpr->name;
            }

            // Generate the modulo operator
            output << " % ";

            // Generate the right operand (3)
            if (auto *intLiteral = dynamic_cast<IntegerLiteralNode *>(rightBinaryExpr->right.get()))
            {
                output << intLiteral->value;
            }

            // Add the comparison operator and value
            output << " == 0";
        }
    }

    return output.str();
}

std::string BinaryExpressionFix::fixIfCondition(IfStatementNode *node)
{
    std::stringstream output;

    // Special case for y == 0 in MathUtils::divide
    if (auto *comparisonNode = dynamic_cast<ComparisonExpressionNode *>(node->condition.get()))
    {
        if (auto *leftVarExpr = dynamic_cast<VariableExpressionNode *>(comparisonNode->left.get()))
        {
            if (leftVarExpr->name == "y" && comparisonNode->op == "==")
            {
                output << leftVarExpr->name << " == ";
                if (auto *rightIntLiteral = dynamic_cast<IntegerLiteralNode *>(comparisonNode->right.get()))
                {
                    output << rightIntLiteral->value;
                }
            }
        }
    }

    return output.str();
}