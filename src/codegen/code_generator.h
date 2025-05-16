#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <set>
#include <vector>

// Forward declarations
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

class CodeGenerator
{
public:
    CodeGenerator() : indentLevel(0) {}

    // Generate C++ code from the AST
    std::string generate(ProgramNode *program)
    {
        output.str("");
        generateProgram(program);
        return output.str();
    }

private:
    std::stringstream output;
    int indentLevel;
    std::string currentType;
    bool hasMainFunction = false;

    void outputIndent()
    {
        for (int i = 0; i < indentLevel; ++i)
        {
            output << "    "; // 4 spaces per indent level
        }
    }

    void generateProgram(ProgramNode *node)
    {
        // Add standard includes
        output << "#include <iostream>\n";
        output << "#include <string>\n";
        output << "#include <vector>\n";
        output << "#include <memory>\n";
        output << "#include <functional>\n\n";

        // Process all children nodes
        for (const auto &child : node->children)
        {
            if (auto *classNode = dynamic_cast<ClassNode *>(child.get()))
            {
                generateClass(classNode);
            }
            else if (auto *functionNode = dynamic_cast<FunctionNode *>(child.get()))
            {
                generateFunction(functionNode);
            }
            else if (auto *varDeclNode = dynamic_cast<VariableDeclarationNode *>(child.get()))
            {
                generateVariableDeclaration(varDeclNode);
            }
            else if (auto *interfaceNode = dynamic_cast<InterfaceNode *>(child.get()))
            {
                generateInterface(interfaceNode);
            }
            else if (auto *exportNode = dynamic_cast<ExportNode *>(child.get()))
            {
                // Handle export node
            }
            else if (auto *importNode = dynamic_cast<ImportNode *>(child.get()))
            {
                // Handle import node
            }
            else if (auto *templateNode = dynamic_cast<TemplateNode *>(child.get()))
            {
                // Handle template node
            }
            // Add more node types as needed
        }

        // Add main function if not already defined
        if (!hasMainFunction)
        {
            output << "int main() {\n";
            output << "    // Auto-generated main function\n";
            output << "    return 0;\n";
            output << "}\n";
        }
    }

    void generateType(TypeNode *node)
    {
        // Map edu types to C++ types
        std::map<std::string, std::string> typeMap = {
            {"bool", "bool"},
            {"char", "char"},
            {"int", "int"},
            {"float", "float"},
            {"double", "double"},
            {"void", "void"},
            {"wchar_t", "wchar_t"},
            {"string", "std::string"},
            {"Error", "std::runtime_error"}};

        // Check if the type is in the map
        if (typeMap.find(node->typeName) != typeMap.end())
        {
            currentType = typeMap[node->typeName];
        }
        else
        {
            // Assume it's a user-defined type
            currentType = node->typeName;
        }
    }

    void generateFunctionParameter(FunctionParameterNode *node)
    {
        if (node->type)
        {
            generateType(node->type.get());
            output << currentType << " " << node->name;
        }
        else
        {
            output << "auto " << node->name;
        }
    }

    void generateBlockStatement(BlockStatementNode *node)
    {
        output << " {\n";
        indentLevel++;

        for (const auto &statement : node->statements)
        {
            outputIndent();
            if (auto *varDeclNode = dynamic_cast<VariableDeclarationNode *>(statement.get()))
            {
                generateVariableDeclaration(varDeclNode);
            }
            else if (auto *returnNode = dynamic_cast<ReturnStatementNode *>(statement.get()))
            {
                generateReturnStatement(returnNode);
            }
            else if (auto *ifNode = dynamic_cast<IfStatementNode *>(statement.get()))
            {
                // Handle if statement
            }
            else if (auto *forNode = dynamic_cast<ForStatementNode *>(statement.get()))
            {
                // Handle for statement
            }
            else if (auto *whileNode = dynamic_cast<WhileStatementNode *>(statement.get()))
            {
                // Handle while statement
            }
            else if (auto *consoleLogNode = dynamic_cast<ConsoleLogNode *>(statement.get()))
            {
                generateConsoleLog(consoleLogNode);
            }
            else if (auto *exprStmtNode = dynamic_cast<ExpressionStatementNode *>(statement.get()))
            {
                generateExpressionStatement(exprStmtNode);
            }
            // Add more statement types as needed
        }

        indentLevel--;
        outputIndent();
        output << "}";
    }

    void generateFunction(FunctionNode *node)
    {
        // Check if this is the main function
        if (node->name == "main")
        {
            hasMainFunction = true;
            // Always use int for main function
            output << "int ";
        }
        else
        {
            // Handle async functions
            if (node->isAsync)
            {
                output << "std::future<";
                if (node->returnType.empty())
                {
                    output << "void";
                }
                else
                {
                    output << node->returnType;
                }
                output << "> ";
            }
            else
            {
                // Output return type
                if (node->returnType.empty())
                {
                    output << "void ";
                }
                else
                {
                    output << node->returnType << " ";
                }
            }
        }

        // Output function name and parameters
        output << node->name << "(";

        for (size_t i = 0; i < node->parameters.size(); ++i)
        {
            generateFunctionParameter(node->parameters[i].get());
            if (i < node->parameters.size() - 1)
            {
                output << ", ";
            }
        }

        output << ")";

        // Output function body
        if (node->body)
        {
            generateBlockStatement(node->body.get());
        }
        else
        {
            output << " {}";
        }

        output << "\n\n";
    }

    void generateClass(ClassNode *node)
    {
        output << "class " << node->name;

        // TODO: Handle inheritance

        output << " {\n";
        output << "public:\n";
        indentLevel++;

        // Process class members
        for (const auto &member : node->members)
        {
            outputIndent();
            if (auto *functionNode = dynamic_cast<FunctionNode *>(member.get()))
            {
                // For methods, we need to prefix with the class name
                std::string returnType = functionNode->returnType.empty() ? "void" : functionNode->returnType;
                output << returnType << " " << functionNode->name << "(";

                for (size_t i = 0; i < functionNode->parameters.size(); ++i)
                {
                    generateFunctionParameter(functionNode->parameters[i].get());
                    if (i < functionNode->parameters.size() - 1)
                    {
                        output << ", ";
                    }
                }

                output << ")";

                if (functionNode->body)
                {
                    generateBlockStatement(functionNode->body.get());
                }
                else
                {
                    output << " {}";
                }

                output << "\n";
            }
            else if (auto *constructorNode = dynamic_cast<ConstructorNode *>(member.get()))
            {
                // For constructors
                output << node->name << "(";

                for (size_t i = 0; i < constructorNode->parameters.size(); ++i)
                {
                    generateFunctionParameter(constructorNode->parameters[i].get());
                    if (i < constructorNode->parameters.size() - 1)
                    {
                        output << ", ";
                    }
                }

                output << ")";

                if (constructorNode->body)
                {
                    generateBlockStatement(constructorNode->body.get());
                }
                else
                {
                    output << " {}";
                }

                output << "\n";
            }
            else if (auto *propertyNode = dynamic_cast<PropertyDeclarationNode *>(member.get()))
            {
                generatePropertyDeclaration(propertyNode);
            }
            // Add more member types as needed
        }

        indentLevel--;
        output << "};\n\n";
    }

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
            else if (auto *charLiteralNode = dynamic_cast<CharLiteralNode *>(node->initializer.get()))
            {
                generateCharLiteral(charLiteralNode);
            }
            else if (auto *nullLiteralNode = dynamic_cast<NullLiteralNode *>(node->initializer.get()))
            {
                generateNullLiteral(nullLiteralNode);
            }
            else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->initializer.get()))
            {
                generateCallExpression(callExprNode);
            }
            // Add more initializer types as needed
        }

        output << ";\n";
    }

    void generateAdditionExpression(AdditionExpressionNode *node)
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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->left.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->left.get()))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->left.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->left.get()))
        {
            generateDivisionExpression(divExprNode);
        }

        // Generate operator
        output << " + ";

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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->right.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->right.get()))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->right.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->right.get()))
        {
            generateDivisionExpression(divExprNode);
        }
    }

    void generateSubtractionExpression(SubtractionExpressionNode *node)
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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->left.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->left.get()))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->left.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->left.get()))
        {
            generateDivisionExpression(divExprNode);
        }

        // Generate operator
        output << " - ";

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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->right.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->right.get()))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->right.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->right.get()))
        {
            generateDivisionExpression(divExprNode);
        }
    }

    void generateMultiplicationExpression(MultiplicationExpressionNode *node)
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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->left.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->left.get()))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->left.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->left.get()))
        {
            generateDivisionExpression(divExprNode);
        }

        // Generate operator
        output << " * ";

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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->right.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->right.get()))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->right.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->right.get()))
        {
            generateDivisionExpression(divExprNode);
        }
    }

    void generateDivisionExpression(DivisionExpressionNode *node)
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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->left.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->left.get()))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->left.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->left.get()))
        {
            generateDivisionExpression(divExprNode);
        }

        // Generate operator
        output << " / ";

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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->right.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->right.get()))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->right.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->right.get()))
        {
            generateDivisionExpression(divExprNode);
        }
    }

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
                generateAdditionExpression(addExprNode);
            }
            else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->expression.get()))
            {
                generateSubtractionExpression(subExprNode);
            }
            else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->expression.get()))
            {
                generateMultiplicationExpression(mulExprNode);
            }
            else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->expression.get()))
            {
                generateDivisionExpression(divExprNode);
            }
            // Add more expression types as needed
        }

        output << ";\n";
    }

    void generateConsoleLog(ConsoleLogNode *node)
    {
        output << "std::cout << ";

        if (node->expression)
        {
            if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->expression.get()))
            {
                generateStringLiteral(stringLiteralNode);
            }
            else if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->expression.get()))
            {
                generateIntegerLiteral(intLiteralNode);
            }
            else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->expression.get()))
            {
                generateFloatingPointLiteral(floatLiteralNode);
            }
            else if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->expression.get()))
            {
                generateVariableExpression(varExprNode);
            }
            else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->expression.get()))
            {
                generateCallExpression(callExprNode);
            }
            // Add more expression types as needed
        }

        output << " << std::endl;\n";
    }

    void generateStringLiteral(StringLiteralNode *node)
    {
        output << "\"" << node->value << "\"";
    }

    void generateIntegerLiteral(IntegerLiteralNode *node)
    {
        output << node->value;
    }

    void generateFloatingPointLiteral(FloatingPointLiteralNode *node)
    {
        output << node->value;
    }

    void generateBooleanLiteral(BooleanLiteralNode *node)
    {
        output << (node->value ? "true" : "false");
    }

    void generateCharLiteral(CharLiteralNode *node)
    {
        output << "'" << node->value << "'";
    }

    void generateNullLiteral(NullLiteralNode *node)
    {
        output << "nullptr";
    }

    void generateVariableExpression(VariableExpressionNode *node)
    {
        output << node->name;
    }

    void generateCallExpression(CallExpressionNode *node)
    {
        if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->callee.get()))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *memberAccessNode = dynamic_cast<MemberAccessExpressionNode *>(node->callee.get()))
        {
            generateMemberAccessExpression(memberAccessNode);
        }

        output << "(";

        for (size_t i = 0; i < node->arguments.size(); ++i)
        {
            if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->arguments[i].get()))
            {
                generateIntegerLiteral(intLiteralNode);
            }
            else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->arguments[i].get()))
            {
                generateFloatingPointLiteral(floatLiteralNode);
            }
            else if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->arguments[i].get()))
            {
                generateStringLiteral(stringLiteralNode);
            }
            else if (auto *boolLiteralNode = dynamic_cast<BooleanLiteralNode *>(node->arguments[i].get()))
            {
                generateBooleanLiteral(boolLiteralNode);
            }
            else if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->arguments[i].get()))
            {
                generateVariableExpression(varExprNode);
            }

            if (i < node->arguments.size() - 1)
            {
                output << ", ";
            }
        }

        output << ")";
    }

    void generateMemberAccessExpression(MemberAccessExpressionNode *node)
    {
        if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->object.get()))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->object.get()))
        {
            generateCallExpression(callExprNode);
        }
        else if (auto *memberAccessNode = dynamic_cast<MemberAccessExpressionNode *>(node->object.get()))
        {
            generateMemberAccessExpression(memberAccessNode);
        }

        output << "." << node->memberName;
    }

    void generateExpressionStatement(ExpressionStatementNode *node)
    {
        // Skip null expressions (which would generate empty statements)
        if (auto *nullLiteralNode = dynamic_cast<NullLiteralNode *>(node->expression.get()))
        {
            return;
        }

        if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->expression.get()))
        {
            // Check if this is a class instantiation (constructor call)
            if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(callExprNode->callee.get()))
            {
                // If the first letter is uppercase, it's likely a class name
                if (!varExprNode->name.empty() && isupper(varExprNode->name[0]))
                {
                    // This is a class instantiation, so we need to declare a variable
                    output << varExprNode->name << " ";
                    // Generate a default variable name based on lowercase class name
                    std::string varName = varExprNode->name;
                    if (!varName.empty())
                    {
                        varName[0] = tolower(varName[0]);
                    }
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

    void generateAssignmentExpression(AssignmentExpressionNode *node)
    {
        if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->left.get()))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *memberAccessNode = dynamic_cast<MemberAccessExpressionNode *>(node->left.get()))
        {
            generateMemberAccessExpression(memberAccessNode);
        }

        output << " " << node->op << " ";

        if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->right.get()))
        {
            generateIntegerLiteral(intLiteralNode);
        }
        else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->right.get()))
        {
            generateFloatingPointLiteral(floatLiteralNode);
        }
        else if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->right.get()))
        {
            generateStringLiteral(stringLiteralNode);
        }
        else if (auto *boolLiteralNode = dynamic_cast<BooleanLiteralNode *>(node->right.get()))
        {
            generateBooleanLiteral(boolLiteralNode);
        }
        else if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->right.get()))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->right.get()))
        {
            generateCallExpression(callExprNode);
        }
    }

    void generatePropertyDeclaration(PropertyDeclarationNode *node)
    {
        if (node->type)
        {
            generateType(node->type.get());
            output << currentType << " " << node->name;
        }
        else
        {
            output << "auto " << node->name;
        }

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
        }

        output << ";\n";
    }

    void generateInterface(InterfaceNode *node)
    {
        // In C++, interfaces are typically implemented as abstract classes
        output << "class " << node->name << " {\n";
        output << "public:\n";
        indentLevel++;

        // Add a virtual destructor
        outputIndent();
        output << "virtual ~" << node->name << "() = default;\n";

        // Process interface members
        for (const auto &member : node->members)
        {
            outputIndent();
            if (auto *functionNode = dynamic_cast<FunctionNode *>(member.get()))
            {
                // For methods, make them pure virtual
                std::string returnType = functionNode->returnType.empty() ? "void" : functionNode->returnType;
                output << "virtual " << returnType << " " << functionNode->name << "(";

                for (size_t i = 0; i < functionNode->parameters.size(); ++i)
                {
                    generateFunctionParameter(functionNode->parameters[i].get());
                    if (i < functionNode->parameters.size() - 1)
                    {
                        output << ", ";
                    }
                }

                output << ") = 0;\n";
            }
            else if (auto *propertyNode = dynamic_cast<PropertyDeclarationNode *>(member.get()))
            {
                // For properties, add getter and setter methods
                if (propertyNode->type)
                {
                    generateType(propertyNode->type.get());
                    std::string propType = currentType;

                    // Getter
                    output << "virtual " << propType << " get" << propertyNode->name << "() const = 0;\n";

                    // Setter
                    outputIndent();
                    output << "virtual void set" << propertyNode->name << "(" << propType << " value) = 0;\n";
                }
            }
        }

        indentLevel--;
        output << "};\n\n";
    }
};