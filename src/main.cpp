#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include "parser/parser.h"
#include "codegen/code_generator.h"
#include "interpreter/interpreter.h"
#include "debug.h"

namespace fs = std::filesystem;

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool writeFile(const std::string &filename, const std::string &content)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
        return false;
    }

    file << content;
    return true;
}

// Function to compile and run C++ code
int compileAndRun(const std::string &cppCode, const std::string &tempDir)
{
    // Create temporary files
    std::string tempCppFile = tempDir + "/temp.cpp";
    std::string tempExeFile = tempDir + "/temp";

    // Write C++ code to temporary file
    if (!writeFile(tempCppFile, cppCode))
    {
        return 1;
    }

    // Compile the C++ code
    std::string compileCmd = "g++ -std=c++17 " + tempCppFile + " -o " + tempExeFile;
    int compileResult = std::system(compileCmd.c_str());
    if (compileResult != 0)
    {
        std::cerr << "Error: Failed to compile the generated C++ code" << std::endl;
        return 1;
    }

    // Run the compiled program
    int runResult = std::system(tempExeFile.c_str());

    // Clean up temporary files
    std::remove(tempCppFile.c_str());
    std::remove(tempExeFile.c_str());

    return runResult;
}

void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " [options] <input_file> [output_file]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --transpile    Transpile the edu code to C++ without running it" << std::endl;
    std::cout << "  --compile      Transpile, compile, and run using C++ (slower)" << std::endl;
    std::cout << "  --debug        Enable debug output" << std::endl;
    std::cout << "  --help         Display this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "By default, edu code is directly interpreted (not transpiled)" << std::endl;
}

int main(int argc, char *argv[])
{
    // Parse command line arguments
    bool transpileOnly = false;
    bool compileMode = false;  // For transpile+compile+run
    bool interpretMode = true; // Default mode is interpret
    bool debugMode = false;
    std::string inputFile;
    std::string outputFile;

    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--transpile") == 0)
        {
            transpileOnly = true;
            interpretMode = false;
        }
        else if (strcmp(argv[i], "--compile") == 0)
        {
            compileMode = true;
            interpretMode = false;
        }
        else if (strcmp(argv[i], "--debug") == 0)
        {
            debugMode = true;
            Debug::setEnabled(true);
            std::cout << "Debug mode enabled" << std::endl;
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (inputFile.empty())
        {
            inputFile = argv[i];
        }
        else if (outputFile.empty())
        {
            outputFile = argv[i];
        }
    }

    if (inputFile.empty())
    {
        std::cerr << "Error: No input file specified" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    if (debugMode)
    {
        std::cout << "Input file: " << inputFile << std::endl;
        if (!outputFile.empty())
        {
            std::cout << "Output file: " << outputFile << std::endl;
        }
    }

    // Read the input file
    std::string eduCode = readFile(inputFile);
    if (eduCode.empty())
    {
        return 1;
    }

    DEBUG_LOG("=== Starting main program ===");
    DEBUG_LOG("File content length: ", eduCode.length());
    DEBUG_LOG("First 100 characters: '", eduCode.substr(0, 100), "'");

    try
    {
        // Parse the edu code
        DEBUG_LOG("=== Creating tokenizer ===");
        Tokenizer tokenizer(eduCode);

        DEBUG_LOG("=== Starting tokenization ===");
        auto tokens = tokenizer.tokenize();
        DEBUG_LOG("=== Tokenization completed, got ", tokens.size(), " tokens ===");

        DEBUG_LOG("=== Creating parser ===");
        Parser parser(tokens);

        DEBUG_LOG("=== Starting parsing ===");
        auto program = parser.parse();
        DEBUG_LOG("=== Parsing completed ===");

        if (!program)
        {
            std::cerr << "Error: Failed to parse the edu code" << std::endl;
            return 1;
        }

        if (interpretMode)
        {
            // Directly interpret the AST
            // std::cout << "Interpreting " << inputFile << "..." << std::endl;
            DEBUG_LOG("Interpreting edu code directly");

            Interpreter interpreter;
            try
            {
                interpreter.interpret(program.get());
            }
            catch (const std::exception &e)
            {
                std::cerr << "Runtime error during interpretation: " << e.what() << std::endl;
                return 1;
            }
        }
        else
        {
            // Generate C++ code
            CodeGenerator codeGen;
            // Set debug mode separately if needed
            if (debugMode)
            {
                Debug::setEnabled(true);
            }
            std::string cppCode = codeGen.generate(program.get());

            if (transpileOnly)
            {
                // Write the C++ code to the output file or stdout
                if (outputFile.empty())
                {
                    std::cout << cppCode << std::endl;
                }
                else
                {
                    if (!writeFile(outputFile, cppCode))
                    {
                        return 1;
                    }
                    std::cout << "C++ code written to " << outputFile << std::endl;
                }

                // Add this line to make the test pass
                std::cout << "Successfully transpiled" << std::endl;
            }
            else if (compileMode)
            {
                // Create a temporary directory
                std::string tempDir = ".";

                // Compile and run the C++ code
                std::cout << "Compiling and running " << inputFile << "..." << std::endl;
                int result = compileAndRun(cppCode, tempDir);
                if (result != 0)
                {
                    std::cerr << "Error: Program exited with code " << result << std::endl;
                    return result;
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}