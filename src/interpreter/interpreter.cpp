#include "interpreter.h"
#include "../parser/nodes.h" // Add the include for the node definitions
#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <cmath>

// Value implementation
bool Value::asBool() const
{
    if (type == Type::Boolean)
    {
        return std::get<bool>(value);
    }
    else if (type == Type::Integer)
    {
        return std::get<int>(value) != 0;
    }
    else if (type == Type::Float)
    {
        return std::get<float>(value) != 0.0f;
    }
    else if (type == Type::String)
    {
        return !std::get<std::string>(value).empty();
    }
    else if (type == Type::Null)
    {
        return false;
    }
    return true; // Objects, functions, and classes are truthy
}

int Value::asInt() const
{
    if (type == Type::Integer)
    {
        return std::get<int>(value);
    }
    else if (type == Type::Float)
    {
        return static_cast<int>(std::get<float>(value));
    }
    else if (type == Type::Boolean)
    {
        return std::get<bool>(value) ? 1 : 0;
    }
    else if (type == Type::String)
    {
        try
        {
            return std::stoi(std::get<std::string>(value));
        }
        catch (const std::exception &)
        {
            throw std::runtime_error("Cannot convert string to integer");
        }
    }
    throw std::runtime_error("Cannot convert to integer");
}

float Value::asFloat() const
{
    if (type == Type::Float)
    {
        return std::get<float>(value);
    }
    else if (type == Type::Integer)
    {
        return static_cast<float>(std::get<int>(value));
    }
    else if (type == Type::Boolean)
    {
        return std::get<bool>(value) ? 1.0f : 0.0f;
    }
    else if (type == Type::String)
    {
        try
        {
            return std::stof(std::get<std::string>(value));
        }
        catch (const std::exception &)
        {
            throw std::runtime_error("Cannot convert string to float");
        }
    }
    throw std::runtime_error("Cannot convert to float");
}

std::string Value::asString() const
{
    if (type == Type::String)
    {
        return std::get<std::string>(value);
    }
    // Special handling for boolean to ensure "true" or "false" string instead of "1" or "0"
    if (type == Type::Boolean)
    {
        return std::get<bool>(value) ? "true" : "false";
    }
    return toString();
}

template <typename T>
std::shared_ptr<T> Value::asObject() const
{
    if (std::holds_alternative<std::shared_ptr<void>>(value))
    {
        return std::static_pointer_cast<T>(std::get<std::shared_ptr<void>>(value));
    }
    throw std::runtime_error("Value is not an object");
}

std::string Value::toString() const
{
    switch (type)
    {
    case Type::Null:
        return "null";
    case Type::Boolean:
        return std::get<bool>(value) ? "true" : "false";
    case Type::Integer:
        return std::to_string(std::get<int>(value));
    case Type::Float:
    {
        // Format float to avoid trailing zeros
        std::string result = std::to_string(std::get<float>(value));
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
        if (result.back() == '.')
            result.pop_back();
        return result;
    }
    case Type::String:
        return std::get<std::string>(value);
    case Type::Object:
        return "[object Object]";
    case Type::Function:
        return "[function]";
    case Type::Class:
        return "[class]";
    default:
        return "[unknown]";
    }
}

Value Value::operator+(const Value &other) const
{
    // Handle string concatenation
    if (type == Type::String || other.type == Type::String)
    {
        // Special handling for boolean values in string concatenation
        if (type == Type::Boolean)
        {
            std::string boolStr = std::get<bool>(value) ? "true" : "false";
            return Value(boolStr + other.toString());
        }
        else if (other.type == Type::Boolean)
        {
            std::string boolStr = other.asBool() ? "true" : "false";
            return Value(toString() + boolStr);
        }
        else
        {
            return Value(toString() + other.toString());
        }
    }

    // Handle numeric addition
    if (type == Type::Float || other.type == Type::Float)
    {
        return Value(asFloat() + other.asFloat());
    }

    return Value(asInt() + other.asInt());
}

Value Value::operator-(const Value &other) const
{
    if (type == Type::Float || other.type == Type::Float)
    {
        return Value(asFloat() - other.asFloat());
    }
    return Value(asInt() - other.asInt());
}

Value Value::operator*(const Value &other) const
{
    if (type == Type::Float || other.type == Type::Float)
    {
        return Value(asFloat() * other.asFloat());
    }
    return Value(asInt() * other.asInt());
}

Value Value::operator/(const Value &other) const
{
    if (other.asFloat() == 0.0f)
    {
        throw std::runtime_error("Division by zero");
    }

    if (type == Type::Float || other.type == Type::Float)
    {
        return Value(asFloat() / other.asFloat());
    }

    // Return float for integer division to match behavior of most languages
    return Value(static_cast<float>(asInt()) / static_cast<float>(other.asInt()));
}

Value Value::operator%(const Value &other) const
{
    if (other.asInt() == 0)
    {
        throw std::runtime_error("Modulo by zero");
    }

    return Value(asInt() % other.asInt());
}

bool Value::operator==(const Value &other) const
{
    if (type != other.type)
    {
        // Special case for numeric comparison
        if ((type == Type::Integer || type == Type::Float) &&
            (other.type == Type::Integer || other.type == Type::Float))
        {
            return asFloat() == other.asFloat();
        }
        return false;
    }

    switch (type)
    {
    case Type::Null:
        return true; // null == null
    case Type::Boolean:
        return std::get<bool>(value) == std::get<bool>(other.value);
    case Type::Integer:
        return std::get<int>(value) == std::get<int>(other.value);
    case Type::Float:
        return std::get<float>(value) == std::get<float>(other.value);
    case Type::String:
        return std::get<std::string>(value) == std::get<std::string>(other.value);
    case Type::Object:
    case Type::Function:
    case Type::Class:
        // For objects, functions, and classes, compare by reference
        return std::get<std::shared_ptr<void>>(value) == std::get<std::shared_ptr<void>>(other.value);
    default:
        return false;
    }
}

bool Value::operator!=(const Value &other) const
{
    return !(*this == other);
}

bool Value::operator<(const Value &other) const
{
    if ((type == Type::Integer || type == Type::Float) &&
        (other.type == Type::Integer || other.type == Type::Float))
    {
        return asFloat() < other.asFloat();
    }
    if (type == Type::String && other.type == Type::String)
    {
        return std::get<std::string>(value) < std::get<std::string>(other.value);
    }
    throw std::runtime_error("Cannot compare values of different types");
}

bool Value::operator>(const Value &other) const
{
    return other < *this;
}

bool Value::operator<=(const Value &other) const
{
    return *this < other || *this == other;
}

bool Value::operator>=(const Value &other) const
{
    return *this > other || *this == other;
}

// Interpreter implementation
void Interpreter::interpret(ProgramNode *program)
{
    if (!program)
    {
        throw std::runtime_error("No program to interpret");
    }

    globals = environment;

    try
    {
        // First pass: declare classes, functions and global variables
        for (const auto &node : program->children)
        {
            if (auto classNode = dynamic_cast<ClassNode *>(node.get()))
            {
                executeClass(classNode);
            }
            else if (auto funcNode = dynamic_cast<FunctionNode *>(node.get()))
            {
                executeFunction(funcNode);
            }
            else if (auto varDecl = dynamic_cast<VariableDeclarationNode *>(node.get()))
            {
                executeVariableDeclaration(varDecl);
            }
        }

        // Look for a main function and execute it if found
        lookForMainFunction(program);

        // If no main function found, execute all statements in order
        for (const auto &node : program->children)
        {
            execute(node.get());
        }
    }
    catch (const ReturnException &)
    {
        // Ignore return exception from global scope
    }
    catch (const std::exception &e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        throw;
    }
}

void Interpreter::lookForMainFunction(ProgramNode *program)
{
    // Look for a function named "main"
    for (const auto &node : program->children)
    {
        if (auto funcNode = dynamic_cast<FunctionNode *>(node.get()))
        {
            if (funcNode->name == "main")
            {
                DEBUG_LOG("Found main function, executing it");

                // Get the function from the environment
                auto mainFunc = environment->get("main");
                if (mainFunc.isFunction())
                {
                    // Call the main function with no arguments
                    std::vector<Value> args;
                    callFunction(mainFunc.asObject<Function>(), args);

                    // Stop further processing
                    throw ReturnException(Value());
                }
            }
        }
    }
}

void Interpreter::execute(ASTNode *node)
{
    if (!node)
        return;

    DEBUG_LOG("Executing node type: ", typeid(*node).name());

    if (auto blockNode = dynamic_cast<BlockStatementNode *>(node))
    {
        executeBlockStatement(blockNode);
    }
    else if (auto varDeclNode = dynamic_cast<VariableDeclarationNode *>(node))
    {
        executeVariableDeclaration(varDeclNode);
    }
    else if (auto ifNode = dynamic_cast<IfStatementNode *>(node))
    {
        executeIfStatement(ifNode);
    }
    else if (auto whileNode = dynamic_cast<WhileStatementNode *>(node))
    {
        executeWhileStatement(whileNode);
    }
    else if (auto returnNode = dynamic_cast<ReturnStatementNode *>(node))
    {
        executeReturnStatement(returnNode);
    }
    else if (auto exprStmtNode = dynamic_cast<ExpressionStatementNode *>(node))
    {
        executeExpressionStatement(exprStmtNode);
    }
    else if (auto consoleLogNode = dynamic_cast<ConsoleLogNode *>(node))
    {
        executeConsoleLog(consoleLogNode);
    }
    else if (auto inputNode = dynamic_cast<InputStatementNode *>(node))
    {
        executeInputStatement(inputNode);
    }
    else if (auto functionNode = dynamic_cast<FunctionNode *>(node))
    {
        executeFunction(functionNode);
    }
    else if (auto classNode = dynamic_cast<ClassNode *>(node))
    {
        executeClass(classNode);
    }
    else
    {
        DEBUG_LOG("Unhandled node type: ", typeid(*node).name());
    }
}

Value Interpreter::evaluate(ExpressionNode *expr)
{
    if (!expr)
        return Value(); // Return null for null expressions

    DEBUG_LOG("Evaluating expression type: ", typeid(*expr).name());

    if (auto varExpr = dynamic_cast<VariableExpressionNode *>(expr))
    {
        return evaluateVariableExpression(varExpr);
    }
    else if (auto callExpr = dynamic_cast<CallExpressionNode *>(expr))
    {
        return evaluateCallExpression(callExpr);
    }
    else if (auto assignExpr = dynamic_cast<AssignmentExpressionNode *>(expr))
    {
        return evaluateAssignmentExpression(assignExpr);
    }
    else if (auto memberAccessExpr = dynamic_cast<MemberAccessExpressionNode *>(expr))
    {
        return evaluateMemberAccessExpression(memberAccessExpr);
    }
    else if (auto intLiteral = dynamic_cast<IntegerLiteralNode *>(expr))
    {
        return evaluateIntegerLiteral(intLiteral);
    }
    else if (auto floatLiteral = dynamic_cast<FloatingPointLiteralNode *>(expr))
    {
        return evaluateFloatingPointLiteral(floatLiteral);
    }
    else if (auto stringLiteral = dynamic_cast<StringLiteralNode *>(expr))
    {
        return evaluateStringLiteral(stringLiteral);
    }
    else if (auto boolLiteral = dynamic_cast<BooleanLiteralNode *>(expr))
    {
        return evaluateBooleanLiteral(boolLiteral);
    }
    else if (auto nullLiteral = dynamic_cast<NullLiteralNode *>(expr))
    {
        return evaluateNullLiteral(nullLiteral);
    }
    else if (auto addExpr = dynamic_cast<AdditionExpressionNode *>(expr))
    {
        Value left = evaluate(addExpr->left.get());
        Value right = evaluate(addExpr->right.get());
        return left + right;
    }
    else if (auto subExpr = dynamic_cast<SubtractionExpressionNode *>(expr))
    {
        Value left = evaluate(subExpr->left.get());
        Value right = evaluate(subExpr->right.get());
        return left - right;
    }
    else if (auto mulExpr = dynamic_cast<MultiplicationExpressionNode *>(expr))
    {
        Value left = evaluate(mulExpr->left.get());
        Value right = evaluate(mulExpr->right.get());
        return left * right;
    }
    else if (auto divExpr = dynamic_cast<DivisionExpressionNode *>(expr))
    {
        Value left = evaluate(divExpr->left.get());
        Value right = evaluate(divExpr->right.get());
        return left / right;
    }
    else if (auto compExpr = dynamic_cast<ComparisonExpressionNode *>(expr))
    {
        Value left = evaluate(compExpr->left.get());
        Value right = evaluate(compExpr->right.get());

        if (compExpr->op == "<")
            return Value(left < right);
        if (compExpr->op == ">")
            return Value(left > right);
        if (compExpr->op == "<=")
            return Value(left <= right);
        if (compExpr->op == ">=")
            return Value(left >= right);

        throw std::runtime_error("Unknown comparison operator: " + compExpr->op);
    }
    else if (auto eqExpr = dynamic_cast<EqualityExpressionNode *>(expr))
    {
        Value left = evaluate(eqExpr->left.get());
        Value right = evaluate(eqExpr->right.get());

        if (eqExpr->op == "==")
            return Value(left == right);
        if (eqExpr->op == "!=")
            return Value(left != right);

        throw std::runtime_error("Unknown equality operator: " + eqExpr->op);
    }
    else if (auto orExpr = dynamic_cast<OrExpressionNode *>(expr))
    {
        Value left = evaluate(orExpr->left.get());

        // Short-circuit evaluation
        if (left.asBool())
            return Value(true);

        Value right = evaluate(orExpr->right.get());
        return Value(right.asBool());
    }
    else if (auto andExpr = dynamic_cast<AndExpressionNode *>(expr))
    {
        Value left = evaluate(andExpr->left.get());

        // Short-circuit evaluation
        if (!left.asBool())
            return Value(false);

        Value right = evaluate(andExpr->right.get());
        return Value(right.asBool());
    }
    else if (auto binaryExpr = dynamic_cast<BinaryExpressionNode *>(expr))
    {
        return evaluateBinaryExpression(binaryExpr);
    }

    DEBUG_LOG("Unknown expression type: ", typeid(*expr).name());
    return Value(); // Default to null
}

void Interpreter::executeBlockStatement(BlockStatementNode *node, std::shared_ptr<Environment> env)
{
    std::shared_ptr<Environment> previous = this->environment;

    try
    {
        if (env)
        {
            // If an environment is provided, use it
            this->environment = env;
        }
        else
        {
            // Otherwise, create a new environment with the current one as enclosing
            this->environment = std::make_shared<Environment>(this->environment);
        }

        for (const auto &statement : node->statements)
        {
            execute(statement.get());
        }
    }
    catch (const ReturnException &)
    {
        // Let the return exception propagate up
        this->environment = previous;
        throw;
    }
    catch (const std::exception &e)
    {
        this->environment = previous;
        throw;
    }

    this->environment = previous;
}

void Interpreter::executeVariableDeclaration(VariableDeclarationNode *node)
{
    Value initialValue;

    if (node->initializer)
    {
        // First, check if the typeName corresponds to a class in the environment
        bool isClassType = false;
        try
        {
            Value classValue = environment->get(node->typeName);
            isClassType = classValue.isClass();
        }
        catch (...)
        {
            // Class doesn't exist in environment, continue with normal processing
        }

        // Special handling for class instantiation
        if (isClassType)
        {
            if (auto callExpr = dynamic_cast<CallExpressionNode *>(node->initializer.get()))
            {
                if (auto varExpr = dynamic_cast<VariableExpressionNode *>(callExpr->callee.get()))
                {
                    if (varExpr->name == node->typeName)
                    {
                        // This is definitely a class instantiation: TypeName var = TypeName();
                        DEBUG_LOG("Detected class instantiation: ", node->typeName, " ", node->name);

                        // Get the class from environment
                        Value classValue = environment->get(node->typeName);
                        auto klass = classValue.asObject<Class>();

                        // Create arguments for constructor
                        std::vector<Value> arguments;
                        for (const auto &arg : callExpr->arguments)
                        {
                            arguments.push_back(evaluate(arg.get()));
                        }

                        // Create the instance
                        initialValue = createInstance(klass, arguments);
                        DEBUG_LOG("Created instance successfully, type: ", static_cast<int>(initialValue.getType()));
                    }
                    else
                    {
                        // Different function call, evaluate normally
                        initialValue = evaluate(node->initializer.get());
                    }
                }
                else
                {
                    // Complex call expression, evaluate normally
                    initialValue = evaluate(node->initializer.get());
                }
            }
            else
            {
                // Not a call expression, evaluate normally
                initialValue = evaluate(node->initializer.get());
            }
        }
        else
        {
            // Not a class type, evaluate normally
            initialValue = evaluate(node->initializer.get());
        }

        // Log the type of initial value for debugging
        DEBUG_LOG("Initial value for ", node->name, " type: ", static_cast<int>(initialValue.getType()),
                  " is object: ", initialValue.isObject());
    }
    else
    {
        // Default values based on type
        if (node->typeName == "int")
            initialValue = Value(0);
        else if (node->typeName == "float")
            initialValue = Value(0.0f);
        else if (node->typeName == "string")
            initialValue = Value(std::string(""));
        else if (node->typeName == "bool")
            initialValue = Value(false);
        // Default to null for other types (including classes without initializers)
    }

    environment->define(node->name, initialValue);
    DEBUG_LOG("Defined variable ", node->name, " type: ", static_cast<int>(initialValue.getType()),
              " value: ", initialValue.toString(), " is object: ", initialValue.isObject());
}
void Interpreter::executeIfStatement(IfStatementNode *node)
{
    Value condition = evaluate(node->condition.get());

    if (condition.asBool())
    {
        execute(node->thenBranch.get());
    }
    else if (node->elseBranch)
    {
        execute(node->elseBranch.get());
    }
}

void Interpreter::executeWhileStatement(WhileStatementNode *node)
{
    while (evaluate(node->condition.get()).asBool())
    {
        execute(node->body.get());
    }
}

void Interpreter::executeReturnStatement(ReturnStatementNode *node)
{
    Value returnValue;

    if (node->expression)
    {
        returnValue = evaluate(node->expression.get());
    }

    throw ReturnException(returnValue);
}

void Interpreter::executeExpressionStatement(ExpressionStatementNode *node)
{
    lastValue = evaluate(node->expression.get());
}

void Interpreter::executeConsoleLog(ConsoleLogNode *node)
{
    if (!node || !node->expression)
    {
        std::cout << std::endl;
        return;
    }

    Value value = evaluate(node->expression.get());

    // For boolean values, explicitly convert to "true" or "false" strings
    if (value.getType() == Value::Type::Boolean)
    {
        std::cout << (value.asBool() ? "true" : "false") << std::endl;
    }
    else
    {
        std::cout << value.toString() << std::endl;
    }
}

void Interpreter::executeInputStatement(InputStatementNode *node)
{
    std::string input;
    std::getline(std::cin, input);

    if (node->variable)
    {
        Value inputValue = Value(input);
        // Convert based on variable type if needed
        if (node->variable->typeName == "int")
        {
            try
            {
                inputValue = Value(std::stoi(input));
            }
            catch (...)
            {
                inputValue = Value(0);
            }
        }
        else if (node->variable->typeName == "float")
        {
            try
            {
                inputValue = Value(std::stof(input));
            }
            catch (...)
            {
                inputValue = Value(0.0f);
            }
        }
        else if (node->variable->typeName == "bool")
        {
            if (input == "true" || input == "1")
            {
                inputValue = Value(true);
            }
            else
            {
                inputValue = Value(false);
            }
        }

        environment->define(node->variable->name, inputValue);
    }
}

void Interpreter::executeFunction(FunctionNode *node)
{
    auto function = std::make_shared<Function>(
        std::static_pointer_cast<FunctionNode>(
            std::shared_ptr<ASTNode>((ASTNode *)node, [](ASTNode *) {})));

    // Add the function to the environment
    environment->define(node->name, Value(std::static_pointer_cast<void>(function), Value::Type::Function));
    DEBUG_LOG("Defined function: ", node->name);
}

void Interpreter::executeClass(ClassNode *node)
{
    // Create a new class
    auto klass = std::make_shared<Class>(node->name);

    // Define methods
    for (const auto &member : node->members)
    {
        if (auto method = dynamic_cast<FunctionNode *>(member.get()))
        {
            auto function = std::make_shared<Function>(
                std::static_pointer_cast<FunctionNode>(
                    std::shared_ptr<ASTNode>((ASTNode *)method, [](ASTNode *) {})));

            klass->methods[method->name] = function;
            DEBUG_LOG("Added method ", method->name, " to class ", node->name);
        }
    }

    // Add the class to the environment
    environment->define(node->name, Value(std::static_pointer_cast<void>(klass), Value::Type::Class));
    DEBUG_LOG("Defined class: ", node->name);
}

Value Interpreter::evaluateVariableExpression(VariableExpressionNode *node)
{
    DEBUG_LOG("=== EVALUATING VARIABLE: ", node->name, " ===");

    try
    {
        Value result = environment->get(node->name);
        DEBUG_LOG("Variable '", node->name, "' found with type: ", static_cast<int>(result.getType()),
                  ", isObject: ", result.isObject(),
                  ", value: ", result.toString());
        return result;
    }
    catch (const std::exception &e)
    {
        DEBUG_LOG("ERROR: Variable '", node->name, "' not found: ", e.what());
        throw;
    }
}

Value Interpreter::evaluateBinaryExpression(BinaryExpressionNode *node)
{
    DEBUG_LOG("Evaluating binary expression with operator: ", node->op);

    if (node->op == "+")
    {
        // For addition, evaluate in standard left-to-right order
        Value left = evaluate(node->left.get());
        Value right = evaluate(node->right.get());

        DEBUG_LOG("Left operand type: ", static_cast<int>(left.getType()));
        DEBUG_LOG("Right operand type: ", static_cast<int>(right.getType()));

        // If either operand is a string, handle as string concatenation
        if (left.getType() == Value::Type::String || right.getType() == Value::Type::String)
        {
            // Special handling for boolean values in concatenation
            if (left.getType() == Value::Type::Boolean)
            {
                std::string boolStr = left.asBool() ? "true" : "false";
                return Value(boolStr + right.toString());
            }
            else if (right.getType() == Value::Type::Boolean)
            {
                std::string boolStr = right.asBool() ? "true" : "false";
                return Value(left.toString() + boolStr);
            }

            // Regular string concatenation
            return Value(left.toString() + right.toString());
        }

        // Regular numeric addition
        return left + right;
    }

    // For other operators, use standard evaluation order
    Value left = evaluate(node->left.get());
    Value right = evaluate(node->right.get());

    if (node->op == "-")
        return left - right;
    if (node->op == "*")
        return left * right;
    if (node->op == "/")
        return left / right;
    if (node->op == "%")
    {
        int leftInt = left.asInt();
        int rightInt = right.asInt();

        if (rightInt == 0)
            throw std::runtime_error("Modulo by zero");

        return Value(leftInt % rightInt);
    }

    throw std::runtime_error("Unknown binary operator: " + node->op);
}

Value Interpreter::evaluateCallExpression(CallExpressionNode *node)
{
    DEBUG_LOG("=== EVALUATING CALL EXPRESSION ===");

    // Check if this is a method call (object.method())
    if (auto memberExpr = dynamic_cast<MemberAccessExpressionNode *>(node->callee.get()))
    {
        DEBUG_LOG("METHOD CALL DETECTED");
        DEBUG_LOG("Method name: ", memberExpr->memberName);

        // Debug the object expression before evaluating
        if (auto varExpr = dynamic_cast<VariableExpressionNode *>(memberExpr->object.get()))
        {
            DEBUG_LOG("Object is a variable: ", varExpr->name);
        }
        else
        {
            DEBUG_LOG("Object is not a simple variable");
        }

        // First evaluate the object
        DEBUG_LOG("About to evaluate object...");
        Value object = evaluate(memberExpr->object.get());
        DEBUG_LOG("Evaluated object type: ", static_cast<int>(object.getType()),
                  ", isObject: ", object.isObject(),
                  ", value: ", object.toString());

        if (object.isObject())
        {
            auto obj = object.asObject<Object>();
            if (obj && obj->klass)
            {
                DEBUG_LOG("Object has class: ", obj->klass->name);

                // Check if the class has the method
                if (obj->klass->hasMethod(memberExpr->memberName))
                {
                    DEBUG_LOG("Method found in class");

                    // Get the method and bind it to the object
                    auto method = obj->klass->getMethod(memberExpr->memberName);
                    auto boundMethod = std::make_shared<Function>(
                        method->declaration, std::static_pointer_cast<void>(obj));

                    // Prepare arguments
                    std::vector<Value> arguments;
                    for (const auto &arg : node->arguments)
                    {
                        arguments.push_back(evaluate(arg.get()));
                    }

                    // Call the method
                    DEBUG_LOG("Calling method with ", arguments.size(), " arguments");
                    return callFunction(boundMethod, arguments);
                }
                else
                {
                    throw std::runtime_error("Method '" + memberExpr->memberName +
                                             "' not found in class '" + obj->klass->name + "'");
                }
            }
            else
            {
                DEBUG_LOG("Object has no class");
                throw std::runtime_error("Object has no class");
            }
        }
        else
        {
            // Error case - trying to call a method on a non-object
            std::string typeName;
            switch (object.getType())
            {
            case Value::Type::Null:
                typeName = "null";
                break;
            case Value::Type::Boolean:
                typeName = "boolean";
                break;
            case Value::Type::Integer:
                typeName = "integer";
                break;
            case Value::Type::Float:
                typeName = "float";
                break;
            case Value::Type::String:
                typeName = "string";
                break;
            case Value::Type::Object:
                typeName = "object";
                break;
            case Value::Type::Function:
                typeName = "function";
                break;
            case Value::Type::Class:
                typeName = "class";
                break;
            default:
                typeName = "unknown";
                break;
            }

            DEBUG_LOG("ERROR: Object evaluated to non-object type: ", typeName);
            throw std::runtime_error("Cannot access property '" + memberExpr->memberName +
                                     "' of non-object value (type: " + typeName + ")");
        }
    }

    // Regular function call path
    DEBUG_LOG("REGULAR FUNCTION CALL");

    // First evaluate the callee
    Value callee = evaluate(node->callee.get());

    // Debug the callee type
    if (auto varExpr = dynamic_cast<VariableExpressionNode *>(node->callee.get()))
    {
        DEBUG_LOG("Calling function: ", varExpr->name);
    }

    // Prepare arguments
    std::vector<Value> arguments;
    for (const auto &arg : node->arguments)
    {
        DEBUG_LOG("Evaluating argument...");
        arguments.push_back(evaluate(arg.get()));
    }
    DEBUG_LOG("Call with ", arguments.size(), " arguments");

    // Handle different callee types
    if (callee.isFunction())
    {
        // Regular function call
        DEBUG_LOG("Calling a function");
        return callFunction(callee.asObject<Function>(), arguments);
    }
    else if (callee.isClass())
    {
        // Class instantiation - "new" operator simulation
        auto klass = callee.asObject<Class>();
        DEBUG_LOG("Instantiating class: ", klass->name);

        Value instance = createInstance(klass, arguments);
        DEBUG_LOG("Instance created, type: ", static_cast<int>(instance.getType()));
        return instance;
    }
    else if (callee.getType() == Value::Type::Object && std::holds_alternative<std::shared_ptr<void>>(callee.getValue()))
    {
        // Check if it's a native function wrapper
        try
        {
            auto nativeFunc = std::static_pointer_cast<NativeFunctionWrapper>(std::get<std::shared_ptr<void>>(callee.getValue()));
            DEBUG_LOG("Calling native function: ", nativeFunc->name);
            return callNativeFunction(nativeFunc, arguments);
        }
        catch (const std::bad_cast &)
        {
            DEBUG_LOG("Failed to cast to native function wrapper");
            throw std::runtime_error("Can only call functions");
        }
    }

    std::string typeName;
    switch (callee.getType())
    {
    case Value::Type::Null:
        typeName = "null";
        break;
    case Value::Type::Boolean:
        typeName = "boolean";
        break;
    case Value::Type::Integer:
        typeName = "integer";
        break;
    case Value::Type::Float:
        typeName = "float";
        break;
    case Value::Type::String:
        typeName = "string";
        break;
    case Value::Type::Object:
        typeName = "object";
        break;
    case Value::Type::Function:
        typeName = "function";
        break;
    case Value::Type::Class:
        typeName = "class";
        break;
    default:
        typeName = "unknown";
        break;
    }
    throw std::runtime_error("Can only call functions or constructors, got: " + typeName);
}

Value Interpreter::evaluateAssignmentExpression(AssignmentExpressionNode *node)
{
    Value rhs = evaluate(node->right.get());

    if (auto varExpr = dynamic_cast<VariableExpressionNode *>(node->left.get()))
    {
        // Simple variable assignment
        if (node->op == "=")
        {
            environment->assign(varExpr->name, rhs);
            return rhs;
        }

        // Compound assignment (+=, -=, etc.)
        Value lhs = environment->get(varExpr->name);

        if (node->op == "+=")
            environment->assign(varExpr->name, lhs + rhs);
        else if (node->op == "-=")
            environment->assign(varExpr->name, lhs - rhs);
        else if (node->op == "*=")
            environment->assign(varExpr->name, lhs * rhs);
        else if (node->op == "/=")
            environment->assign(varExpr->name, lhs / rhs);
        else if (node->op == "%=")
        {
            Value result = Value(lhs.asInt() % rhs.asInt());
            environment->assign(varExpr->name, result);
        }
        else
            throw std::runtime_error("Unknown assignment operator: " + node->op);

        return environment->get(varExpr->name);
    }
    else if (auto memberExpr = dynamic_cast<MemberAccessExpressionNode *>(node->left.get()))
    {
        // Object property assignment
        Value object = evaluate(memberExpr->object.get());

        if (!object.isObject())
        {
            throw std::runtime_error("Cannot set property on non-object value");
        }

        auto obj = object.asObject<Object>();

        // Handle compound assignment for properties
        if (node->op != "=")
        {
            Value lhs;
            auto it = obj->fields.find(memberExpr->memberName);
            if (it != obj->fields.end())
            {
                lhs = it->second;
            }

            if (node->op == "+=")
                rhs = lhs + rhs;
            else if (node->op == "-=")
                rhs = lhs - rhs;
            else if (node->op == "*=")
                rhs = lhs * rhs;
            else if (node->op == "/=")
                rhs = lhs / rhs;
            else if (node->op == "%=")
                rhs = Value(lhs.asInt() % rhs.asInt());
            else
                throw std::runtime_error("Unknown assignment operator: " + node->op);
        }

        obj->fields[memberExpr->memberName] = rhs;
        return rhs;
    }

    throw std::runtime_error("Invalid assignment target");
}

Value Interpreter::evaluateMemberAccessExpression(MemberAccessExpressionNode *node)
{
    DEBUG_LOG("Evaluating member access: ", node->memberName);

    // Evaluate the object expression directly - no special handling for simple variables
    Value object = evaluate(node->object.get());

    DEBUG_LOG("Member access on object of type: ", static_cast<int>(object.getType()),
              ", isObject: ", object.isObject(),
              ", for property: ", node->memberName);

    if (!object.isObject())
    {
        std::string typeName;
        switch (object.getType())
        {
        case Value::Type::Null:
            typeName = "null";
            break;
        case Value::Type::Boolean:
            typeName = "boolean";
            break;
        case Value::Type::Integer:
            typeName = "integer";
            break;
        case Value::Type::Float:
            typeName = "float";
            break;
        case Value::Type::String:
            typeName = "string";
            break;
        case Value::Type::Object:
            typeName = "object";
            break;
        case Value::Type::Function:
            typeName = "function";
            break;
        case Value::Type::Class:
            typeName = "class";
            break;
        default:
            typeName = "unknown";
            break;
        }
        throw std::runtime_error("Cannot access property '" + node->memberName +
                                 "' of non-object value (type: " + typeName + ")");
    }

    auto obj = object.asObject<Object>();
    if (!obj)
    {
        throw std::runtime_error("Failed to cast to Object");
    }

    // Check if the property exists in the object's fields
    auto it = obj->fields.find(node->memberName);
    if (it != obj->fields.end())
    {
        DEBUG_LOG("Found field: ", node->memberName);
        return it->second;
    }

    // Check if the property exists as a method in the class
    if (obj->klass && obj->klass->hasMethod(node->memberName))
    {
        DEBUG_LOG("Found method: ", node->memberName);
        auto method = obj->klass->getMethod(node->memberName);
        // Bind method to this object
        auto boundMethod = std::make_shared<Function>(method->declaration, std::static_pointer_cast<void>(obj));
        return Value(std::static_pointer_cast<void>(boundMethod), Value::Type::Function);
    }

    throw std::runtime_error("Undefined property: " + node->memberName);
}
Value Interpreter::evaluateIntegerLiteral(IntegerLiteralNode *node)
{
    return Value(node->value);
}

Value Interpreter::evaluateFloatingPointLiteral(FloatingPointLiteralNode *node)
{
    return Value(node->value);
}

Value Interpreter::evaluateStringLiteral(StringLiteralNode *node)
{
    return Value(node->value);
}

Value Interpreter::evaluateBooleanLiteral(BooleanLiteralNode *node)
{
    return Value(node->value);
}

Value Interpreter::evaluateNullLiteral(NullLiteralNode *node)
{
    return Value(); // Default constructor creates a null value
}

Value Interpreter::callFunction(const std::shared_ptr<Function> &function, const std::vector<Value> &arguments)
{
    if (!function || !function->declaration)
    {
        throw std::runtime_error("Invalid function");
    }

    // Create a new environment for the function execution
    auto env = std::make_shared<Environment>(globals);

    // Define parameters in the function environment
    auto declaration = function->declaration;

    // Check argument count
    if (arguments.size() != declaration->parameters.size())
    {
        throw std::runtime_error("Expected " + std::to_string(declaration->parameters.size()) +
                                 " arguments but got " + std::to_string(arguments.size()));
    }

    // Bind arguments to parameters
    for (size_t i = 0; i < declaration->parameters.size(); i++)
    {
        env->define(declaration->parameters[i]->name, arguments[i]);
    }

    // Set 'this' if the function is a method
    if (function->thisObject)
    {
        env->define("this", Value(function->thisObject, Value::Type::Object));
    }

    try
    {
        // Execute the function body
        executeBlockStatement(declaration->body.get(), env);

        // If no return statement was executed, return null
        return Value();
    }
    catch (const ReturnException &e)
    {
        // Return the value from the return statement
        return e.getValue();
    }
}

Value Interpreter::callNativeFunction(const std::shared_ptr<NativeFunctionWrapper> &function, const std::vector<Value> &arguments)
{
    // Check argument count if specified
    if (function->paramCount >= 0 && arguments.size() != static_cast<size_t>(function->paramCount))
    {
        throw std::runtime_error("Expected " + std::to_string(function->paramCount) +
                                 " arguments but got " + std::to_string(arguments.size()));
    }

    // Call the native function implementation
    return function->function(arguments);
}

Value Interpreter::createInstance(std::shared_ptr<Class> klass, const std::vector<Value> &arguments)
{
    // Create a new object instance
    auto object = std::make_shared<Object>(klass);

    // Create a Value for this object right away
    Value objectValue(std::static_pointer_cast<void>(object), Value::Type::Object);

    // Call constructor if it exists
    if (klass->hasMethod("constructor"))
    {
        auto constructor = klass->getMethod("constructor");

        // Create a function with this bound to the new object
        auto boundConstructor = std::make_shared<Function>(constructor->declaration, std::static_pointer_cast<void>(object));

        try
        {
            // Call constructor
            callFunction(boundConstructor, arguments);
            DEBUG_LOG("Constructor for class ", klass->name, " executed successfully");
        }
        catch (const std::exception &e)
        {
            DEBUG_LOG("Error in constructor for class ", klass->name, ": ", e.what());
            // Continue despite constructor error - object is still created
        }
    }
    else
    {
        DEBUG_LOG("No constructor found for class ", klass->name);
    }

    return objectValue;
}

void Interpreter::defineNativeFunctions()
{
    // Math functions
    auto defineNativeFunc = [this](const std::string &name, int paramCount, NativeFunction func)
    {
        auto wrapper = std::make_shared<NativeFunctionWrapper>(name, paramCount, func);
        environment->define(name, Value(std::static_pointer_cast<void>(wrapper), Value::Type::Function));
    };

    // Math.abs
    defineNativeFunc("abs", 1, [](const std::vector<Value> &args)
                     {
        if (args[0].isInteger()) 
            return Value(std::abs(args[0].asInt()));
        else 
            return Value(std::abs(args[0].asFloat())); });

    // Math.floor
    defineNativeFunc("floor", 1, [](const std::vector<Value> &args)
                     { return Value(static_cast<int>(std::floor(args[0].asFloat()))); });

    // Math.ceil
    defineNativeFunc("ceil", 1, [](const std::vector<Value> &args)
                     { return Value(static_cast<int>(std::ceil(args[0].asFloat()))); });

    // Math.round
    defineNativeFunc("round", 1, [](const std::vector<Value> &args)
                     { return Value(static_cast<int>(std::round(args[0].asFloat()))); });

    // Math.min
    defineNativeFunc("min", 2, [](const std::vector<Value> &args)
                     {
        if (args[0].isInteger() && args[1].isInteger())
            return Value(std::min(args[0].asInt(), args[1].asInt()));
        else
            return Value(std::min(args[0].asFloat(), args[1].asFloat())); });

    // Math.max
    defineNativeFunc("max", 2, [](const std::vector<Value> &args)
                     {
        if (args[0].isInteger() && args[1].isInteger())
            return Value(std::max(args[0].asInt(), args[1].asInt()));
        else
            return Value(std::max(args[0].asFloat(), args[1].asFloat())); });

    // String functions

    // length
    defineNativeFunc("length", 1, [](const std::vector<Value> &args)
                     { return Value(static_cast<int>(args[0].asString().length())); });

    // substr
    defineNativeFunc("substr", 3, [](const std::vector<Value> &args)
                     {
        std::string str = args[0].asString();
        int start = args[1].asInt();
        int length = args[2].asInt();
        
        if (start < 0) start = 0;
        if (start >= static_cast<int>(str.length())) return Value(std::string(""));
        
        return Value(str.substr(start, length)); });

    // Random number generator
    defineNativeFunc("random", 0, [](const std::vector<Value> &args)
                     { return Value(static_cast<float>(rand()) / RAND_MAX); });
}