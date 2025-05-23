#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

// Forward declarations to avoid circular dependencies
class Value;
class Function;
class Environment;
class Interpreter;

// Module registry for handling imported functions
class ModuleRegistry
{
public:
    ModuleRegistry();

    // Register a module's environment and exported functions
    void registerModule(const std::string &moduleName,
                        std::shared_ptr<Environment> moduleEnv);

    // Register a module's exported function
    void registerFunction(const std::string &moduleName,
                          const std::string &functionName,
                          std::shared_ptr<Function> functionObj);

    // Register a module function implementation
    void registerModuleImplementation(
        const std::string &moduleName,
        const std::string &functionName,
        std::function<Value(const std::vector<Value> &)> implementation);

    // Execute a function in its module's environment
    Value executeFunction(const std::string &moduleName,
                          const std::string &functionName,
                          const std::vector<Value> &args);

    // Get a registered function
    std::shared_ptr<Function> getFunction(const std::string &moduleName,
                                          const std::string &functionName);

    // Get a pending module call
    std::shared_ptr<Function> getPendingCall(const std::string &callId);

    // Set the interpreter instance
    void setInterpreter(Interpreter *interp) { interpreter = interp; }

private:
    // Map of modules to their environments
    std::map<std::string, std::shared_ptr<Environment>> moduleEnvironments;

    // Map of modules to their exported functions
    std::map<std::string, std::map<std::string, std::shared_ptr<Function>>> moduleFunctions;

    // Map of module function implementations
    std::map<std::string, std::map<std::string, std::function<Value(const std::vector<Value> &)>>> moduleImplementations;

    // Temporary storage for pending module calls
    std::map<std::string, std::shared_ptr<Function>> pendingModuleCalls;

    // Cache for previously executed module functions
    std::map<std::string, std::function<Value(const std::vector<Value> &)>> executionCache;

    // Handle module-specific functionality
    Interpreter *interpreter; // Will be set by the interpreter
};

// Global registry instance
extern ModuleRegistry gModuleRegistry;