#include "tokenizer.h"
#include "../debug.h"

Token Tokenizer::nextToken()
{
    skipWhitespaceAndComments();

    if (position >= source.length())
    {
        return {TokenType::EndOfFile, "", line};
    }

    char currentChar = source[position];

    // Check for identifier or keyword
    if (isalpha(currentChar) || currentChar == '_')
    {
        return identifierOrKeyword();
    }

    // Check for numeric literal
    if (isdigit(currentChar))
    {
        return numericLiteral();
    }

    // Check for string literal
    if (currentChar == '"')
    {
        return stringLiteral();
    }

    if (currentChar == '\'')
    {
        return characterLiteral();
    }

    // Check for operators and punctuators
    if (isOperatorOrPunctuatorStart(currentChar))
    {
        return operatorOrPunctuator();
    }

    if (isUnknownCharacter(currentChar))
    {
        std::cerr << "Error: Unknown character '" << currentChar << "' at line "
                  << line << std::endl;
        return {TokenType::Unknown, std::string(1, currentChar), line};
    }

    return {TokenType::Unknown, std::string(1, currentChar), line};
}

std::vector<Token> Tokenizer::tokenize()
{
    DEBUG_LOG("=== Starting tokenization ===");
    std::vector<Token> tokens;
    Token token;

    do
    {
        token = nextToken();
        tokens.push_back(token);

    } while (token.type != TokenType::EndOfFile);

    DEBUG_LOG("=== Finished tokenization with ", tokens.size(), " tokens ===");
    return tokens;
}

bool Tokenizer::isKeyword(const std::string &value)
{
    static const std::set<std::string> keywords = {
        "bool", "char", "int", "float", "double", "void",
        "wchar_t", "string", "Error", "interface", "function", "const",
        "export", "extends", "async", "await", "null", "true",
        "false", "try", "catch", "import", "from", "template",
        "copy", "for", "while", "if", "else", "switch",
        "case", "default", "break", "continue", "return", "try",
        "catch", "throw", "print"
        // Add other keywords specific to your language
    };

    return keywords.find(value) != keywords.end();
}

Token Tokenizer::characterLiteral()
{
    position++; // Skip opening quote
    if (position >= source.length())
    {
        std::cerr << "Error: Unterminated character literal at line " << line
                  << std::endl;
        return {TokenType::Unknown, "", line};
    }

    char charValue = source[position++];

    // Handle escape sequences in char literals if needed
    if (charValue == '\\' && position < source.length())
    {
        charValue = processEscapeSequence(source[position++]);
    }

    if (position >= source.length() || source[position] != '\'')
    {
        std::cerr << "Error: Unterminated character literal at line " << line
                  << std::endl;
        return {TokenType::Unknown, "", line};
    }

    position++; // Skip closing quote

    // Convert charValue to a string
    return {TokenType::Character, std::string(1, charValue), line};
}

bool Tokenizer::isOperatorOrPunctuatorStart(char ch)
{
    return isOperator(ch) || isPunctuator(ch);
}

bool Tokenizer::isOperator(char ch)
{
    static const std::string opStartChars = "+-*/%=&|<>!.";
    return opStartChars.find(ch) != std::string::npos;
}

bool Tokenizer::isUnknownCharacter(char ch)
{
    static const std::string validChars =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        "+-*/%=&|<>!.,;()[]{}: \t\n" // Include ':' for generics syntax
        "'\"\\`";                    // Include backtick (`) if template strings are supported

    return validChars.find(ch) == std::string::npos;
}

void Tokenizer::skipWhitespaceAndComments()
{
    while (position < source.length())
    {
        if (isspace(source[position]))
        {
            // Handle new lines
            if (source[position] == '\n')
            {
                line++;
            }
            position++;
        }
        else if (source[position] == '/' && position + 1 < source.length())
        {
            if (source[position + 1] == '/')
            {
                // Single-line comment
                position += 2;
                while (position < source.length() && source[position] != '\n')
                {
                    position++;
                }
            }
            else if (source[position + 1] == '*')
            {
                // Multi-line comment
                position += 2;
                while (position + 1 < source.length() &&
                       !(source[position] == '*' && source[position + 1] == '/'))
                {
                    if (source[position] == '\n')
                    {
                        line++;
                    }
                    position++;
                }
                if (position < source.length())
                {
                    position += 2; // Skip the closing '*/'
                }
            }
            else
            {
                break; // Not a comment, so break out of the loop
            }
        }
        else
        {
            break; // Not whitespace or comment, so break out of the loop
        }
    }
}

Token Tokenizer::identifierOrKeyword()
{
    size_t start = position++;
    while (position < source.length() &&
           (isalnum(source[position]) || source[position] == '_'))
    {
        position++;
    }

    std::string value = source.substr(start, position - start);

    if (isDeclarationKeyword(value))
    {
        return {TokenType::Declaration, value, line};
    }

    if (isKeyword(value))
    {
        return {TokenType::Keyword, value, line};
    }

    return {TokenType::Identifier, value, line};
}

bool Tokenizer::isDeclarationKeyword(const std::string &value)
{
    static const std::set<std::string> declarationKeywords = {
        "class", "function", "const", "interface",
        "async" // Add other declaration keywords
    };

    return declarationKeywords.find(value) != declarationKeywords.end();
}

Token Tokenizer::numericLiteral()
{
    size_t start = position++;
    bool hasDecimalPoint = false;

    while (position < source.length() &&
           (isdigit(source[position]) || source[position] == '.'))
    {
        if (source[position] == '.')
        {
            if (hasDecimalPoint)
                break; // Avoid multiple decimal points
            hasDecimalPoint = true;
        }
        position++;
    }

    return {TokenType::Number, source.substr(start, position - start), line};
}

Token Tokenizer::stringLiteral()
{
    char quoteType = source[position++];
    size_t start = position;
    std::string value;

    while (position < source.length() && source[position] != quoteType)
    {
        if (source[position] == '\\' && position + 1 < source.length())
        {
            position++;
            value += processEscapeSequence(source[position]);
        }
        else
        {
            value += source[position];
        }
        position++;
    }

    if (position == source.length())
    {
        std::cerr << "Error: Unterminated string literal at line " << line
                  << std::endl;
        return {TokenType::Unknown, "",
                line}; // Or handle this error as appropriate
    }

    position++; // Skip the closing quote
    return {TokenType::String, value, line};
}

char Tokenizer::processEscapeSequence(char escapedChar)
{
    switch (escapedChar)
    {
    case 'n':
        return '\n';
    case 't':
        return '\t';
    case '\\':
        return '\\';
    case '"':
        return '\"';
    case '\'':
        return '\'';
    // Add other cases as needed
    default:
        return escapedChar;
    }
}

Token Tokenizer::operatorOrPunctuator()
{
    char currentChar = source[position];

    // Handle punctuators first (single characters)
    if (isPunctuator(currentChar))
    {
        position++; // Advance position BEFORE creating token
        return {TokenType::Punctuator, std::string(1, currentChar), line};
    }
    else
    {
        // Handle multi-character operators
        size_t start = position;
        position++; // Advance position for first character

        if (position < source.length())
        {
            std::string potentialOperator = source.substr(start, 2);
            if (isMultiCharacterOperator(potentialOperator))
            {
                position++; // Advance position for second character
                return {TokenType::Operator, potentialOperator, line};
            }
        }

        // Single character operator
        return {TokenType::Operator, std::string(1, currentChar), line};
    }
}

bool Tokenizer::isPunctuator(char ch)
{
    static const std::string punctuators = ";,(){}[]:"; // Make sure colon is included!
    return punctuators.find(ch) != std::string::npos;
}

bool Tokenizer::isMultiCharacterOperator(const std::string &op)
{
    static const std::set<std::string> multiCharOperators = {
        "&&", "||", "==", "!=", "<=", ">=", "+=", "-=", "*=", "/=", "++", "--"};
    return multiCharOperators.find(op) != multiCharOperators.end();
}