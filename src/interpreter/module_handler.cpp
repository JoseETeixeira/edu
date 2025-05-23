#include "module_handler.h"
#include "interpreter.h"
#include "../parser/nodes.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

// Constructor
ModuleRegistry::ModuleRegistry() : interpreter(nullptr)
{
    DEBUG_LOG("Module registry initialized");
}

// Register a module's environment
void ModuleRegistry::registerModule(const std::string &moduleName,
                                    std::shared_ptr<Environment> moduleEnv)
{
    moduleEnvironments[moduleName] = moduleEnv;
    DEBUG_LOG("Registered module environment: ", moduleName);
}

// Register a module's exported function
void ModuleRegistry::registerFunction(const std::string &moduleName,
                                      const std::string &functionName,
                                      std::shared_ptr<Function> functionObj)
{
    moduleFunctions[moduleName][functionName] = functionObj;
    DEBUG_LOG("Registered function ", functionName, " from module ", moduleName);
}

// Register a module function implementation
void ModuleRegistry::registerModuleImplementation(
    const std::string &moduleName,
    const std::string &functionName,
    std::function<Value(const std::vector<Value> &)> implementation)
{
    moduleImplementations[moduleName][functionName] = implementation;
    DEBUG_LOG("Registered implementation for ", moduleName, ".", functionName);
}

// Get a function from the registry
std::shared_ptr<Function> ModuleRegistry::getFunction(const std::string &moduleName,
                                                      const std::string &functionName)
{
    auto moduleIt = moduleFunctions.find(moduleName);
    if (moduleIt == moduleFunctions.end())
    {
        return nullptr;
    }

    auto &functions = moduleIt->second;
    auto funcIt = functions.find(functionName);
    if (funcIt == functions.end())
    {
        return nullptr;
    }

    return funcIt->second;
}

// Execute a function in its module's environment
Value ModuleRegistry::executeFunction(const std::string &moduleName,
                                      const std::string &functionName,
                                      const std::vector<Value> &args)
{
    DEBUG_LOG("ModuleRegistry executing: ", moduleName, ".", functionName);

    // Try different module name variations
    std::vector<std::string> possibleModuleNames = {
        moduleName,
        moduleName + ".edu"};

    // Try to find the function using different module name formats
    std::shared_ptr<Function> function = nullptr;
    std::string actualModuleName;

    for (const auto &modName : possibleModuleNames)
    {
        function = getFunction(modName, functionName);
        if (function)
        {
            actualModuleName = modName;
            DEBUG_LOG("Found function using module name: ", modName);
            break;
        }
    }

    if (!function)
    {
        DEBUG_LOG("Function not found: ", moduleName, ".", functionName);
        return Value();
    }

    // Try to find the module environment using different name formats
    std::shared_ptr<Environment> moduleEnv = nullptr;

    for (const auto &modName : possibleModuleNames)
    {
        auto moduleIt = moduleEnvironments.find(modName);
        if (moduleIt != moduleEnvironments.end())
        {
            moduleEnv = moduleIt->second;
            DEBUG_LOG("Found module environment using name: ", modName);
            break;
        }
    }

    if (!moduleEnv)
    {
        DEBUG_LOG("Module environment not found for: ", moduleName);
        return Value();
    }

    // Execute the imported function from the module

    // First check if there's a registered implementation
    auto moduleImplIt = moduleImplementations.find(moduleName);
    if (moduleImplIt != moduleImplementations.end())
    {
        auto funcImplIt = moduleImplIt->second.find(functionName);
        if (funcImplIt != moduleImplIt->second.end())
        {
            DEBUG_LOG("Using registered implementation for ", moduleName, ".", functionName);
            return funcImplIt->second(args);
        }
    }

    // This is the key part - we're not trying to execute the function ourselves
    // We're just delegating to the already-parsed function from when the module was imported

    // At this point, since we've registered the function during import, we just need to call it
    // with the right environment

    // Create an environment for function execution with module environment as parent
    auto execEnv = std::make_shared<Environment>(moduleEnv);

    // Bind arguments to parameters
    auto paramNames = function->getParameterNames();
    for (size_t i = 0; i < paramNames.size() && i < args.size(); i++)
    {
        execEnv->define(paramNames[i], args[i]);
        DEBUG_LOG("Binding param ", paramNames[i], " = ", args[i].toString());
    }
    // Get the actual function to execute (which should have already been interpreted when imported)
    auto execFunction = function;
    if (function->importedFunction)
    {
        execFunction = function->importedFunction;
        DEBUG_LOG("Using imported function reference");
    }

    // Debug the function structure
    // For module functions, we need to execute them differently to avoid circular calls
    // The key is that the original function from the module should have all the necessary info

    // Save current environment
    auto previousEnv = interpreter->getEnvironment();

    try
    {
        // Set up the execution environment with the module environment as parent
        interpreter->setEnvironment(execEnv);

        // Now we need to execute the function. Since this is a module function,
        // we should have the original function with its body intact.
        if (execFunction && execFunction->declaration && execFunction->declaration->body)
        {

            // Execute the function body in the current environment
            interpreter->execute(execFunction->declaration->body.get());

            // If we get here without a return, return null
            interpreter->setEnvironment(previousEnv);
            return Value();
        }
        else
        {
            DEBUG_LOG("ERROR: Module function missing body. Trying alternative execution.");

            // If the body is missing, try executing through the function's closure
            if (execFunction && execFunction->closure)
            {
                // Create a new environment with the function's closure as parent
                auto funcEnv = std::make_shared<Environment>(execFunction->closure);

                // Bind parameters
                auto paramNames = execFunction->getParameterNames();
                for (size_t i = 0; i < paramNames.size() && i < args.size(); i++)
                {
                    funcEnv->define(paramNames[i], args[i]);
                }

                interpreter->setEnvironment(funcEnv);

                // Try to find and execute the body through the data
                if (execFunction->data && execFunction->data->body)
                {
                    DEBUG_LOG("Executing via function data body");
                    interpreter->execute(execFunction->data->body.get());
                    interpreter->setEnvironment(previousEnv);
                    return Value();
                }
            }

            // If all else fails, restore environment and return null
            interpreter->setEnvironment(previousEnv);
            DEBUG_LOG("Failed to find executable body for module function");
            return Value();
        }
    }
    catch (const ReturnException &e)
    {
        // Restore previous environment
        interpreter->setEnvironment(previousEnv);

        // Return the value from the return statement
        return e.getValue();
    }
    catch (const std::exception &e)
    {
        // Restore previous environment
        interpreter->setEnvironment(previousEnv);
        DEBUG_LOG("Error executing module function: ", e.what());
        throw;
    }

    // If we get here, something went wrong
    DEBUG_LOG("Failed to execute module function: ", moduleName, ".", functionName);
    return Value();
    return Value();
}

// Get a pending module call
std::shared_ptr<Function> ModuleRegistry::getPendingCall(const std::string &callId)
{
    auto it = pendingModuleCalls.find(callId);
    if (it != pendingModuleCalls.end())
    {
        auto func = it->second;
        // Remove it from the pending calls
        pendingModuleCalls.erase(it);
        return func;
    }
    return nullptr;
}

// Global registry instance
ModuleRegistry gModuleRegistry;