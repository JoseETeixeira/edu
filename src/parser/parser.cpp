#include "parser.h"
#include "../debug.h"

bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }

void Parser::error(const std::string &message)
{
    // Error handling logic
    std::cout << message << std::endl;
    throw std::runtime_error(message);
}

std::unique_ptr<ProgramNode> Parser::parse()
{
    DEBUG_LOG("=== Starting Parser::parse() ===");
    auto program = std::make_unique<ProgramNode>(0); // Assuming 0 as the starting line

    int declarationCount = 0;
    while (!isAtEnd())
    {
        DEBUG_LOG("=== Parsing declaration #", declarationCount++, " ===");
        DEBUG_LOG("Current token: type=", static_cast<int>(peek().type), " value='", peek().value, "' line=", peek().line);

        try
        {
            auto declaration = parseDeclaration();
            DEBUG_LOG("Successfully parsed declaration");
            program->children.push_back(std::move(declaration));
        }
        catch (const std::runtime_error &e)
        {
            DEBUG_LOG("Error parsing declaration: ", e.what());
            std::cout << e.what() << std::endl;
            throw std::runtime_error(e.what());
        }

        DEBUG_LOG("After parsing declaration, current token: type=", static_cast<int>(peek().type), " value='", peek().value, "'");
    }

    DEBUG_LOG("=== Finished Parser::parse() with ", program->children.size(), " declarations ===");
    return program;
}

// Main Parsing Methods

std::unique_ptr<ASTNode> Parser::parseDeclaration()
{
    DEBUG_LOG("=== Starting parseDeclaration ===");
    DEBUG_LOG("Current token: type=", static_cast<int>(peek().type), " value='", peek().value, "'");
    DEBUG_LOG("Next token: type=", static_cast<int>(peekNext().type), " value='", peekNext().value, "'");

    if (match(TokenType::Keyword, "export"))
    {
        DEBUG_LOG("Matched export");
        auto exportedItem = parseDeclaration();
        std::unique_ptr<ExportNode> node =
            std::make_unique<ExportNode>(previous().line);
        node->exportItem = std::move(exportedItem);
        return node;
    }
    else if (match(TokenType::Keyword, "template"))
    {
        DEBUG_LOG("Matched template");
        return parseTemplateDeclaration();
    }
    else if (peek().value == "class" && peek().type == TokenType::Declaration)
    {
        DEBUG_LOG("Matched class declaration");
        return parseClassDeclaration();
    }
    else if (peek().type == TokenType::Keyword &&
             peekNext().type == TokenType::Declaration &&
             peekNext().value == "function")
    {
        DEBUG_LOG("Matched function with return type");
        return parseFunctionDeclaration();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "async" &&
             peekNext().type == TokenType::Declaration && peekNext().value == "function")
    {
        DEBUG_LOG("Matched async function");
        return parseFunctionDeclaration();
    }
    else if (match(TokenType::Keyword, "interface"))
    {
        DEBUG_LOG("Matched interface");
        return parseInterfaceDeclaration();
    }
    else if (match(TokenType::Keyword, "if"))
    {
        DEBUG_LOG("Matched if");
        return parseIfStatement();
    }
    else if (match(TokenType::Keyword, "for"))
    {
        DEBUG_LOG("Matched for");
        return parseForStatement();
    }
    else if (match(TokenType::Keyword, "while"))
    {
        DEBUG_LOG("Matched while");
        return parseWhileStatement();
    }
    else if (match(TokenType::Keyword, "return"))
    {
        DEBUG_LOG("Matched return");
        return parseReturnStatement();
    }
    else if (match(TokenType::Keyword, "break"))
    {
        DEBUG_LOG("Matched break");
        return parseBreakStatement();
    }
    else if (match(TokenType::Keyword, "continue"))
    {
        DEBUG_LOG("Matched continue");
        return parseContinueStatement();
    }
    else if (check(TokenType::Keyword, "switch")) // Using check() instead of match()
    {
        DEBUG_LOG("Matched switch");
        return parseSwitchStatement();
    }
    else if (match(TokenType::Keyword, "try"))
    {
        DEBUG_LOG("Matched try");
        return parseTryCatchStatement();
    }
    else if (match(TokenType::Keyword, "export"))
    {
        DEBUG_LOG("Matched export (duplicate)");
        return parseExportStatement();
    }
    else if (match(TokenType::Keyword, "import"))
    {
        DEBUG_LOG("Matched import");
        return parseImportStatement();
    }
    else if (match(TokenType::Keyword, "null"))
    {
        DEBUG_LOG("Matched null");
        return parseNullReference();
    }
    else if (check(TokenType::Keyword, "print")) // Using check() instead of match()
    {
        DEBUG_LOG("Matched print");
        return parseConsoleLog();
    }
    else if (match(TokenType::Keyword, "await"))
    {
        DEBUG_LOG("Matched await");
        return parseAwaitExpression();
    }
    else if (match(TokenType::Keyword, "input"))
    {
        DEBUG_LOG("Matched input");
        return parseInputStatement();
    }
    else if (match(TokenType::Keyword, "") ||
             match(TokenType::Keyword, "const"))
    {
        DEBUG_LOG("Matched variable declaration");
        return parseVariableDeclaration(previous().value);
    }

    DEBUG_LOG("No match found, falling back to parseStatement");
    return parseStatement();
}

std::unique_ptr<FunctionParameterNode> Parser::parseFunctionParameter()
{
    std::unique_ptr<TypeNode> paramType;
    if (isType(peek().value))
    {
        // Parse parameter type if present
        paramType = parseType();
    }
    std::string paramName =
        consume(TokenType::Identifier, "", "Expected parameter name").value;

    std::unique_ptr<FunctionParameterNode> parameter =
        std::make_unique<FunctionParameterNode>(paramName, previous().line);
    parameter->type = std::move(paramType);
    return parameter;
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{
    DEBUG_LOG("=== Starting parseStatement ===");
    DEBUG_LOG("Current token: type=", static_cast<int>(peek().type), " value='", peek().value, "'");

    if (peek().type == TokenType::Punctuator && peek().value == "{")
    {
        DEBUG_LOG("Parsing block statement");
        return parseBlockStatement();
    }
    else if (peek().type == TokenType::Punctuator && peek().value == ";")
    {
        DEBUG_LOG("Parsing empty statement");
        // Handle empty statements
        advance(); // Consume the semicolon
        return std::make_unique<ExpressionStatementNode>(
            std::make_unique<NullLiteralNode>(previous().line), previous().line);
    }
    // Add handling for specific statement types
    else if (peek().type == TokenType::Keyword && peek().value == "print")
    {
        DEBUG_LOG("Parsing print statement");
        return parseConsoleLog();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "if")
    {
        DEBUG_LOG("Parsing if statement");
        return parseIfStatement();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "for")
    {
        DEBUG_LOG("Parsing for statement");
        return parseForStatement();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "while")
    {
        DEBUG_LOG("Parsing while statement");
        return parseWhileStatement();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "return")
    {
        DEBUG_LOG("Parsing return statement");
        return parseReturnStatement();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "break")
    {
        DEBUG_LOG("Parsing break statement");
        return parseBreakStatement();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "continue")
    {
        DEBUG_LOG("Parsing continue statement");
        return parseContinueStatement();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "switch")
    {
        DEBUG_LOG("Parsing switch statement");
        return parseSwitchStatement();
    }
    else if (peek().type == TokenType::Keyword && peek().value == "try")
    {
        DEBUG_LOG("Parsing try statement");
        return parseTryCatchStatement();
    }
    // Check for variable declaration with class instantiation or imported type
    // Pattern: Identifier Identifier = expression
    else if (peek().type == TokenType::Identifier &&
             peekNext().type == TokenType::Identifier &&
             peekNextNext().type == TokenType::Operator &&
             peekNextNext().value == "=")
    {
        DEBUG_LOG("Parsing variable declaration with class instantiation");
        std::string typeName = peek().value;
        advance(); // Consume the class name

        std::string varName = peek().value;
        advance(); // Consume the variable name

        advance(); // Consume the equals sign

        auto expr = parseExpression();

        // Consume the semicolon
        if (peek().type == TokenType::Punctuator && peek().value == ";")
        {
            advance(); // Consume the semicolon
        }
        else
        {
            std::cout << "Expected ';' after variable declaration, got: "
                      << "Type: " << static_cast<int>(peek().type)
                      << ", Value: '" << peek().value << "'" << std::endl;
        }

        auto varDecl = std::make_unique<VariableDeclarationNode>(varName, previous().line);
        varDecl->typeName = typeName;
        varDecl->initializer = std::move(expr);
        varDecl->isConst = false; // Ensure it's not const

        return varDecl;
    }
    // Parse an expression statement
    else
    {
        DEBUG_LOG("Parsing expression statement (fallback)");
        auto expr = parseExpression();

        // Consume the semicolon
        if (peek().type == TokenType::Punctuator && peek().value == ";")
        {
            advance(); // Consume the semicolon
        }
        else
        {
            std::cout << "Expected ';' after expression, got: "
                      << "Type: " << static_cast<int>(peek().type)
                      << ", Value: '" << peek().value << "'" << std::endl;
        }

        return std::make_unique<ExpressionStatementNode>(std::move(expr), previous().line);
    }
}

std::unique_ptr<ExpressionNode> Parser::parseExpression()
{
    if (check(TokenType::Punctuator, "["))
    {
        return parseArrayLiteral();
    }
    else if (check(TokenType::Punctuator, "{"))
    {
        return parseObjectLiteral();
    }
    else
    {
        auto expr = parseAssignmentExpression();

        // Check for member access
        while (peek().type == TokenType::Operator && peek().value == ".")
        {
            advance(); // Consume the dot

            // Get the member name
            std::string memberName = consume(TokenType::Identifier, "", "Expected member name after '.'").value;

            // Create a member access expression
            auto memberAccessNode = std::make_unique<MemberAccessExpressionNode>(previous().line);
            memberAccessNode->object = std::move(expr);
            memberAccessNode->memberName = memberName;

            // Check if this is a method call
            if (peek().type == TokenType::Punctuator && peek().value == "(")
            {
                advance(); // Consume the opening parenthesis

                // Parse arguments
                std::vector<std::unique_ptr<ExpressionNode>> arguments;
                if (!check(TokenType::Punctuator, ")"))
                {
                    do
                    {
                        arguments.push_back(parseExpression());
                    } while (match(TokenType::Punctuator, ","));
                }

                // Consume the closing parenthesis
                consume(TokenType::Punctuator, ")", "Expected ')' after arguments");

                // Create a call expression
                auto callNode = std::make_unique<CallExpressionNode>(previous().line);
                callNode->callee = std::move(memberAccessNode);
                callNode->arguments = std::move(arguments);

                expr = std::move(callNode);
            }
            else
            {
                expr = std::move(memberAccessNode);
            }
        }

        return expr;
    }
}

std::unique_ptr<ExpressionNode> Parser::parseAssignmentExpression()
{
    auto left = parseOrExpression(); // Start with the next level of precedence,
                                     // which might be logical OR.

    if (match(TokenType::Operator, "=") || match(TokenType::Operator, "+=") ||
        match(TokenType::Operator,
              "-=") /* ... other assignment operators ... */)
    {
        std::string operatorValue = previous().value;
        auto right =
            parseAssignmentExpression(); // Recursively parse the right-hand side

        // Ensure the left-hand side is a valid lvalue (this is language specific
        // and may involve more checks)
        if (dynamic_cast<VariableExpressionNode *>(left.get()) ||
            dynamic_cast<MemberAccessExpressionNode *>(left.get()))
        {
            std::unique_ptr<AssignmentExpressionNode> node =
                std::make_unique<AssignmentExpressionNode>(operatorValue,
                                                           previous().line);
            node->left = std::move(left);
            node->right = std::move(right);
            return node;
        }
        else
        {
            error("Invalid left-hand side in assignment");
        }
    }

    return left; // If no assignment operator is found, just return the
                 // left-hand expression
}

std::unique_ptr<ExpressionNode> Parser::parseOrExpression()
{
    auto left = parseAndExpression(); // Start with the next level of
                                      // precedence, which might be logical AND.

    while (match(TokenType::Operator, "||"))
    {
        std::string operatorValue = previous().value;
        auto right = parseAndExpression(); // Recursively parse the right operand.
        left = std::make_unique<OrExpressionNode>(
            std::move(left), operatorValue, std::move(right), previous().line);
    }

    return left; // If no OR operator is found, just return the left operand
                 // expression.
}

std::unique_ptr<ExpressionNode> Parser::parseAndExpression()
{
    auto left =
        parseEqualityExpression(); // Start with the next level of precedence,
                                   // which might be equality expressions.

    while (match(TokenType::Operator, "&&"))
    {
        std::string operatorValue = previous().value;
        auto right =
            parseEqualityExpression(); // Recursively parse the right operand.
        left = std::make_unique<AndExpressionNode>(
            std::move(left), operatorValue, std::move(right), previous().line);
    }

    return left; // If no AND operator is found, just return the left operand
                 // expression.
}

std::unique_ptr<ExpressionNode> Parser::parseEqualityExpression()
{
    auto left =
        parseComparisonExpression(); // Start with the next level of precedence.

    while (match(TokenType::Operator, "==") || match(TokenType::Operator, "!="))
    {
        std::string operatorValue = previous().value;
        auto right =
            parseComparisonExpression(); // Recursively parse the right operand.
        left = std::make_unique<EqualityExpressionNode>(
            std::move(left), operatorValue, std::move(right), previous().line);
    }

    return left; // If no equality operator is found, just return the left
                 // operand expression.
}
std::unique_ptr<ExpressionNode> Parser::parseComparisonExpression()
{
    auto left =
        parseAdditionExpression(); // Start with the next level of precedence.

    while (match(TokenType::Operator, "<") || match(TokenType::Operator, ">") ||
           match(TokenType::Operator, "<=") || match(TokenType::Operator, ">="))
    {

        std::string operatorValue = previous().value;
        auto right =
            parseAdditionExpression(); // Recursively parse the right operand.
        left = std::make_unique<ComparisonExpressionNode>(
            std::move(left), operatorValue, std::move(right), previous().line);
    }

    return left; // If no comparison operator is found, just return the left
                 // operand expression.
}

std::unique_ptr<ExpressionNode> Parser::parseAdditionExpression()
{
    auto left = parseMultiplicationExpression(); // Start with the next level of precedence

    while (true)
    {
        if (match(TokenType::Operator, "+"))
        {
            // Before parsing the right operand, check if it's a member access/method call
            // If so, parse it completely with higher precedence
            auto right = parseMultiplicationExpression(); // Parse the right operand

            // Check if the right operand needs member access/method call parsing
            while (peek().type == TokenType::Operator && peek().value == ".")
            {
                advance(); // Consume the dot
                std::string memberName = consume(TokenType::Identifier, "", "Expected member name after '.'").value;

                auto memberAccess = std::make_unique<MemberAccessExpressionNode>(previous().line);
                memberAccess->object = std::move(right);
                memberAccess->memberName = memberName;

                // Check for method call
                if (peek().type == TokenType::Punctuator && peek().value == "(")
                {
                    advance(); // Consume opening parenthesis
                    std::vector<std::unique_ptr<ExpressionNode>> arguments;
                    if (!check(TokenType::Punctuator, ")"))
                    {
                        do
                        {
                            arguments.push_back(parseExpression());
                        } while (match(TokenType::Punctuator, ","));
                    }
                    consume(TokenType::Punctuator, ")", "Expected ')' after arguments");

                    auto callExpr = std::make_unique<CallExpressionNode>(previous().line);
                    callExpr->callee = std::move(memberAccess);
                    callExpr->arguments = std::move(arguments);
                    right = std::move(callExpr);
                }
                else
                {
                    right = std::move(memberAccess);
                }
            }

            left = std::make_unique<AdditionExpressionNode>(
                std::move(left), "+", std::move(right), previous().line);
        }
        else if (match(TokenType::Operator, "-"))
        {
            // Similar handling for subtraction
            auto right = parseMultiplicationExpression(); // Parse the right operand

            // Check if the right operand needs member access/method call parsing
            while (peek().type == TokenType::Operator && peek().value == ".")
            {
                advance(); // Consume the dot
                std::string memberName = consume(TokenType::Identifier, "", "Expected member name after '.'").value;

                auto memberAccess = std::make_unique<MemberAccessExpressionNode>(previous().line);
                memberAccess->object = std::move(right);
                memberAccess->memberName = memberName;

                // Check for method call
                if (peek().type == TokenType::Punctuator && peek().value == "(")
                {
                    advance(); // Consume opening parenthesis
                    std::vector<std::unique_ptr<ExpressionNode>> arguments;
                    if (!check(TokenType::Punctuator, ")"))
                    {
                        do
                        {
                            arguments.push_back(parseExpression());
                        } while (match(TokenType::Punctuator, ","));
                    }
                    consume(TokenType::Punctuator, ")", "Expected ')' after arguments");

                    auto callExpr = std::make_unique<CallExpressionNode>(previous().line);
                    callExpr->callee = std::move(memberAccess);
                    callExpr->arguments = std::move(arguments);
                    right = std::move(callExpr);
                }
                else
                {
                    right = std::move(memberAccess);
                }
            }

            left = std::make_unique<SubtractionExpressionNode>(
                std::move(left), "-", std::move(right), previous().line);
        }
        else
        {
            break; // No more addition or subtraction operators
        }
    }

    return left; // Return the built expression node
}

std::unique_ptr<ExpressionNode> Parser::parseMultiplicationExpression()
{
    auto left =
        parseUnaryExpression(); // Start with the next level of precedence.

    while (true)
    {
        if (match(TokenType::Operator, "*"))
        {
            auto right = parseUnaryExpression(); // Parse the right operand.
            left = std::make_unique<MultiplicationExpressionNode>(
                std::move(left), "*", std::move(right), previous().line);
        }
        else if (match(TokenType::Operator, "/"))
        {
            auto right = parseUnaryExpression(); // Parse the right operand.
            left = std::make_unique<DivisionExpressionNode>(
                std::move(left), "/", std::move(right), previous().line);
        }
        else if (match(TokenType::Operator, "%"))
        {
            auto right = parseUnaryExpression(); // Parse the right operand.
            // Use the BinaryExpressionNode for modulo since we don't have a specific ModuloExpressionNode
            auto binaryNode = std::make_unique<BinaryExpressionNode>("%", previous().line);
            binaryNode->left = std::move(left);
            binaryNode->right = std::move(right);
            left = std::move(binaryNode);
        }
        else
        {
            break; // No more multiplication, division, or modulo operators.
        }
    }
    return left; // Return the built expression node.
}

std::unique_ptr<ExpressionNode> Parser::parseUnaryExpression()

{
    // Check for prefix unary operators (++, --, -, !)
    if (match(TokenType::Operator, "++") || match(TokenType::Operator, "--") ||
        match(TokenType::Operator, "-") || match(TokenType::Operator, "!"))
    {
        std::string operatorValue = previous().value; // Get the unary operator
        auto operand = parseUnaryExpression();        // Recursively parse the operand
        std::unique_ptr<UnaryExpressionNode> node =
            std::make_unique<UnaryExpressionNode>(operatorValue, previous().line);
        node->operand = std::move(operand);
        node->isPrefix = true; // Mark as prefix operator
        return node;
    }

    // Parse the primary expression first
    auto expr = parsePrimaryExpression();

    // Check for postfix operators (++ and --)
    if (match(TokenType::Operator, "++") || match(TokenType::Operator, "--"))
    {
        std::string operatorValue = previous().value;
        std::unique_ptr<UnaryExpressionNode> node =
            std::make_unique<UnaryExpressionNode>(operatorValue, previous().line);
        node->operand = std::move(expr);
        node->isPrefix = false; // Mark as postfix operator
        return node;
    }

    // Check for member access and function calls
    while (true)
    {
        if (match(TokenType::Punctuator, "("))
        {
            // It's a function call
            expr = parseCallExpression(std::move(expr));
        }
        else if (match(TokenType::Punctuator, "."))
        {
            // It's a member access
            expr = parseMemberAccessExpression(std::move(expr));
        }
        else
        {
            break;
        }
    }

    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parsePrimaryExpression()
{
    if (match(TokenType::Number, "") || match(TokenType::String, "") ||
        match(TokenType::Character, "") || match(TokenType::Keyword, "true") ||
        match(TokenType::Keyword, "false") || match(TokenType::Keyword, "null"))
    {
        return parseLiteral();
    }
    else if (match(TokenType::Identifier, ""))
    {
        std::string identifier = previous().value;

        if (match(TokenType::Punctuator, "("))
        {
            // It's a function call or object creation
            std::vector<std::unique_ptr<ExpressionNode>> arguments;
            if (!check(TokenType::Punctuator, ")"))
            {
                do
                {
                    arguments.push_back(parseExpression());
                } while (match(TokenType::Punctuator, ","));
            }
            consume(TokenType::Punctuator, ")", "Expected ')' after arguments");
            std::unique_ptr<CallExpressionNode> callNode =
                std::make_unique<CallExpressionNode>(previous().line);
            callNode->arguments = std::move(arguments);
            callNode->callee =
                std::make_unique<VariableExpressionNode>(identifier, previous().line);
            return callNode;
        }
        else if (match(TokenType::Punctuator, "."))
        {
            // It's a member access
            consume(TokenType::Identifier, "", "Expected member name after '.'");
            std::string memberName = previous().value;
            std::unique_ptr<MemberAccessExpressionNode> memberAccessNode =
                std::make_unique<MemberAccessExpressionNode>(previous().line);
            memberAccessNode->memberName = memberName;
            memberAccessNode->object =
                std::make_unique<VariableExpressionNode>(identifier, previous().line);
            return memberAccessNode;
        }

        // Just an identifier
        return std::make_unique<VariableExpressionNode>(identifier,
                                                        previous().line);
    }
    else if (match(TokenType::Punctuator, "["))
    {
        return parseArrayLiteral();
    }
    else if (match(TokenType::Punctuator, "{"))
    {
        return parseObjectLiteral();
    }
    else if (match(TokenType::Keyword, "function"))
    {
        return parseAnonymousFunction();
    }
    else if (peek().value == "new")
    {
        return parseAnonymousFunction();
    }
    else if (match(TokenType::Punctuator, "("))
    {
        auto expr = parseExpression();
        consume(TokenType::Punctuator, ")", "Expected ')' after expression");
        return expr;
    }
    else if (peek().type == TokenType::Operator && peek().value == ".")
    {
        // It's a member access expression
        advance(); // Consume the dot
        std::string memberName = consume(TokenType::Identifier, "", "Expected member name after '.'").value;

        // Create a member access expression
        std::unique_ptr<MemberAccessExpressionNode> memberAccessNode =
            std::make_unique<MemberAccessExpressionNode>(previous().line);
        memberAccessNode->memberName = memberName;

        // The object is the result of the previous expression
        memberAccessNode->object = parseExpression();

        return memberAccessNode;
    }
    else
    {
        std::cout << "Unexpected token in primary expression: "
                  << "Type: " << static_cast<int>(peek().type)
                  << ", Value: '" << peek().value << "'" << std::endl;
        throw std::runtime_error("Unexpected token in primary expression");
    }
}

std::unique_ptr<NullReferenceNode> Parser::parseNullReference()
{
    // Ensure that the current token is 'null'
    if (!match(TokenType::Keyword, "null"))
    {
        error("Expected 'null'");
    }

    // Create a NullReferenceNode
    auto nullNode = std::make_unique<NullReferenceNode>(previous().line);

    // Return the created null reference node
    return nullNode;
}

bool Parser::isType(const std::string &keyword)
{
    // Check if the keyword is a valid type
    static const std::set<std::string> validTypes = {
        "bool", "char", "int", "float", "double",
        "void", "wchar_t", "string", "Error"};
    return validTypes.find(keyword) != validTypes.end();
}

std::unique_ptr<ConsoleLogNode> Parser::parseConsoleLog()
{
    DEBUG_LOG("=== Starting parseConsoleLog ===");
    DEBUG_LOG("Current token: type=", static_cast<int>(peek().type), " value='", peek().value, "'");

    // Consume the 'print' keyword first
    consume(TokenType::Keyword, "print", "Expected 'print' keyword");
    DEBUG_LOG("Consumed 'print' keyword");

    // Consume the opening parenthesis
    consume(TokenType::Punctuator, "(", "Expected '(' after 'print'");
    DEBUG_LOG("Consumed opening parenthesis");

    // Parse the expression to be logged
    DEBUG_LOG("About to parse print expression");
    auto expression = parseExpression();
    DEBUG_LOG("Parsed print expression");

    // Consume the closing parenthesis
    consume(TokenType::Punctuator, ")", "Expected ')' after print expression");
    DEBUG_LOG("Consumed closing parenthesis");

    // Consume the semicolon at the end of the print statement
    consume(TokenType::Punctuator, ";", "Expected ';' after print statement");
    DEBUG_LOG("Consumed semicolon");

    // Create and return a new ConsoleLogNode
    auto console_log_node = std::make_unique<ConsoleLogNode>(previous().line);
    console_log_node->expression = std::move(expression);

    DEBUG_LOG("=== Finished parseConsoleLog ===");
    return console_log_node;
}

std::unique_ptr<InputStatementNode> Parser::parseInputStatement()
{
    // Ensure the current token is the 'input' keyword
    consume(TokenType::Keyword, "input", "Expected 'input'");

    // Optionally, parse a variable declaration if it's part of the input syntax
    std::unique_ptr<VariableDeclarationNode> variable;
    if (match(TokenType::Identifier, "input"))
    {
        std::string variableName = previous().value;
        consume(TokenType::Operator, "=",
                "Expected '='"); // Assuming the syntax is 'input variableName =
                                 // input();'

        // Parse the input function call (assuming it's represented as a call)
        consume(TokenType::Keyword, "input",
                "Expected 'input'"); // Consuming the actual input keyword again
                                     // if the syntax is like 'input()'
        consume(TokenType::Punctuator, "(", "Expected '('");
        consume(TokenType::Punctuator, ")", "Expected ')'");

        variable = std::make_unique<VariableDeclarationNode>(variableName,
                                                             previous().line);
    }

    // Consume the semicolon at the end of the input statement
    consume(TokenType::Punctuator, ";", "Expected ';' after input statement");

    // Create and return a new InputStatementNode
    std::unique_ptr<InputStatementNode> node =
        std::make_unique<InputStatementNode>(previous().line);
    node->variable = std::move(variable);
    return node;
}
std::unique_ptr<ClassNode> Parser::parseClassDeclaration()
{
    consume(TokenType::Declaration, "class", "Expected 'class' keyword");
    auto classNameToken =
        consume(TokenType::Identifier, "", "Expected class name");
    std::string className = classNameToken.value;

    std::string baseClassName;
    if (match(TokenType::Keyword, "extends"))
    {
        baseClassName =
            consume(TokenType::Identifier, "", "Expected base class name").value;
    }

    // Add the class name to declaredClasses
    declaredClasses.insert(className);

    auto classNode = std::make_unique<ClassNode>(className, classNameToken.line);
    classNode->baseClassName = baseClassName;

    // Consume the opening brace '{' of the class body
    consume(TokenType::Punctuator, "{", "Expected '{' after class name");

    // Parse class members until the closing brace '}'
    while (!check(TokenType::Punctuator, "}") && !isAtEnd())
    {
        classNode->members.push_back(parseClassMember());
    }

    // Consume the closing brace '}'
    consume(TokenType::Punctuator, "}", "Expected '}' at the end of class body");

    return classNode;
}

std::unique_ptr<ASTNode> Parser::parseClassMember()
{
    DEBUG_LOG(peek().value);

    // Check for constructor
    if (peek().value == "constructor")
    {
        DEBUG_LOG("Parsing constructor declaration");
        return parseConstructorDeclaration();
    }
    // Check if the member is a method or property (both start with a type)
    else if (peek().type == TokenType::Keyword && isType(peek().value))
    {
        // Look ahead to determine if it's a method or property
        // Methods: "type identifier(" or "type function identifier("
        // Properties: "type identifier;" or "type identifier ="

        size_t savedPos = current;
        advance(); // Skip the type

        if (peek().value == "function")
        {
            // Method with function keyword: "type function identifier("
            current = savedPos; // Reset position
            DEBUG_LOG("Parsing function declaration with function keyword");
            return parseFunctionDeclaration();
        }
        else if (peek().type == TokenType::Identifier && peekNext().value == "(")
        {
            // Method without function keyword: "type identifier("
            current = savedPos; // Reset position
            DEBUG_LOG("Parsing method declaration without function keyword");
            return parseFunctionDeclaration();
        }
        else
        {
            // Property: "type identifier;" or "type identifier ="
            current = savedPos; // Reset position
            DEBUG_LOG("Parsing property declaration");
            return parsePropertyDeclaration();
        }
    }

    DEBUG_LOG("Unsupported class member type: ", peek().value);
    throw std::runtime_error("Unsupported class member type");
}

std::unique_ptr<ASTNode> Parser::parsePropertyDeclaration()
{
    DEBUG_LOG("int"); // This seems to be logging the type
    std::unique_ptr<TypeNode> propertyType;
    if (peek().type == TokenType::Keyword && isType(peek().value))
    {
        // Assuming next token is type if it's an identifier
        propertyType = parseType();
    }

    // Assuming properties are declared like variables
    std::string propertyName =
        consume(TokenType::Identifier, "", "Expected property name").value;

    // Check for '=' and parse the initializer expression if present
    std::unique_ptr<ExpressionNode> initializer;
    if (match(TokenType::Operator, "="))
    {
        auto expr = parseExpression(); // Returns std::unique_ptr<ASTNode>
        initializer = std::unique_ptr<ExpressionNode>(expr.release());
        if (!initializer)
        {
            std::cout << "Expected expression for initializer" << std::endl;

            throw std::runtime_error("Expected expression for initializer");
        }
    }

    // Consume the semicolon at the end of the property declaration
    consume(TokenType::Punctuator, ";",
            "Expected ';' after property declaration");

    // Create and return a property declaration node
    return std::make_unique<PropertyDeclarationNode>(
        propertyName, std::move(propertyType), std::move(initializer),
        previous().line);
}

std::unique_ptr<ASTNode> Parser::parseConstructorDeclaration()
{
    // Consume the constructor keyword
    consume(TokenType::Identifier, "constructor", "Expected 'constructor' keyword");

    // Now consume the opening parenthesis
    consume(TokenType::Punctuator, "(", "Expected '(' after 'constructor'");

    // Parse parameters manually
    std::vector<std::unique_ptr<FunctionParameterNode>> parameters;

    if (!check(TokenType::Punctuator, ")"))
    {
        do
        {
            // Parse the type of the parameter
            auto type = parseType();

            // Parse the name of the parameter
            std::string paramName =
                consume(TokenType::Identifier, "", "Expected parameter name").value;

            // Create a FunctionParameterNode and add it to the parameters vector
            std::unique_ptr<FunctionParameterNode> parameter =
                std::make_unique<FunctionParameterNode>(paramName, previous().line);
            parameter->type = std::move(type);
            parameters.push_back(std::move(parameter));
        } while (match(TokenType::Punctuator, ","));
    }

    consume(TokenType::Punctuator, ")", "Expected ')' after constructor parameters");

    auto body = parseBlockStatement(); // Parse the constructor's body

    return std::make_unique<ConstructorNode>(std::move(parameters),
                                             std::move(body), previous().line);
}
std::unique_ptr<FunctionNode> Parser::parseFunctionDeclaration()
{
    bool isAsync = false;
    std::string returnType;

    // Check for 'async' keyword
    if (peek().type == TokenType::Keyword && peek().value == "async")
    {
        isAsync = true;
        advance(); // Consume 'async'
    }

    // Check for return type
    if (peek().type == TokenType::Keyword && isType(peek().value))
    {
        returnType = peek().value;
        advance(); // Consume return type
    }

    // Consume 'function' keyword
    consume(TokenType::Declaration, "function", "Expected 'function' keyword");

    // Get the function name
    std::string functionName =
        consume(TokenType::Identifier, "", "Expected function name").value;

    // Consume the opening parenthesis '(' of the parameter list
    consume(TokenType::Punctuator, "(", "Expected '(' after function name");

    // Parse function parameters
    std::vector<std::unique_ptr<FunctionParameterNode>> parameters;
    if (!check(TokenType::Punctuator, ")"))
    {
        do
        {
            parameters.push_back(parseFunctionParameter());
        } while (match(TokenType::Punctuator, ","));
    }

    // Consume the closing parenthesis ')' of the parameter list
    consume(TokenType::Punctuator, ")", "Expected ')' after parameters");
    // Parse the function body
    auto body = parseBlockStatement();

    // Create and return a new FunctionNode
    auto functionNode =
        std::make_unique<FunctionNode>(functionName, previous().line);
    functionNode->parameters = std::move(parameters);
    functionNode->body = std::move(body);
    functionNode->returnType = returnType;
    functionNode->isAsync = isAsync;

    return functionNode;
}

std::unique_ptr<VariableDeclarationNode>
Parser::parseVariableDeclaration(std::string type)
{
    bool isConst = match(TokenType::Keyword, "const");

    // First, correctly parse the type
    auto variableName =
        consume(TokenType::Identifier, "", "Expected identifier").value;
    std::string typeName = type;
    if (!isType(typeName))
    {
        std::cout << "Unknown type: " + typeName << std::endl;

        throw std::runtime_error("Unknown type: " + typeName);
    }

    // Optional initializer
    std::unique_ptr<ExpressionNode> initializer;
    if (match(TokenType::Operator, "="))
    {
        initializer = parseExpression();
    }

    // Consume the semicolon if present
    if (peek().type == TokenType::Punctuator && peek().value == ";")
    {
        advance(); // Consume the semicolon
    }
    else
    {
        std::cout << "Expected ';' after variable declaration, got: "
                  << "Type: " << static_cast<int>(peek().type)
                  << ", Value: '" << peek().value << "'" << std::endl;
    }

    std::unique_ptr<VariableDeclarationNode> node =
        std::make_unique<VariableDeclarationNode>(variableName, previous().line);
    node->initializer = std::move(initializer);
    node->isConst = isConst;
    node->typeName = typeName;
    return node;
}

std::unique_ptr<InterfaceNode> Parser::parseInterfaceDeclaration()
{
    // Ensure we are starting with 'interface' keyword
    consume(TokenType::Keyword, "interface", "Expected 'interface'");

    // Get the interface name
    std::string interfaceName =
        consume(TokenType::Identifier, "Expected interface name", "").value;

    // Add the class name to declaredClasses
    declaredInterfaces.insert(interfaceName);

    // Create a new InterfaceNode with the parsed name
    auto interfaceNode =
        std::make_unique<InterfaceNode>(interfaceName, previous().line);

    // Consume the opening brace '{' of the interface body
    consume(TokenType::Punctuator, "{", "Expected '{' after interface name");

    // Parse interface members until closing brace '}'
    while (!check(TokenType::Punctuator, "}") && !isAtEnd())
    {
        if (peek().type == TokenType::Identifier)
        {
            // Parse a property or method declaration
            interfaceNode->members.push_back(parseInterfaceMember());
        }
        else
        {
            error("Expected interface member declaration");
        }
    }

    // Consume the closing brace '}'
    consume(TokenType::Punctuator, "}", "Expected '}' at end of interface body");

    return interfaceNode;
}

std::unique_ptr<ASTNode> Parser::parseInterfaceMember()
{
    // Check if the member is a method
    if (match(TokenType::Keyword, "function"))
    {
        return parseFunctionDeclaration();
    }
    // Handle other types of interface members (e.g., properties)
    else
    {

        std::string propertyName =
            consume(TokenType::Identifier, "Expected property name", "").value;

        // Optional: Parse property type if it follows the property name
        std::unique_ptr<TypeNode> propertyType;
        if (peek().type == TokenType::Identifier)
        {
            // Assuming next token is type if it's an identifier
            propertyType = parseType();
        }

        // Consume the semicolon at the end of the property declaration
        consume(TokenType::Punctuator, ";",
                "Expected ';' after property declaration");

        // Create and return a property declaration node
        return std::make_unique<PropertyDeclarationNode>(
            propertyName, std::move(propertyType), nullptr,
            previous().line); // You may need to provide initializer if supported

        std::cout << "Unsupported interface member type" << std::endl;

        throw std::runtime_error("Unsupported interface member type");
    }
}

std::unique_ptr<BlockStatementNode> Parser::parseBlockStatement()
{
    // Consume the opening brace '{'
    consume(TokenType::Punctuator, "{", "Expected '{' at the start of block");

    // Create a block statement node
    auto block = std::make_unique<BlockStatementNode>(previous().line);

    // Parse statements until the closing brace '}'
    while (!check(TokenType::Punctuator, "}") && !isAtEnd())
    {
        try
        {
            block->statements.push_back(std::unique_ptr<StatementNode>(
                dynamic_cast<StatementNode *>(parseDeclaration().release())));
        }
        catch (const std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;

            error(e.what()); // Handle parsing errors in each statement
        }
    }

    // Consume the closing brace '}'
    consume(TokenType::Punctuator, "}", "Expected '}' at the end of block");

    return block;
}

std::unique_ptr<IfStatementNode> Parser::parseIfStatement()
{
    // Consume the opening parenthesis '('
    consume(TokenType::Punctuator, "(", "Expected '(' after 'if'");

    // Parse the condition expression
    auto condition = parseExpression();

    // Consume the closing parenthesis ')'
    consume(TokenType::Punctuator, ")", "Expected ')' after if condition");

    // Parse the 'then' branch (true branch)
    auto thenBranch = parseStatement();

    // Optional 'else' branch
    std::unique_ptr<StatementNode> elseBranch = nullptr;
    if (match(TokenType::Keyword, "else"))
    {
        auto elseStatement = parseStatement();
        elseBranch = std::unique_ptr<StatementNode>(
            dynamic_cast<StatementNode *>(elseStatement.release()));
        if (!elseBranch)
        {
            error("Expected a statement for the 'else' branch");
        }
    }

    // Create and return the IfStatementNode
    std::unique_ptr<IfStatementNode> node =
        std::make_unique<IfStatementNode>(previous().line);
    node->condition = std::move(condition);
    node->thenBranch = std::move(thenBranch);
    node->elseBranch = std::move(elseBranch);
    return node;
}

std::unique_ptr<ForStatementNode> Parser::parseForStatement()
{
    // The 'for' keyword has already been consumed in parseDeclaration()

    // Consume the opening parenthesis '('
    consume(TokenType::Punctuator, "(", "Expected '(' after 'for'");

    // Parse the initializer
    std::unique_ptr<StatementNode> initializer;
    if (isType(peek().value)) // Check for type names like "int", "string", etc.
    {
        std::string typeName = peek().value;
        advance(); // Consume the type
        initializer = parseVariableDeclaration(typeName);
    }
    else if (!match(TokenType::Punctuator, ";"))
    {
        initializer = parseExpressionStatement();
    }

    // Parse the condition
    std::unique_ptr<ExpressionNode> condition;
    if (!check(TokenType::Punctuator, ";"))
    {
        auto expr = parseExpression();
        condition = std::unique_ptr<ExpressionNode>(
            dynamic_cast<ExpressionNode *>(expr.release()));
        if (!condition)
        {
            error("Expected a valid expression for the condition");
        }
    }
    consume(TokenType::Punctuator, ";", "Expected ';' after for condition");

    // Parse the increment
    std::unique_ptr<ExpressionNode> increment;
    if (!check(TokenType::Punctuator, ")"))
    {
        auto expr = parseExpression();
        increment = std::unique_ptr<ExpressionNode>(
            dynamic_cast<ExpressionNode *>(expr.release()));
        if (!increment)
        {
            error("Expected a valid expression for the increment");
        }
    }
    consume(TokenType::Punctuator, ")", "Expected ')' after for clauses");

    // Parse the body
    auto body = parseStatement();

    // Create and return the ForStatementNode
    std::unique_ptr<ForStatementNode> node =
        std::make_unique<ForStatementNode>(previous().line);
    node->body = std::move(body);
    node->condition = std::move(condition);
    node->increment = std::move(increment);
    node->initializer = std::move(initializer);
    return node;
}

std::unique_ptr<StatementNode> Parser::parseExpressionStatement()
{
    // Parse the expression
    auto expr = parseExpression();

    // Consume the semicolon if present
    if (peek().type == TokenType::Punctuator && peek().value == ";")
    {
        advance(); // Consume the semicolon
    }
    else
    {
        std::cout << "Expected ';' after expression, got: "
                  << "Type: " << static_cast<int>(peek().type)
                  << ", Value: '" << peek().value << "'" << std::endl;
    }

    // Return the expression wrapped in a StatementNode
    return std::make_unique<ExpressionStatementNode>(std::move(expr),
                                                     previous().line);
}
std::unique_ptr<WhileStatementNode> Parser::parseWhileStatement()
{
    // The 'while' keyword has already been consumed in parseDeclaration

    // Consume the opening parenthesis '('
    consume(TokenType::Punctuator, "(", "Expected '(' after 'while'");

    // Parse the condition expression inside the while statement
    auto condition = parseExpression();

    // Consume the closing parenthesis ')'
    consume(TokenType::Punctuator, ")", "Expected ')' after while condition");

    // Parse the statement that forms the body of the while loop
    auto body = parseStatement();

    // Create and return the while statement node
    std::unique_ptr<WhileStatementNode> node =
        std::make_unique<WhileStatementNode>(previous().line);
    node->condition = std::move(condition);
    node->body = std::move(body);
    return node;
}

std::unique_ptr<ReturnStatementNode> Parser::parseReturnStatement()
{

    std::unique_ptr<ExpressionNode> returnValue;

    // Check if the next token is not a semicolon, indicating a return value is
    // present
    if (!check(TokenType::Punctuator, ";"))
    {
        // Parse the return value expression
        auto expr = parseExpression();

        // Cast the ASTNode to ExpressionNode
        returnValue = std::unique_ptr<ExpressionNode>(
            dynamic_cast<ExpressionNode *>(expr.release()));
        if (!returnValue)
        {
            error("Invalid expression in return statement");
        }
    }

    // Consume the semicolon at the end of the return statement
    consume(TokenType::Punctuator, ";", "Expected ';' after return statement");

    // Create and return the return statement node with the optional return value
    std::unique_ptr<ReturnStatementNode> node =
        std::make_unique<ReturnStatementNode>(previous().line);
    node->expression = std::move(returnValue);
    return node;
}

std::unique_ptr<BreakStatementNode> Parser::parseBreakStatement()
{
    // Consume the 'break' keyword
    consume(TokenType::Keyword, "break",
            "Expected 'break' keyword in break statement");

    // Consume the semicolon at the end of the break statement
    consume(TokenType::Punctuator, ";", "Expected ';' after break statement");

    // Create and return the break statement node
    return std::make_unique<BreakStatementNode>(previous().line);
}

std::unique_ptr<ContinueStatementNode> Parser::parseContinueStatement()
{
    // Consume the 'continue' keyword
    consume(TokenType::Keyword, "continue",
            "Expected 'continue' keyword in continue statement");

    // Consume the semicolon at the end of the continue statement
    consume(TokenType::Punctuator, ";", "Expected ';' after continue statement");

    // Create and return the continue statement node
    return std::make_unique<ContinueStatementNode>(previous().line);
}

std::unique_ptr<SwitchStatementNode> Parser::parseSwitchStatement()
{
    DEBUG_LOG("=== Starting parseSwitchStatement ===");
    DEBUG_LOG("Current token before consume: type=", static_cast<int>(peek().type), " value='", peek().value, "'");

    // Debug the check() method result
    DEBUG_LOG("check(TokenType::Keyword, 'switch') = ", check(TokenType::Keyword, "switch"));
    DEBUG_LOG("TokenType::Keyword = ", static_cast<int>(TokenType::Keyword));

    // Consume the 'switch' keyword
    try
    {
        consume(TokenType::Keyword, "switch", "Expected 'switch' keyword in switch statement");
        DEBUG_LOG("Successfully consumed 'switch' keyword");
    }
    catch (const std::exception &e)
    {
        DEBUG_LOG("ERROR: Failed to consume switch keyword: ", e.what());
        throw;
    }

    // Continue with the rest of the method...
    DEBUG_LOG("About to consume opening parenthesis");
    consume(TokenType::Punctuator, "(", "Expected '(' after 'switch'");
    DEBUG_LOG("Successfully consumed opening parenthesis");

    DEBUG_LOG("About to parse control expression");
    auto controlExpression = parseExpression();
    DEBUG_LOG("Successfully parsed control expression");

    consume(TokenType::Punctuator, ")", "Expected ')' after switch control expression");
    DEBUG_LOG("Successfully consumed closing parenthesis");

    consume(TokenType::Punctuator, "{", "Expected '{' at the start of switch body");
    DEBUG_LOG("Successfully consumed opening brace, about to parse cases");

    std::vector<std::unique_ptr<CaseClauseNode>> cases;
    int caseCount = 0;
    while (!check(TokenType::Punctuator, "}") && !isAtEnd())
    {
        DEBUG_LOG("Parsing case #", caseCount++);
        DEBUG_LOG("Current token: type=", static_cast<int>(peek().type), " value='", peek().value, "'");

        cases.push_back(parseCaseClause());
        DEBUG_LOG("Finished parsing case #", caseCount - 1);
    }

    DEBUG_LOG("Finished parsing all cases, total count: ", cases.size());

    consume(TokenType::Punctuator, "}", "Expected '}' at the end of switch body");
    DEBUG_LOG("Successfully consumed closing brace, switch parsing complete");

    std::unique_ptr<SwitchStatementNode> node =
        std::make_unique<SwitchStatementNode>(previous().line);
    node->condition = std::move(controlExpression);
    node->cases = std::move(cases);

    DEBUG_LOG("=== Finished parseSwitchStatement ===");
    return node;
}

std::unique_ptr<TryCatchNode> Parser::parseTryCatchStatement()
{
    // Consume the 'try' keyword
    consume(TokenType::Keyword, "try",
            "Expected 'try' keyword in try-catch statement");

    // Parse the try block
    auto tryBlock = parseBlockStatement();

    // Consume the 'catch' keyword
    consume(TokenType::Keyword, "catch",
            "Expected 'catch' keyword after try block");

    // Consume the opening parenthesis '('
    consume(TokenType::Punctuator, "(", "Expected '(' after 'catch'");

    // Parse the error type and variable name
    consume(TokenType::Identifier, "Error",
            "Expected 'Error' type in catch clause");
    std::string errorVarName =
        consume(TokenType::Identifier, "", "Expected error variable name").value;

    // Consume the closing parenthesis ')'
    consume(TokenType::Punctuator, ")", "Expected ')' after catch clause header");

    // Parse the catch block
    auto catchBlock = parseBlockStatement();

    // Create the error object structure (assuming a constructor or similar method
    // exists)
    auto errorObject = std::make_unique<ErrorTypeNode>(
        errorVarName, "message", "errorCode", previous().line);

    // Create and return the try-catch statement node
    std::unique_ptr<TryCatchNode> node =
        std::make_unique<TryCatchNode>(previous().line);
    node->catchBlock = std::move(catchBlock);
    node->tryBlock = std::move(tryBlock);
    return node;
}

std::unique_ptr<ExportNode> Parser::parseExportStatement()
{
    // Get the current line number
    int line = peek().line;

    // Consume the 'export' keyword
    consume(TokenType::Keyword, "export",
            "Expected 'export' keyword in export statement");

    // Check if it's a default export: "export default function/class/value"
    bool isDefault = match(TokenType::Keyword, "default");

    // Create the export node
    std::unique_ptr<ExportNode> node = std::make_unique<ExportNode>(line);
    node->isDefault = isDefault;

    // Check if it's a re-export: "export * from './module'" or "export { a, b } from './module'"
    if (match(TokenType::Punctuator, "*"))
    {
        // Handle "export * from './module'"
        if (match(TokenType::Keyword, "from"))
        {
            std::string moduleName = consume(TokenType::String, "", "Expected module name after 'from'").value;
            auto reExportNode = std::make_unique<ReExportNode>(line);
            reExportNode->moduleName = moduleName;
            reExportNode->exportAll = true;
            consume(TokenType::Punctuator, ";", "Expected ';' after re-export statement");
            return std::unique_ptr<ExportNode>(reExportNode.release());
        }
        else
        {
            error("Expected 'from' after export *");
        }
    }
    else if (match(TokenType::Punctuator, "{"))
    {
        // Handle "export { a, b } from './module'" or "export { a as b, c } from './module'"
        std::vector<std::pair<std::string, std::string>> namedExports;
        do
        {
            std::string originalName = consume(TokenType::Identifier, "", "Expected export name").value;
            std::string exportName = originalName;

            // Handle renamed exports: "export { a as b }"
            if (match(TokenType::Keyword, "as"))
            {
                exportName = consume(TokenType::Identifier, "", "Expected export alias after 'as'").value;
            }

            namedExports.push_back({originalName, exportName});
        } while (match(TokenType::Punctuator, ","));

        consume(TokenType::Punctuator, "}", "Expected '}' after export items");

        // Check if it's a re-export with 'from'
        if (match(TokenType::Keyword, "from"))
        {
            std::string moduleName = consume(TokenType::String, "", "Expected module name after 'from'").value;
            auto reExportNode = std::make_unique<ReExportNode>(line);
            reExportNode->moduleName = moduleName;
            reExportNode->namedExports = std::move(namedExports);
            reExportNode->exportAll = false;
            consume(TokenType::Punctuator, ";", "Expected ';' after re-export statement");
            return std::unique_ptr<ExportNode>(reExportNode.release());
        }
        else
        {
            // Regular named export of local variables
            consume(TokenType::Punctuator, ";", "Expected ';' after export statement");
            node->exportName = namedExports[0].first; // In this case, we just export existing declarations
            return node;
        }
    }

    // Handle normal exports: "export function/class/variable" or "export default function/class/variable"
    std::unique_ptr<ASTNode> exportItem;

    if (peek().type == TokenType::Keyword)
    {
        const std::string &nextTokenValue = peek().value;
        if (nextTokenValue == "class" || nextTokenValue == "function" ||
            nextTokenValue == "interface" || nextTokenValue == "template" ||
            nextTokenValue == "async")
        {
            // If the next token is a 'class', 'function', 'interface', 'template', or
            // 'async', parse it accordingly
            exportItem = parseDeclaration();

            // For named exports, store the name
            if (!isDefault)
            {
                if (auto *funcNode = dynamic_cast<FunctionNode *>(exportItem.get()))
                {
                    node->exportName = funcNode->name;
                }
                else if (auto *classNode = dynamic_cast<ClassNode *>(exportItem.get()))
                {
                    node->exportName = classNode->name;
                }
            }
        }
        else
        {
            exportItem = parseVariableDeclaration(peek().value);

            if (!isDefault && exportItem)
            {
                if (auto *varNode = dynamic_cast<VariableDeclarationNode *>(exportItem.get()))
                {
                    node->exportName = varNode->name;
                }
            }
        }
    }
    else if (isDefault)
    {
        // For "export default expression"
        exportItem = std::make_unique<ExpressionStatementNode>(
            parseExpression(), line);
    }
    else
    {
        error("Expected a declaration after 'export'");
    }

    // Ensure that exportItem is not null
    if (!exportItem)
    {
        error("Expected a valid item to export");
    }

    node->exportItem = std::move(exportItem);
    return node;
}

std::unique_ptr<ImportNode> Parser::parseImportStatement()
{
    // Create the import node
    std::unique_ptr<ImportNode> node = std::make_unique<ImportNode>(previous().line);

    // Note: 'import' keyword is already consumed in parseDeclaration()

    // Handle default imports
    if (peek().type == TokenType::Identifier)
    {
        // This is a default import: "import defaultName from './module'"
        node->hasDefaultImport = true;
        node->defaultImportName = consume(TokenType::Identifier, "", "Expected identifier for default import").value;

        // Check for named imports after default import: "import defaultName, { name1, name2 } from './module'"
        if (match(TokenType::Punctuator, ","))
        {
            if (!match(TokenType::Punctuator, "{"))
            {
                error("Expected '{' after default import and comma");
            }

            // Parse named imports
            do
            {
                std::string originalName = consume(TokenType::Identifier, "", "Expected import name").value;
                std::string localName = originalName;

                // Handle renamed imports: "import { originalName as localName }"
                if (match(TokenType::Keyword, "as"))
                {
                    localName = consume(TokenType::Identifier, "", "Expected local name after 'as'").value;
                }

                node->namedImports.push_back({originalName, localName});
            } while (match(TokenType::Punctuator, ","));

            consume(TokenType::Punctuator, "}", "Expected '}' after named imports");
        }
    }
    // Handle named imports without default import
    else if (match(TokenType::Punctuator, "{"))
    {
        // This is a named import: "import { name1, name2 } from './module'"
        do
        {
            std::string originalName = consume(TokenType::Identifier, "", "Expected import name").value;
            std::string localName = originalName;

            // Handle renamed imports: "import { originalName as localName }"
            if (match(TokenType::Keyword, "as"))
            {
                localName = consume(TokenType::Identifier, "", "Expected local name after 'as'").value;
            }

            node->namedImports.push_back({originalName, localName});
        } while (match(TokenType::Punctuator, ","));

        consume(TokenType::Punctuator, "}", "Expected '}' after named imports");
    }
    // Handle namespace imports: "import * as name from './module'"
    else if (match(TokenType::Punctuator, "*"))
    {
        if (match(TokenType::Keyword, "as"))
        {
            std::string namespaceName = consume(TokenType::Identifier, "", "Expected namespace name after 'as'").value;
            node->hasDefaultImport = true;
            node->defaultImportName = namespaceName;
        }
        else
        {
            error("Expected 'as' after '*' in import statement");
        }
    }
    else
    {
        // This handles side-effect imports: "import './module';"
        node->moduleName = consume(TokenType::String, "", "Expected module name").value;
        consume(TokenType::Punctuator, ";", "Expected ';' after import statement");
        return node;
    }

    // Consume 'from' keyword
    consume(TokenType::Keyword, "from", "Expected 'from' after import specifiers");

    // Parse the module name
    node->moduleName = consume(TokenType::String, "", "Expected module name after 'from'").value;

    // Consume the end of statement token (semicolon)
    consume(TokenType::Punctuator, ";", "Expected ';' after import statement");

    return node;
}

std::unique_ptr<CallExpressionNode>
Parser::parseCallExpression(std::unique_ptr<ExpressionNode> callee)
{
    std::vector<std::unique_ptr<ExpressionNode>> arguments;
    if (!check(TokenType::Punctuator, ")"))
    {
        do
        {
            arguments.push_back(parseExpression());
        } while (match(TokenType::Punctuator, ","));
    }
    consume(TokenType::Punctuator, ")", "Expected ')' after arguments");
    std::unique_ptr<CallExpressionNode> node =
        std::make_unique<CallExpressionNode>(previous().line);
    node->callee = std::move(callee);
    node->arguments = std::move(arguments);
    return node;
}
std::unique_ptr<MemberAccessExpressionNode>
Parser::parseMemberAccessExpression(std::unique_ptr<ExpressionNode> object)
{
    auto memberNameToken =
        consume(TokenType::Identifier, "", "Expected member name after '.'");
    std::string memberName = memberNameToken.value;

    auto node =
        std::make_unique<MemberAccessExpressionNode>(memberNameToken.line);
    node->object = std::move(object);
    node->memberName = std::move(memberName);

    return node;
}

std::unique_ptr<ExpressionNode> Parser::parseAnonymousFunction()
{
    // Assuming the 'function' keyword has already been consumed

    consume(TokenType::Punctuator, "(", "Expected '(' after 'function'");

    std::vector<std::unique_ptr<FunctionParameterNode>> parameters;
    if (!check(TokenType::Punctuator, ")"))
    {
        do
        {
            std::string paramName =
                consume(TokenType::Identifier, "", "Expected parameter name").value;
            // If your language supports types for parameters, parse the type here
            parameters.push_back(
                std::make_unique<FunctionParameterNode>(paramName, previous().line));
        } while (match(TokenType::Punctuator, ","));
    }

    consume(TokenType::Punctuator, ")", "Expected ')' after parameters");

    auto body = parseBlockStatement(); // Assuming parseBlockStatement() parses a
                                       // block of statements

    // Create a FunctionNode. Since it's anonymous, we pass an empty string for
    // the function name
    auto functionNode = std::make_unique<FunctionNode>("", previous().line);

    // Wrap the FunctionNode in an ExpressionNode if necessary
    return std::make_unique<FunctionExpressionNode>(std::move(functionNode),
                                                    previous().line);
}

std::unique_ptr<ExpressionNode> Parser::parseLiteral()
{
    if (previous().type == TokenType::Number)
    {
        // For numeric literals (integers, floats, doubles)
        std::string value = previous().value;
        if (value.find('.') != std::string::npos)
        {
            // Contains a decimal point, treat as a floating point or double
            return std::make_unique<FloatingPointLiteralNode>(value, previous().line);
        }
        else
        {
            // No decimal point, treat as an integer
            return std::make_unique<IntegerLiteralNode>(value, previous().line);
        }
    }
    else if (previous().type == TokenType::String)
    {
        // For string literals
        return std::make_unique<StringLiteralNode>(previous().value,
                                                   previous().line);
    }
    else if ((previous().type == TokenType::Keyword &&
              previous().value == "true") ||
             (previous().type == TokenType::Keyword &&
              previous().value == "false"))
    {
        // For boolean literals
        bool value = previous().value == "true";
        return std::make_unique<BooleanLiteralNode>(value, previous().line);
    }
    else if ((previous().type == TokenType::Keyword &&
              previous().value == "null"))
    {
        // For null literals
        return std::make_unique<NullLiteralNode>(previous().line);
    }
    else if (previous().type == TokenType::Character)
    {
        char value = previous().value[0];
        return std::make_unique<CharLiteralNode>(value, previous().line);
    }
    // Add more cases as needed for other types of literals
    std::cout << "Expected literal" << std::endl;

    throw std::runtime_error("Expected literal");
}

std::unique_ptr<ArrayLiteralNode> Parser::parseArrayLiteral()
{
    consume(TokenType::Punctuator, "[",
            "Expected '[' at the start of array literal");

    std::vector<std::unique_ptr<ExpressionNode>> elements;
    if (!check(TokenType::Punctuator, "]"))
    {
        do
        {
            auto element = parseExpression();
            elements.push_back(std::unique_ptr<ExpressionNode>(
                dynamic_cast<ExpressionNode *>(element.release())));
            if (!elements.back())
            {
                std::cout << "Expected expression in array literal" << std::endl;

                throw std::runtime_error("Expected expression in array literal");
            }
        } while (match(TokenType::Punctuator, ","));
    }

    consume(TokenType::Punctuator, "]",
            "Expected ']' at the end of array literal");

    std::unique_ptr<ArrayLiteralNode> node =
        std::make_unique<ArrayLiteralNode>(previous().line);
    node->elements = std::move(elements);
    return node;
}

std::unique_ptr<ObjectLiteralNode> Parser::parseObjectLiteral()
{
    consume(TokenType::Punctuator, "{",
            "Expected '{' at the start of object literal");

    std::vector<std::pair<std::string, std::unique_ptr<ExpressionNode>>>
        properties;
    if (!check(TokenType::Punctuator, "}"))
    {
        do
        {
            // Parse the key
            std::string key;
            if (match(TokenType::String, "") || match(TokenType::Identifier, ""))
            {
                key = previous().value;
            }
            else
            {
                std::cout << "Expected string or identifier as object key" << std::endl;

                throw std::runtime_error("Expected string or identifier as object key");
            }

            // Parse the colon separator
            consume(TokenType::Punctuator, ":", "Expected ':' after object key");

            // Parse the value (an expression)
            auto value = parseExpression();

            properties.emplace_back(key, std::move(value));
        } while (match(TokenType::Punctuator, ","));
    }

    consume(TokenType::Punctuator, "}",
            "Expected '}' at the end of object literal");

    std::unique_ptr<ObjectLiteralNode> node =
        std::make_unique<ObjectLiteralNode>(previous().line);
    node->properties = std::move(properties);
    return node;
}

std::unique_ptr<TypeNode> Parser::parseType()
{
    auto typeToken = consume(TokenType::Keyword, "", "Expected a type");

    std::string typeName = typeToken.value;
    if (typeName == "bool" || typeName == "char" || typeName == "int" ||
        typeName == "float" || typeName == "double" || typeName == "void" ||
        typeName == "wchar_t" || typeName == "string" || typeName == "Error" ||
        isClassName(typeName) || isInterfaceName(typeName))
    {
        return std::make_unique<TypeNode>(typeName, previous().line);
    }
    else
    {
        std::cout << "Unknown type: " + typeName << std::endl;

        throw std::runtime_error("Unknown type: " + typeName);
    }
}

bool Parser::isClassName(const std::string &name)
{
    return declaredClasses.find(name) != declaredClasses.end();
}

bool Parser::isInterfaceName(const std::string &name)
{
    return declaredInterfaces.find(name) != declaredInterfaces.end();
}

std::vector<std::unique_ptr<FunctionParameterNode>> Parser::parseParameters()
{
    std::vector<std::unique_ptr<FunctionParameterNode>> parameters;

    consume(TokenType::Punctuator, "(",
            "Expected '(' at the start of parameters");

    if (!check(TokenType::Punctuator, ")"))
    {
        do
        {
            // Parse the type of the parameter
            auto type = parseType();

            // Parse the name of the parameter
            std::string paramName =
                consume(TokenType::Identifier, "", "Expected parameter name").value;

            // Create a FunctionParameterNode and add it to the parameters vector
            std::unique_ptr<FunctionParameterNode> parameter =
                std::make_unique<FunctionParameterNode>(paramName, previous().line);
            parameter->type = std::move(type);
            parameters.push_back(std::move(parameter));
        } while (match(TokenType::Punctuator, ","));
    }

    consume(TokenType::Punctuator, ")", "Expected ')' after parameters");

    return parameters;
}

std::unique_ptr<AwaitExpressionNode> Parser::parseAwaitExpression()
{
    consume(TokenType::Keyword, "await", "Expected 'await'");

    auto expression = parseExpression(); // Parse the expression following 'await'
    std::unique_ptr<AwaitExpressionNode> node =
        std::make_unique<AwaitExpressionNode>(previous().line);
    node->expression = std::move(expression);
    return node;
}

std::unique_ptr<CaseClauseNode> Parser::parseCaseClause()
{
    DEBUG_LOG("=== Starting parseCaseClause ===");
    DEBUG_LOG("Current token: type=", static_cast<int>(peek().type), " value='", peek().value, "'");

    std::unique_ptr<ExpressionNode> caseExpression;
    bool isDefault = false;
    int line = peek().line;

    if (match(TokenType::Keyword, "case"))
    {
        DEBUG_LOG("Matched 'case' keyword");
        auto expr = parseExpression();
        caseExpression = std::unique_ptr<ExpressionNode>(
            dynamic_cast<ExpressionNode *>(expr.release()));
        if (!caseExpression)
        {
            std::cout << "Expected expression after 'case'" << std::endl;
            throw std::runtime_error("Expected expression after 'case'");
        }
        DEBUG_LOG("Parsed case expression");
    }
    else if (match(TokenType::Keyword, "default"))
    {
        DEBUG_LOG("Matched 'default' keyword");
        isDefault = true;
    }
    else
    {
        DEBUG_LOG("ERROR: Expected 'case' or 'default', got: type=", static_cast<int>(peek().type), " value='", peek().value, "'");
        std::cout << "Expected 'case' or 'default' keyword" << std::endl;
        throw std::runtime_error("Expected 'case' or 'default' keyword");
    }

    consume(TokenType::Punctuator, ":", "Expected ':' after case value");
    DEBUG_LOG("Consumed colon, about to parse case statements");

    std::vector<std::unique_ptr<StatementNode>> statements;
    int statementCount = 0;

    // Parse statements until we hit another case, default, closing brace, or EOF
    while (!check(TokenType::Keyword, "case") &&
           !check(TokenType::Keyword, "default") &&
           !check(TokenType::Punctuator, "}") &&
           !isAtEnd())
    {
        DEBUG_LOG("Parsing statement #", statementCount++, " in case");
        DEBUG_LOG("Current token before statement: type=", static_cast<int>(peek().type), " value='", peek().value, "'");

        try
        {
            auto astNode = parseStatement();
            DEBUG_LOG("parseStatement() returned successfully, checking type...");

            // Debug: Print the actual type of the returned node
            DEBUG_LOG("Returned node type: ", typeid(*astNode).name());

            StatementNode *statementNode = dynamic_cast<StatementNode *>(astNode.get());
            if (statementNode)
            {
                statements.push_back(std::unique_ptr<StatementNode>(statementNode));
                astNode.release(); // Release ownership from the original unique_ptr
                DEBUG_LOG("Successfully added statement to case");
            }
            else
            {
                DEBUG_LOG("ERROR: Dynamic cast to StatementNode failed");
                DEBUG_LOG("Actual node type: ", typeid(*astNode).name());
                std::cout << "Expected a statement node" << std::endl;
                throw std::runtime_error("Expected a statement node");
            }
        }
        catch (const std::exception &e)
        {
            DEBUG_LOG("ERROR in parseStatement(): ", e.what());
            throw;
        }

        DEBUG_LOG("Current token after statement: type=", static_cast<int>(peek().type), " value='", peek().value, "'");
    }

    DEBUG_LOG("Finished parsing case statements, count: ", statements.size());
    DEBUG_LOG("Exit condition: case=", check(TokenType::Keyword, "case"),
              " default=", check(TokenType::Keyword, "default"),
              " brace=", check(TokenType::Punctuator, "}"),
              " EOF=", isAtEnd());

    if (isDefault)
    {
        DEBUG_LOG("Creating default case node");
        return std::make_unique<CaseClauseNode>(std::move(statements), line);
    }
    else
    {
        DEBUG_LOG("Creating regular case node");
        return std::make_unique<CaseClauseNode>(std::move(caseExpression),
                                                std::move(statements), line);
    }
}

std::unique_ptr<TemplateNode> Parser::parseTemplateDeclaration()
{
    consume(TokenType::Keyword, "template", "Expected 'template' keyword");

    consume(TokenType::Punctuator, "<", "Expected '<' after 'template'");
    std::vector<std::string> templateParams;
    do
    {
        std::string paramName =
            consume(TokenType::Identifier, "", "Expected template parameter name")
                .value;
        templateParams.push_back(paramName);
    } while (match(TokenType::Punctuator, ","));
    consume(TokenType::Punctuator, ">", "Expected '>' after template parameters");

    auto declaration = parseDeclaration();
    return std::make_unique<TemplateNode>(
        std::move(templateParams), std::move(declaration), previous().line);
}