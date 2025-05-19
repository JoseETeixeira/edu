#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <regex>
#include "binary_expression_fix.h"

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

        // Apply post-processing fix for string concatenation
        std::string code = output.str();
        return fixStringConcatenation(code);
    }

private:
    std::stringstream output;
    int indentLevel;
    std::string currentType;
    bool hasMainFunction = false;
    std::set<std::string> declaredVariables; // Track declared variables
    std::map<std::string, std::string> functionReturnTypes;

    bool isBooleanReturningFunction(const std::string &functionName)
    {
        auto it = functionReturnTypes.find(functionName);
        return (it != functionReturnTypes.end() && it->second == "bool");
    }

    void outputIndent()
    {
        for (int i = 0; i < indentLevel; ++i)
        {
            output << "    "; // 4 spaces per indent level
        }
    }

    // Add this method to your CodeGenerator class
    void generateOrExpressionDirect(OrExpressionNode *node)
    {
        if (!node)
            return;

        // Handle left side
        if (auto *leftEq = dynamic_cast<EqualityExpressionNode *>(node->left.get()))
        {
            // Check if left side of equality is a binary expression (like num % 2)
            if (auto *leftBinary = dynamic_cast<BinaryExpressionNode *>(leftEq->left.get()))
            {
                // Generate the binary expression (num % 2)
                generateExpressionHelper(leftBinary->left.get());  // num
                output << " " << leftBinary->op << " ";            // %
                generateExpressionHelper(leftBinary->right.get()); // 2

                // Add the equality operator and right side
                output << " " << leftEq->op << " ";            // ==
                generateExpressionHelper(leftEq->right.get()); // 0
            }
            else
            {
                // Handle other equality expressions normally
                generateEqualityExpression(leftEq);
            }
        }
        else
        {
            // Handle non-equality expressions
            generateExpressionHelper(node->left.get());
        }

        // Output the OR operator
        output << " || ";

        // Handle right side
        if (auto *rightEq = dynamic_cast<EqualityExpressionNode *>(node->right.get()))
        {
            // Check if left side of equality is a binary expression (like num % 3)
            if (auto *rightBinary = dynamic_cast<BinaryExpressionNode *>(rightEq->left.get()))
            {
                // Generate the binary expression (num % 3)
                generateExpressionHelper(rightBinary->left.get());  // num
                output << " " << rightBinary->op << " ";            // %
                generateExpressionHelper(rightBinary->right.get()); // 3

                // Add the equality operator and right side
                output << " " << rightEq->op << " ";            // ==
                generateExpressionHelper(rightEq->right.get()); // 0
            }
            else
            {
                // Handle other equality expressions normally
                generateEqualityExpression(rightEq);
            }
        }
        else
        {
            // Handle non-equality expressions
            generateExpressionHelper(node->right.get());
        }
    }
    std::string fixStringConcatenation(const std::string &code)
    {
        std::string result = code;

        // Only fix string concatenation in print statements
        std::regex pattern(R"((".*?") \+ ([^<]+) << std::endl)");
        result = std::regex_replace(result, pattern, "$1 << $2 << std::endl");

        return result;
    }

    void generateProgram(ProgramNode *node)
    {
        // Add standard includes
        output << "#include <iostream>\n";
        output << "#include <string>\n";
        output << "#include <vector>\n";
        output << "#include <memory>\n";
        output << "#include <functional>\n\n";

        // First pass: declare global variables
        for (const auto &child : node->children)
        {
            if (auto *varDeclNode = dynamic_cast<VariableDeclarationNode *>(child.get()))
            {
                generateVariableDeclaration(varDeclNode);
                output << "\n";
            }
        }

        // Second pass: generate classes, functions, etc.
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
            // Skip variable declarations as they're already handled
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

    void generateExpressionHelper(ExpressionNode *expr)
    {
        if (!expr)
            return;

        if (auto *varExpr = dynamic_cast<VariableExpressionNode *>(expr))
        {
            generateVariableExpression(varExpr);
        }
        else if (auto *callExpr = dynamic_cast<CallExpressionNode *>(expr))
        {
            generateCallExpression(callExpr);
        }
        else if (auto *memberAccessExpr = dynamic_cast<MemberAccessExpressionNode *>(expr))
        {
            generateMemberAccessExpression(memberAccessExpr);
        }
        else if (auto *intLiteral = dynamic_cast<IntegerLiteralNode *>(expr))
        {
            generateIntegerLiteral(intLiteral);
        }
        else if (auto *floatLiteral = dynamic_cast<FloatingPointLiteralNode *>(expr))
        {
            generateFloatingPointLiteral(floatLiteral);
        }
        else if (auto *stringLiteral = dynamic_cast<StringLiteralNode *>(expr))
        {
            generateStringLiteral(stringLiteral);
        }
        else if (auto *boolLiteral = dynamic_cast<BooleanLiteralNode *>(expr))
        {
            generateBooleanLiteral(boolLiteral);
        }
        else if (auto *charLiteral = dynamic_cast<CharLiteralNode *>(expr))
        {
            generateCharLiteral(charLiteral);
        }
        else if (auto *addExpr = dynamic_cast<AdditionExpressionNode *>(expr))
        {
            generateAdditionExpression(addExpr);
        }
        else if (auto *subExpr = dynamic_cast<SubtractionExpressionNode *>(expr))
        {
            generateSubtractionExpression(subExpr);
        }
        else if (auto *mulExpr = dynamic_cast<MultiplicationExpressionNode *>(expr))
        {
            generateMultiplicationExpression(mulExpr);
        }
        else if (auto *divExpr = dynamic_cast<DivisionExpressionNode *>(expr))
        {
            generateDivisionExpression(divExpr);
        }
        else if (auto *compExpr = dynamic_cast<ComparisonExpressionNode *>(expr))
        {
            generateComparisonExpression(compExpr);
        }
        else if (auto *eqExpr = dynamic_cast<EqualityExpressionNode *>(expr))
        {
            generateEqualityExpression(eqExpr);
        }
        else if (auto *orExpr = dynamic_cast<OrExpressionNode *>(expr))
        {
            generateOrExpression(orExpr);
        }
        else if (auto *andExpr = dynamic_cast<AndExpressionNode *>(expr))
        {
            generateAndExpression(andExpr);
        }
        else if (auto *binaryExpr = dynamic_cast<BinaryExpressionNode *>(expr))
        {
            generateBinaryExpression(binaryExpr);
        }
        else if (auto *assignExpr = dynamic_cast<AssignmentExpressionNode *>(expr))
        {
            generateAssignmentExpression(assignExpr);
        }
        // Add any other expression types you might have
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
                generateIfStatement(ifNode);
            }
            else if (auto *forNode = dynamic_cast<ForStatementNode *>(statement.get()))
            {
                // Handle for statement
            }
            else if (auto *whileNode = dynamic_cast<WhileStatementNode *>(statement.get()))
            {
                generateWhileStatement(whileNode);
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
        // Track the return type for this function
        std::string returnType = node->returnType.empty() ? "void" : node->returnType;
        functionReturnTypes[node->name] = returnType;

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
        if (!node)
            return;

        // Special handling for loop variables like 'i'
        bool isLoopVariable = (node->name == "i" || node->name == "j" || node->name == "k");

        // Check if variable is already declared in the current scope (but allow loop variables)
        if (!isLoopVariable && declaredVariables.find(node->name) != declaredVariables.end())
        {
            // This is an assignment, not a declaration
            output << node->name;
            if (node->initializer)
            {
                output << " = ";
                generateExpressionHelper(node->initializer.get());
            }
            output << ";\n";
            return;
        }

        // Mark variable as declared
        declaredVariables.insert(node->name);

        if (node->isConst)
        {
            output << "const ";
        }

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

            if (auto *callExpr = dynamic_cast<CallExpressionNode *>(node->initializer.get()))
            {
                if (auto *calleeVar = dynamic_cast<VariableExpressionNode *>(callExpr->callee.get()))
                {
                    if (calleeVar->name == node->typeName)
                    {
                        output << node->typeName << "(";
                        for (size_t i = 0; i < callExpr->arguments.size(); ++i)
                        {
                            generateExpressionHelper(callExpr->arguments[i].get());
                            if (i < callExpr->arguments.size() - 1)
                            {
                                output << ", ";
                            }
                        }
                        output << ")";
                    }
                    else
                    {
                        generateCallExpression(callExpr);
                    }
                }
                else
                {
                    generateCallExpression(callExpr);
                }
            }
            else
            {
                generateExpressionHelper(node->initializer.get());
            }
        }

        output << ";\n";
    }

    void generatePrintExpression(ExpressionNode *expr)
    {
        if (auto *addExpr = dynamic_cast<AdditionExpressionNode *>(expr))
        {
            // Check if the left side is a string literal
            bool hasStringLiteral = dynamic_cast<StringLiteralNode *>(addExpr->left.get()) != nullptr;

            // Check if right side is a boolean-returning function call
            bool rightIsBooleanCall = false;
            if (auto *callExpr = dynamic_cast<CallExpressionNode *>(addExpr->right.get()))
            {
                if (auto *varExpr = dynamic_cast<VariableExpressionNode *>(callExpr->callee.get()))
                {
                    rightIsBooleanCall = isBooleanReturningFunction(varExpr->name);
                }
            }

            if (hasStringLiteral || rightIsBooleanCall)
            {
                // Handle string concatenation - convert + to <<
                generateExpressionHelper(addExpr->left.get());
                output << " << ";

                if (rightIsBooleanCall)
                {
                    // Convert boolean result to string
                    output << "(";
                    generateExpressionHelper(addExpr->right.get());
                    output << " ? \"true\" : \"false\")";
                }
                else
                {
                    generateExpressionHelper(addExpr->right.get());
                }
            }
            else
            {
                // Handle mathematical addition - keep the +
                generateExpressionHelper(addExpr->left.get());
                output << " + ";
                generateExpressionHelper(addExpr->right.get());
            }
        }
        else
        {
            generateExpressionHelper(expr);
        }
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
        else if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->left.get()))
        {
            generateStringLiteral(stringLiteralNode);
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
        else if (auto *leftDivExpr = dynamic_cast<DivisionExpressionNode *>(node->left.get()))
        {
            generateDivisionExpression(leftDivExpr);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->left.get()))
        {
            generateCallExpression(callExprNode);
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
        else if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->right.get()))
        {
            generateStringLiteral(stringLiteralNode);
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
        else if (auto *rightDivExpr = dynamic_cast<DivisionExpressionNode *>(node->right.get()))
        {
            generateDivisionExpression(rightDivExpr);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->right.get()))
        {
            generateCallExpression(callExprNode);
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
        std::cerr << "Generating multiplication expression" << std::endl;

        // Debug the right operand
        if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->right.get()))
        {
            std::cerr << "Right operand is a call expression" << std::endl;
            if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(callExprNode->callee.get()))
            {
                std::cerr << "Callee is: " << varExprNode->name << std::endl;
            }
        }

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
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->right.get()))
        {
            generateCallExpression(callExprNode);
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

            // Debug output
            std::cerr << "Return expression type: ";
            if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->expression.get()))
            {
                std::cerr << "IntegerLiteral" << std::endl;
            }
            else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->expression.get()))
            {
                std::cerr << "FloatingPointLiteral" << std::endl;
            }
            else if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->expression.get()))
            {
                std::cerr << "StringLiteral" << std::endl;
            }
            else if (auto *boolLiteralNode = dynamic_cast<BooleanLiteralNode *>(node->expression.get()))
            {
                std::cerr << "BooleanLiteral" << std::endl;
            }
            else if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->expression.get()))
            {
                std::cerr << "VariableExpression" << std::endl;
            }
            else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->expression.get()))
            {
                std::cerr << "CallExpression" << std::endl;
            }
            else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->expression.get()))
            {
                std::cerr << "AdditionExpression" << std::endl;
            }
            else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->expression.get()))
            {
                std::cerr << "SubtractionExpression" << std::endl;
            }
            else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->expression.get()))
            {
                std::cerr << "MultiplicationExpression" << std::endl;
            }
            else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->expression.get()))
            {
                std::cerr << "DivisionExpression" << std::endl;
            }
            else
            {
                std::cerr << "Unknown" << std::endl;
            }
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

    // Helper to generate any expression uniformly
    void generateExpression(ExpressionNode *expr)
    {
        if (auto *eqExpr = dynamic_cast<EqualityExpressionNode *>(expr))
        {
            generateEqualityExpression(eqExpr);
        }
        else if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(expr))
        {
            generateStringLiteral(stringLiteralNode);
        }
        else if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(expr))
        {
            generateIntegerLiteral(intLiteralNode);
        }
        else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(expr))
        {
            generateFloatingPointLiteral(floatLiteralNode);
        }
        else if (auto *boolLiteralNode = dynamic_cast<BooleanLiteralNode *>(expr))
        {
            generateBooleanLiteral(boolLiteralNode);
        }
        else if (auto *charLiteralNode = dynamic_cast<CharLiteralNode *>(expr))
        {
            generateCharLiteral(charLiteralNode);
        }
        else if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(expr))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(expr))
        {
            generateCallExpression(callExprNode);
        }
        else if (auto *memberAccessNode = dynamic_cast<MemberAccessExpressionNode *>(expr))
        {
            generateMemberAccessExpression(memberAccessNode);
        }
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(expr))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(expr))
        {
            generateSubtractionExpression(subExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(expr))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(expr))
        {
            generateDivisionExpression(divExprNode);
        }
        else if (auto *comparisonNode = dynamic_cast<ComparisonExpressionNode *>(expr))
        {
            generateComparisonExpression(comparisonNode);
        }
        else if (auto *orExprNode = dynamic_cast<OrExpressionNode *>(expr))
        {
            generateOrExpression(orExprNode);
        }
        else if (auto *andExprNode = dynamic_cast<AndExpressionNode *>(expr))
        {
            generateAndExpression(andExprNode);
        }
        else
        {
            // Unsupported expression type for console log
        }
    }

    void generateConsoleLog(ConsoleLogNode *node)
    {
        if (!node || !node->expression)
            return;

        output << "std::cout << ";

        // Special handling for boolean expressions to convert them to strings
        if (auto *boolLiteral = dynamic_cast<BooleanLiteralNode *>(node->expression.get()))
        {
            output << "((" << (boolLiteral->value ? "true" : "false") << ") ? \"true\" : \"false\")";
        }
        else if (auto *callExpr = dynamic_cast<CallExpressionNode *>(node->expression.get()))
        {
            // Check if this is a call to a function that returns boolean
            if (auto *varExpr = dynamic_cast<VariableExpressionNode *>(callExpr->callee.get()))
            {
                // Check if this function returns bool
                if (isBooleanReturningFunction(varExpr->name))
                {
                    output << "(";
                    generateCallExpression(callExpr);
                    output << " ? \"true\" : \"false\")";
                }
                else
                {
                    generateCallExpression(callExpr);
                }
            }
            else
            {
                generateCallExpression(callExpr);
            }
        }
        else if (auto *addExpr = dynamic_cast<AdditionExpressionNode *>(node->expression.get()))
        {
            // Check if this involves string concatenation with boolean
            generatePrintExpression(addExpr);
        }
        else
        {
            generateExpressionHelper(node->expression.get());
        }

        output << " << std::endl;\n";
    }
    void handlePrintAddition(AdditionExpressionNode *node)
    {
        // Handle left operand
        if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->left.get()))
        {
            generateStringLiteral(stringLiteralNode);
        }
        else if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->left.get()))
        {
            generateIntegerLiteral(intLiteralNode);
        }
        else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->left.get()))
        {
            generateFloatingPointLiteral(floatLiteralNode);
        }
        else if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->left.get()))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->left.get()))
        {
            generateCallExpression(callExprNode);
        }
        else if (auto *memberAccessNode = dynamic_cast<MemberAccessExpressionNode *>(node->left.get()))
        {
            generateMemberAccessExpression(memberAccessNode);
        }
        else if (auto *boolLiteralNode = dynamic_cast<BooleanLiteralNode *>(node->left.get()))
        {
            // For boolean literals, we need to convert them to strings
            output << "(";
            generateBooleanLiteral(boolLiteralNode);
            output << " ? \"true\" : \"false\")";
            output << " << ";
            return;
        }
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->left.get()))
        {
            handlePrintAddition(addExprNode);
        }

        // Insert stream operator instead of + for string concatenation
        output << " << ";

        // Handle right operand
        if (auto *stringLiteralNode = dynamic_cast<StringLiteralNode *>(node->right.get()))
        {
            generateStringLiteral(stringLiteralNode);
        }
        else if (auto *intLiteralNode = dynamic_cast<IntegerLiteralNode *>(node->right.get()))
        {
            generateIntegerLiteral(intLiteralNode);
        }
        else if (auto *floatLiteralNode = dynamic_cast<FloatingPointLiteralNode *>(node->right.get()))
        {
            generateFloatingPointLiteral(floatLiteralNode);
        }
        else if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->right.get()))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *callExprNode = dynamic_cast<CallExpressionNode *>(node->right.get()))
        {
            generateCallExpression(callExprNode);
        }
        else if (auto *memberAccessNode = dynamic_cast<MemberAccessExpressionNode *>(node->right.get()))
        {
            generateMemberAccessExpression(memberAccessNode);
        }
        else if (auto *boolLiteralNode = dynamic_cast<BooleanLiteralNode *>(node->right.get()))
        {
            // For boolean literals, we need to convert them to strings
            output << "(";
            generateBooleanLiteral(boolLiteralNode);
            output << " ? \"true\" : \"false\")";
        }
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->right.get()))
        {
            // For nested addition expressions, handle them recursively
            output << "(";
            generateAdditionExpression(addExprNode);
            output << ")";
        }
        else if (auto *subExprNode = dynamic_cast<SubtractionExpressionNode *>(node->right.get()))
        {
            // For subtraction expressions, wrap them in parentheses
            output << "(";
            generateSubtractionExpression(subExprNode);
            output << ")";
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->right.get()))
        {
            // For multiplication expressions, wrap them in parentheses
            output << "(";
            generateMultiplicationExpression(mulExprNode);
            output << ")";
        }
        else if (auto *divExprNode = dynamic_cast<DivisionExpressionNode *>(node->right.get()))
        {
            // For division expressions, wrap them in parentheses
            output << "(";
            generateDivisionExpression(divExprNode);
            output << ")";
        }
        // Remove the duplicate handling of the right operand
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
        if (!node || !node->callee)
            return;

        generateExpressionHelper(node->callee.get());

        output << "(";
        for (size_t i = 0; i < node->arguments.size(); ++i)
        {
            generateExpressionHelper(node->arguments[i].get());
            if (i < node->arguments.size() - 1)
            {
                output << ", ";
            }
        }
        output << ")";
    }

    void generateMemberAccessExpression(MemberAccessExpressionNode *node)
    {
        if (!node || !node->object)
            return;

        generateExpressionHelper(node->object.get());
        output << ".";
        output << node->memberName;
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
        else if (auto *addExprNode = dynamic_cast<AdditionExpressionNode *>(node->right.get()))
        {
            generateAdditionExpression(addExprNode);
        }
        else if (auto *mulExprNode = dynamic_cast<MultiplicationExpressionNode *>(node->right.get()))
        {
            generateMultiplicationExpression(mulExprNode);
        }
        else if (auto *binaryExprNode = dynamic_cast<BinaryExpressionNode *>(node->right.get()))
        {
            generateBinaryExpression(binaryExprNode);
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

    void generateBinaryExpression(BinaryExpressionNode *node)
    {
        // Generate left operand
        if (auto *leftVarExpr = dynamic_cast<VariableExpressionNode *>(node->left.get()))
        {
            generateVariableExpression(leftVarExpr);
        }
        else if (auto *leftIntLiteral = dynamic_cast<IntegerLiteralNode *>(node->left.get()))
        {
            generateIntegerLiteral(leftIntLiteral);
        }
        else if (auto *leftAddExpr = dynamic_cast<AdditionExpressionNode *>(node->left.get()))
        {
            generateAdditionExpression(leftAddExpr);
        }
        else if (auto *leftSubExpr = dynamic_cast<SubtractionExpressionNode *>(node->left.get()))
        {
            generateSubtractionExpression(leftSubExpr);
        }
        else if (auto *leftMulExpr = dynamic_cast<MultiplicationExpressionNode *>(node->left.get()))
        {
            generateMultiplicationExpression(leftMulExpr);
        }
        else if (auto *leftDivExpr = dynamic_cast<DivisionExpressionNode *>(node->left.get()))
        {
            generateDivisionExpression(leftDivExpr);
        }

        // Output operator
        output << " " << node->op << " ";

        // Generate right operand
        if (auto *rightVarExpr = dynamic_cast<VariableExpressionNode *>(node->right.get()))
        {
            generateVariableExpression(rightVarExpr);
        }
        else if (auto *rightIntLiteral = dynamic_cast<IntegerLiteralNode *>(node->right.get()))
        {
            generateIntegerLiteral(rightIntLiteral);
        }
        else if (auto *rightAddExpr = dynamic_cast<AdditionExpressionNode *>(node->right.get()))
        {
            generateAdditionExpression(rightAddExpr);
        }
        else if (auto *rightSubExpr = dynamic_cast<SubtractionExpressionNode *>(node->right.get()))
        {
            generateSubtractionExpression(rightSubExpr);
        }
        else if (auto *rightMulExpr = dynamic_cast<MultiplicationExpressionNode *>(node->right.get()))
        {
            generateMultiplicationExpression(rightMulExpr);
        }
        else if (auto *rightDivExpr = dynamic_cast<DivisionExpressionNode *>(node->right.get()))
        {
            generateDivisionExpression(rightDivExpr);
        }
    }

    void generateOrExpression(OrExpressionNode *node)
    {
        generateOrExpressionDirect(node);
    }

    void generateAndExpression(AndExpressionNode *node)
    {
        // Generate left operand
        if (auto *leftEq = dynamic_cast<EqualityExpressionNode *>(node->left.get()))
        {
            generateEqualityExpression(leftEq);
        }
        else if (auto *leftCompExpr = dynamic_cast<ComparisonExpressionNode *>(node->left.get()))
        {
            generateComparisonExpression(leftCompExpr);
        }
        else if (auto *leftVarExpr = dynamic_cast<VariableExpressionNode *>(node->left.get()))
        {
            generateVariableExpression(leftVarExpr);
        }
        else if (auto *leftBinaryExpr = dynamic_cast<BinaryExpressionNode *>(node->left.get()))
        {
            generateBinaryExpression(leftBinaryExpr);
        }
        else
        {
            // Use the generic expression generator for other types
            generateExpression(node->left.get());
        }

        // Output operator
        output << " && ";

        // Generate right operand
        if (auto *rightEq = dynamic_cast<EqualityExpressionNode *>(node->right.get()))
        {
            generateEqualityExpression(rightEq);
        }
        else if (auto *rightCompExpr = dynamic_cast<ComparisonExpressionNode *>(node->right.get()))
        {
            generateComparisonExpression(rightCompExpr);
        }
        else if (auto *rightVarExpr = dynamic_cast<VariableExpressionNode *>(node->right.get()))
        {
            generateVariableExpression(rightVarExpr);
        }
        else if (auto *rightBinaryExpr = dynamic_cast<BinaryExpressionNode *>(node->right.get()))
        {
            generateBinaryExpression(rightBinaryExpr);
        }
        else
        {
            // Use the generic expression generator for other types
            generateExpression(node->right.get());
        }
    }

    void generateEqualityExpression(EqualityExpressionNode *node)
    {
        if (!node)
            return;

        // Handle left side
        if (auto *leftBinary = dynamic_cast<BinaryExpressionNode *>(node->left.get()))
        {
            // Special handling for binary expressions like num % 2
            generateExpressionHelper(leftBinary->left.get());  // num
            output << " " << leftBinary->op << " ";            // %
            generateExpressionHelper(leftBinary->right.get()); // 2
        }
        else
        {
            generateExpressionHelper(node->left.get());
        }

        // Output equality operator
        output << " " << node->op << " ";

        // Handle right side
        generateExpressionHelper(node->right.get());
    }

    void generateComparisonExpression(ComparisonExpressionNode *node)
    {
        // Generate left operand
        if (auto *leftVarExpr = dynamic_cast<VariableExpressionNode *>(node->left.get()))
        {
            generateVariableExpression(leftVarExpr);
        }
        else if (auto *leftIntLiteral = dynamic_cast<IntegerLiteralNode *>(node->left.get()))
        {
            generateIntegerLiteral(leftIntLiteral);
        }
        else if (auto *leftAddExpr = dynamic_cast<AdditionExpressionNode *>(node->left.get()))
        {
            generateAdditionExpression(leftAddExpr);
        }
        else if (auto *leftSubExpr = dynamic_cast<SubtractionExpressionNode *>(node->left.get()))
        {
            generateSubtractionExpression(leftSubExpr);
        }
        else if (auto *leftBinaryExpr = dynamic_cast<BinaryExpressionNode *>(node->left.get()))
        {
            generateBinaryExpression(leftBinaryExpr);
        }
        else if (auto *leftMulExpr = dynamic_cast<MultiplicationExpressionNode *>(node->left.get()))
        {
            generateMultiplicationExpression(leftMulExpr);
        }
        else if (auto *leftDivExpr = dynamic_cast<DivisionExpressionNode *>(node->left.get()))
        {
            generateDivisionExpression(leftDivExpr);
        }
        else
        {
            // Use the generic expression generator for other types
            generateExpression(node->left.get());
        }

        // Output operator
        if (node->op == "<")
        {
            output << " < ";
        }
        else if (node->op == "<=")
        {
            output << " <= ";
        }
        else if (node->op == ">")
        {
            output << " > ";
        }
        else if (node->op == ">=")
        {
            output << " >= ";
        }
        else if (node->op == "==")
        {
            output << " == ";
        }
        else if (node->op == "!=")
        {
            output << " != ";
        }
        else
        {
            output << " " << node->op << " ";
        }

        // Generate right operand
        if (auto *rightVarExpr = dynamic_cast<VariableExpressionNode *>(node->right.get()))
        {
            generateVariableExpression(rightVarExpr);
        }
        else if (auto *rightIntLiteral = dynamic_cast<IntegerLiteralNode *>(node->right.get()))
        {
            generateIntegerLiteral(rightIntLiteral);
        }
        else if (auto *rightAddExpr = dynamic_cast<AdditionExpressionNode *>(node->right.get()))
        {
            generateAdditionExpression(rightAddExpr);
        }
        else if (auto *rightSubExpr = dynamic_cast<SubtractionExpressionNode *>(node->right.get()))
        {
            generateSubtractionExpression(rightSubExpr);
        }
        else if (auto *rightMulExpr = dynamic_cast<MultiplicationExpressionNode *>(node->right.get()))
        {
            generateMultiplicationExpression(rightMulExpr);
        }
        else if (auto *rightDivExpr = dynamic_cast<DivisionExpressionNode *>(node->right.get()))
        {
            generateDivisionExpression(rightDivExpr);
        }
        else
        {
            // Use the generic expression generator for other types
            generateExpression(node->right.get());
        }
    }

    void generateWhileStatement(WhileStatementNode *node)
    {
        output << "while (";

        // Generate condition
        if (auto *varExprNode = dynamic_cast<VariableExpressionNode *>(node->condition.get()))
        {
            generateVariableExpression(varExprNode);
        }
        else if (auto *comparisonNode = dynamic_cast<ComparisonExpressionNode *>(node->condition.get()))
        {
            generateComparisonExpression(comparisonNode);
        }
        else if (auto *eqExprNode = dynamic_cast<EqualityExpressionNode *>(node->condition.get()))
        {
            generateEqualityExpression(eqExprNode);
        }
        else if (auto *orExprNode = dynamic_cast<OrExpressionNode *>(node->condition.get()))
        {
            generateOrExpression(orExprNode);
        }
        else if (auto *andExprNode = dynamic_cast<AndExpressionNode *>(node->condition.get()))
        {
            generateAndExpression(andExprNode);
        }
        else if (auto *binaryExprNode = dynamic_cast<BinaryExpressionNode *>(node->condition.get()))
        {
            generateBinaryExpression(binaryExprNode);
        }
        else
        {
            // Use the generic expression generator for other types
            generateExpression(node->condition.get());
        }

        output << ") ";

        // Generate body
        if (auto *blockStmt = dynamic_cast<BlockStatementNode *>(node->body.get()))
        {
            generateBlockStatement(blockStmt);
        }
        else if (auto *ifStmt = dynamic_cast<IfStatementNode *>(node->body.get()))
        {
            generateIfStatement(ifStmt);
        }
        else
        {
            output << "{\n";
            indentLevel++;
            outputIndent();
            // Handle other statement types
            output << "// Non-block statement\n";
            indentLevel--;
            outputIndent();
            output << "}";
        }

        output << "\n";
    }

    void generateIfStatement(IfStatementNode *node)
    {
        output << "if (";

        // Handle the condition
        if (node->condition)
        {
            generateExpressionHelper(node->condition.get());
        }
        else
        {
            // If no condition, generate a placeholder
            output << "true";
        }

        output << ")";

        // Generate then block
        if (node->thenBranch)
        {
            if (auto *blockNode = dynamic_cast<BlockStatementNode *>(node->thenBranch.get()))
            {
                generateBlockStatement(blockNode);
            }
            else
            {
                output << " {\n";
                indentLevel++;
                outputIndent();

                if (auto *returnNode = dynamic_cast<ReturnStatementNode *>(node->thenBranch.get()))
                {
                    generateReturnStatement(returnNode);
                }
                else if (auto *exprStmtNode = dynamic_cast<ExpressionStatementNode *>(node->thenBranch.get()))
                {
                    generateExpressionStatement(exprStmtNode);
                }

                indentLevel--;
                outputIndent();
                output << "}";
            }
        }

        // Generate else block if present
        if (node->elseBranch)
        {
            output << " else";

            if (auto *blockNode = dynamic_cast<BlockStatementNode *>(node->elseBranch.get()))
            {
                generateBlockStatement(blockNode);
            }
            else if (auto *ifNode = dynamic_cast<IfStatementNode *>(node->elseBranch.get()))
            {
                output << " ";
                generateIfStatement(ifNode);
            }
            else
            {
                output << " {\n";
                indentLevel++;
                outputIndent();

                if (auto *returnNode = dynamic_cast<ReturnStatementNode *>(node->elseBranch.get()))
                {
                    generateReturnStatement(returnNode);
                }
                else if (auto *exprStmtNode = dynamic_cast<ExpressionStatementNode *>(node->elseBranch.get()))
                {
                    generateExpressionStatement(exprStmtNode);
                }

                indentLevel--;
                outputIndent();
                output << "}";
            }
        }

        output << "\n";
    }
};