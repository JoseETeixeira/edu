#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <functional>
#include "../debug.h"

// Forward declarations of all node types needed to be handled
class ASTNode;
class ProgramNode;
class BlockStatementNode;
class VariableDeclarationNode;
class FunctionNode;
class ReturnStatementNode;
class IfStatementNode;
class WhileStatementNode;
class ExpressionStatementNode;
class ExpressionNode;
class ConsoleLogNode;
class InputStatementNode;
class VariableExpressionNode;
class AssignmentExpressionNode;
class BinaryExpressionNode;
class CallExpressionNode;
class MemberAccessExpressionNode;
class ClassNode;
class StringLiteralNode;
class IntegerLiteralNode;
class FloatingPointLiteralNode;
class BooleanLiteralNode;
class NullLiteralNode;
class ComparisonExpressionNode;
class EqualityExpressionNode;
class AdditionExpressionNode;
class SubtractionExpressionNode;
class MultiplicationExpressionNode;
class DivisionExpressionNode;
class AndExpressionNode;
class OrExpressionNode;

// Represents a runtime value in the interpreter
class Value
{
public:
    // Use std::variant to store different possible value types
    using ValueVariant = std::variant<
        std::monostate,       // null
        bool,                 // boolean
        int,                  // integer
        float,                // floating point
        std::string,          // string
        std::shared_ptr<void> // For objects, functions, etc.
        >;

    enum class Type
    {
        Null,
        Boolean,
        Integer,
        Float,
        String,
        Object,
        Function,
        Class
    };

    Value() : type(Type::Null), value(std::monostate{}) {}
    Value(bool val) : type(Type::Boolean), value(val) {}
    Value(int val) : type(Type::Integer), value(val) {}
    Value(float val) : type(Type::Float), value(val) {}
    Value(const std::string &val) : type(Type::String), value(val) {}
    Value(std::shared_ptr<void> val, Type t) : type(t), value(val) {}

    // Type checking
    bool isNull() const { return type == Type::Null; }
    bool isBoolean() const { return type == Type::Boolean; }
    bool isInteger() const { return type == Type::Integer; }
    bool isFloat() const { return type == Type::Float; }
    bool isString() const { return type == Type::String; }
    bool isObject() const { return type == Type::Object; }
    bool isFunction() const { return type == Type::Function; }
    bool isClass() const { return type == Type::Class; }

    // Value retrieval
    bool asBool() const;
    int asInt() const;
    float asFloat() const;
    std::string asString() const;
    template <typename T>
    std::shared_ptr<T> asObject() const;

    // String conversion for output
    std::string toString() const;

    // Operators
    Value operator+(const Value &other) const;
    Value operator-(const Value &other) const;
    Value operator*(const Value &other) const;
    Value operator/(const Value &other) const;
    Value operator%(const Value &other) const; // Added modulo operator
    bool operator==(const Value &other) const;
    bool operator!=(const Value &other) const;
    bool operator<(const Value &other) const;
    bool operator>(const Value &other) const;
    bool operator<=(const Value &other) const;
    bool operator>=(const Value &other) const;

    Type getType() const { return type; }
    const ValueVariant &getValue() const { return value; }

private:
    Type type;
    ValueVariant value;
};

// Exception class for handling return statements
class ReturnException : public std::exception
{
public:
    ReturnException(const Value &value) : returnValue(value) {}

    const Value &getValue() const { return returnValue; }

private:
    Value returnValue;
};

// Function and method representation
struct Function
{
    std::shared_ptr<FunctionNode> declaration;
    std::shared_ptr<void> thisObject; // Used for methods

    Function(std::shared_ptr<FunctionNode> decl, std::shared_ptr<void> thisObj = nullptr)
        : declaration(decl), thisObject(thisObj) {}
};

// Native function type (for built-in functions)
using NativeFunction = std::function<Value(const std::vector<Value> &)>;

// Native function wrapper
struct NativeFunctionWrapper
{
    NativeFunction function;
    std::string name;
    int paramCount;

    NativeFunctionWrapper(const std::string &name, int paramCount, NativeFunction func)
        : name(name), paramCount(paramCount), function(func) {}
};

// Class representation
struct Class
{
    std::string name;
    std::map<std::string, std::shared_ptr<Function>> methods;

    Class(const std::string &name) : name(name) {}

    bool hasMethod(const std::string &name) const
    {
        return methods.find(name) != methods.end();
    }

    std::shared_ptr<Function> getMethod(const std::string &name) const
    {
        auto it = methods.find(name);
        if (it != methods.end())
        {
            return it->second;
        }
        return nullptr;
    }
};

// Object representation
struct Object
{
    std::shared_ptr<Class> klass;
    std::map<std::string, Value> fields;

    Object(std::shared_ptr<Class> klass) : klass(klass) {}
};

// Environment to store variables during execution
class Environment
{
public:
    Environment() = default;
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

    void define(const std::string &name, const Value &value)
    {
        values[name] = value;
    }

    Value get(const std::string &name)
    {
        auto it = values.find(name);
        if (it != values.end())
        {
            return it->second;
        }

        if (enclosing)
        {
            return enclosing->get(name);
        }

        throw std::runtime_error("Undefined variable: " + name);
    }

    void assign(const std::string &name, const Value &value)
    {
        auto it = values.find(name);
        if (it != values.end())
        {
            it->second = value;
            return;
        }

        if (enclosing)
        {
            enclosing->assign(name, value);
            return;
        }

        throw std::runtime_error("Undefined variable: " + name);
    }

    bool contains(const std::string &name) const
    {
        if (values.find(name) != values.end())
        {
            return true;
        }

        if (enclosing)
        {
            return enclosing->contains(name);
        }

        return false;
    }

private:
    std::map<std::string, Value> values;
    std::shared_ptr<Environment> enclosing;
};

// Main interpreter class
class Interpreter
{
public:
    Interpreter() : environment(std::make_shared<Environment>())
    {
        globals = environment;
        // Define native functions
        defineNativeFunctions();
    }

    // Main interpretation method - match parameter type with CodeGenerator
    void interpret(ProgramNode *program);

private:
    std::shared_ptr<Environment> environment;
    std::shared_ptr<Environment> globals;
    Value lastValue;   // Store the last evaluated value
    Value currentThis; // Current 'this' pointer for method calls

    // Execution methods for different node types
    void execute(ASTNode *node);
    Value evaluate(ExpressionNode *expr);

    // Statement execution
    void executeBlockStatement(BlockStatementNode *node, std::shared_ptr<Environment> env = nullptr);
    void executeVariableDeclaration(VariableDeclarationNode *node);
    void executeIfStatement(IfStatementNode *node);
    void executeWhileStatement(WhileStatementNode *node);
    void executeReturnStatement(ReturnStatementNode *node);
    void executeExpressionStatement(ExpressionStatementNode *node);
    void executeConsoleLog(ConsoleLogNode *node);
    void executeInputStatement(InputStatementNode *node);
    void executeFunction(FunctionNode *node);
    void executeClass(ClassNode *node);

    // Expression evaluation
    Value evaluateVariableExpression(VariableExpressionNode *node);
    Value evaluateBinaryExpression(BinaryExpressionNode *node);
    Value evaluateCallExpression(CallExpressionNode *node);
    Value evaluateAssignmentExpression(AssignmentExpressionNode *node);
    Value evaluateMemberAccessExpression(MemberAccessExpressionNode *node);
    Value evaluateIntegerLiteral(IntegerLiteralNode *node);
    Value evaluateFloatingPointLiteral(FloatingPointLiteralNode *node);
    Value evaluateStringLiteral(StringLiteralNode *node);
    Value evaluateBooleanLiteral(BooleanLiteralNode *node);
    Value evaluateNullLiteral(NullLiteralNode *node);

    // Function execution
    Value callFunction(const std::shared_ptr<Function> &function, const std::vector<Value> &arguments);
    Value callNativeFunction(const std::shared_ptr<NativeFunctionWrapper> &function, const std::vector<Value> &arguments);

    // Helper methods
    void defineNativeFunctions();
    void lookForMainFunction(ProgramNode *program);

    // Create object instance helper
    Value createInstance(std::shared_ptr<Class> klass, const std::vector<Value> &arguments);
};