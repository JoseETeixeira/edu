#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "parser/parser.h"
#include "codegen/code_generator.h"

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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : inputFile + ".cpp";

    // Read the input file
    std::string source = readFile(inputFile);
    if (source.empty())
    {
        return 1;
    }

    try
    {
        // Tokenize the source code
        Tokenizer tokenizer(source);
        std::vector<Token> tokens = tokenizer.tokenize();

        // Parse the tokens into an AST
        Parser parser(tokens);
        std::unique_ptr<ProgramNode> program = parser.parse();

        // Generate C++ code from the AST
        CodeGenerator codeGenerator;
        std::string cppCode = codeGenerator.generate(program.get());

        // Write the generated code to the output file
        if (!writeFile(outputFile, cppCode))
        {
            return 1;
        }

        std::cout << "Successfully transpiled " << inputFile << " to " << outputFile << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}