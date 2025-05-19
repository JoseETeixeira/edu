#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include "parser/parser.h"
#include "codegen/code_generator.h"

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

// Function to transpile edu code to C++
std::string transpileCode(const std::string &source)
{
    // Tokenize the source code
    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Parse the tokens into an AST
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> program = parser.parse();

    // Generate C++ code from the AST
    CodeGenerator codeGenerator;
    return codeGenerator.generate(program.get());
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
    std::cout << "  --help         Display this help message" << std::endl;
}

int main(int argc, char *argv[])
{
    // Parse command line arguments
    bool transpileOnly = false;
    std::string inputFile;
    std::string outputFile;

    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    // Process arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--transpile") == 0)
        {
            transpileOnly = true;
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

    if (outputFile.empty() && transpileOnly)
    {
        outputFile = inputFile + ".cpp";
    }

    // Read the input file
    std::string source = readFile(inputFile);
    if (source.empty())
    {
        return 1;
    }

    try
    {
        // Transpile the edu code to C++
        std::string cppCode = transpileCode(source);

        if (transpileOnly)
        {
            // Write the generated code to the output file
            if (!writeFile(outputFile, cppCode))
            {
                return 1;
            }
            std::cout << "Successfully transpiled " << inputFile << " to " << outputFile << std::endl;
        }
        else
        {
            // Create a temporary directory for compilation
            std::string tempDir = fs::temp_directory_path().string();

            // Compile and run the C++ code
            int result = compileAndRun(cppCode, tempDir);
            if (result != 0)
            {
                std::cerr << "Program exited with code " << result << std::endl;
                return result;
            }
        }

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}