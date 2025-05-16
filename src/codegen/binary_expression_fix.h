// This file contains the missing implementations for binary expressions in the code generator

// Add these methods to the CodeGenerator class:

void generateBinaryExpression(BinaryExpressionNode *node)
{
    // Generate left operand
    if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->left.get()))
    {
        generateVariableExpression(varExprNode);
    }
    else if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->left.get()))
    {
        generateIntegerLiteral(intLiteralNode);
    }
    else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->left.get()))
    {
        generateFloatingPointLiteral(floatLiteralNode);
    }
    else if (auto *binaryExprNode = dynamic_cast<BinaryExpressionNode *>(node->left.get()))
    {
        generateBinaryExpression(binaryExprNode);
    }

    // Generate operator
    output << " " << node->op << " ";

    // Generate right operand
    if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->right.get()))
    {
        generateVariableExpression(varExprNode);
    }
    else if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->right.get()))
    {
        generateIntegerLiteral(intLiteralNode);
    }
    else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->right.get()))
    {
        generateFloatingPointLiteral(floatLiteralNode);
    }
    else if (auto *binaryExprNode = dynamic_cast<BinaryExpressionNode *>(node->right.get()))
    {
        generateBinaryExpression(binaryExprNode);
    }
}

// Modify the generateReturnStatement method to handle binary expressions:
void generateReturnStatement(ReturnStatementNode *node)
{
    output << "return";

    if (node->expression)
    {
        output << " ";
        if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->expression.get()))
        {
            generateIntegerLiteral(intLiteralNode);
        }
        else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->expression.get()))
        {
            generateFloatingPointLiteral(floatLiteralNode);
        }
        else if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->expression.get()))
        {
            generateStringLiteral(stringLiteralNode);
        }
        else if (auto *boolLiteralNode = dynamic_cast<BooleanLiteralNode *>(node->expression.get()))
        {
            generateBooleanLiteral(boolLiteralNode);
        }
        else if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->expression.get()))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->expression.get()))
        {
            generateCallExpression(callExprNode);
        }
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->expression.get()))
        {
            generateBinaryExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->expression.get()))
        {
            generateBinaryExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->expression.get()))
        {
            generateBinaryExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->expression.get()))
        {
            generateBinaryExpression(divExprNode);
        }
    }

    output << ";\n";
}

// Modify the generateVariableDeclaration method to properly handle string type:
void generateVariableDeclaration(VariableDeclarationNode *node)
{
    if (node->isConst)
    {
        output << "const ";
    }

    // Handle string type specifically
    if (node->typeName == "string")
    {
        output << "std::string";
    }
    else
    {
        output << node->typeName;
    }

    output << " " << node->name;

    if (node->initializer)
    {
        output << " = ";
        if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->initializer.get()))
        {
            generateIntegerLiteral(intLiteralNode);
        }
        else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->initializer.get()))
        {
            generateFloatingPointLiteral(floatLiteralNode);
        }
        else if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->initializer.get()))
        {
            generateStringLiteral(stringLiteralNode);
        }
        else if (auto *boolLiteralNode = dynamic_cast<BooleanLiteralNode *>(node->initializer.get()))
        {
            generateBooleanLiteral(boolLiteralNode);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->initializer.get()))
        {
            generateCallExpression(callExprNode);
        }
    }

    output << ";\n";
}

// Add proper handling for class instantiation in expression statements:
void generateExpressionStatement(ExpressionStatementNode *node)
{
    if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->expression.get()))
    {
        // Check if this is a class instantiation (constructor call)
        if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(callExprNode->callee.get()))
        {
            // If the first letter is uppercase, it's likely a class name
            if (isupper(varExprNode->name[0]))
            {
                // This is a class instantiation, so we need to declare a variable
                output << varExprNode->name << " ";
                // Generate a default variable name based on lowercase class name
                std::string varName = varExprNode->name;
                varName[0] = tolower(varName[0]);
                output << varName << " = ";
            }
        }
        generateCallExpression(callExprNode);
    }
    else if (auto *assignExprNode = dynamic_cast<AssignmentExpressionNode *>(node->expression.get()))
    {
        generateAssignmentExpression(assignExprNode);
    }

    output << ";\n";
}