#include "../parser.h"
#include <gtest/gtest.h>

// Fixture for Parser tests
class ParserTest : public ::testing::Test
{
protected:
};

// Variable Declaration Tests
TEST_F(ParserTest, ParseIntVariableDeclaration)
{
  std::string source = "int myVar = 42;";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize(); // Use auto& to avoid copying

  Parser parser(tokens); // Pass the const reference to the constructor

  auto program = parser.parse();

  ASSERT_NE(program->children.size(), 0)
      << "Program should have children nodes";

  auto varDeclNode =
      dynamic_cast<VariableDeclarationNode *>(program->children[0].get());
  ASSERT_NE(varDeclNode, nullptr)
      << "First child should be a VariableDeclarationNode";

  ASSERT_EQ(varDeclNode->name, "myVar") << "Variable name should be 'myVar'";
  ASSERT_EQ(varDeclNode->typeName, "int") << "Variable type should be 'int'";

  auto initializer =
      dynamic_cast<IntegerLiteralNode *>(varDeclNode->initializer.get());
  ASSERT_NE(initializer, nullptr)
      << "Initializer should be an IntegerLiteralNode";
  ASSERT_EQ(initializer->value, 42) << "Initializer value should be 42";
}

TEST_F(ParserTest, ParseBoolVariableDeclaration)
{
  std::string source = "bool myBool = true;";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize(); // Use auto& to avoid copying

  Parser parser(tokens); // Pass the const reference to the constructor

  auto program = parser.parse();
  ASSERT_NE(program->children.size(), 0)
      << "Program should have children nodes";

  auto varDeclNode =
      dynamic_cast<VariableDeclarationNode *>(program->children[0].get());

  ASSERT_EQ(varDeclNode->typeName, "bool") << "Variable type should be 'bool'";
  auto initializer =
      dynamic_cast<BooleanLiteralNode *>(varDeclNode->initializer.get());
  ASSERT_NE(initializer, nullptr)
      << "Initializer should be a BooleanLiteralNode";
  ASSERT_EQ(initializer->value, true) << "Initializer value should be true";
}

TEST_F(ParserTest, ParseCharVariableDeclaration)
{
  std::string source = "char myChar = 'a';";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize(); // Use auto& to avoid copying

  Parser parser(tokens); // Pass the const reference to the constructor

  auto program = parser.parse();
  ASSERT_NE(program->children.size(), 0)
      << "Program should have children nodes";

  auto varDeclNode =
      dynamic_cast<VariableDeclarationNode *>(program->children[0].get());

  ASSERT_EQ(varDeclNode->typeName, "char") << "Variable type should be 'char'";
  auto initializer =
      dynamic_cast<CharLiteralNode *>(varDeclNode->initializer.get());
  ASSERT_NE(initializer, nullptr)
      << "Initializer should be a CharacterLiteralNode";
  ASSERT_EQ(initializer->value, 'a') << "Initializer value should be 'a'";
}

TEST_F(ParserTest, ParseFloatVariableDeclaration)
{
  std::string source = "float myFloat = 3.14;";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize(); // Use auto& to avoid copying

  Parser parser(tokens); // Pass the const reference to the constructor

  auto program = parser.parse();
  ASSERT_NE(program->children.size(), 0)
      << "Program should have children nodes";

  auto varDeclNode =
      dynamic_cast<VariableDeclarationNode *>(program->children[0].get());

  ASSERT_EQ(varDeclNode->typeName, "float")
      << "Variable type should be 'float'";
  auto initializer =
      dynamic_cast<FloatingPointLiteralNode *>(varDeclNode->initializer.get());
  ASSERT_NE(initializer, nullptr)
      << "Initializer should be a FloatingPointLiteralNode";
  ASSERT_FLOAT_EQ(initializer->value, 3.14f)
      << "Initializer value should be 3.14";
}

TEST_F(ParserTest, ParseStringVariableDeclaration)
{
  std::string source = "string myString = \"Hello\";";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize(); // Use auto& to avoid copying

  Parser parser(tokens); // Pass the const reference to the constructor

  auto program = parser.parse();
  ASSERT_NE(program->children.size(), 0)
      << "Program should have children nodes";

  auto varDeclNode =
      dynamic_cast<VariableDeclarationNode *>(program->children[0].get());

  ASSERT_EQ(varDeclNode->typeName, "string")
      << "Variable type should be 'string'";
  auto initializer =
      dynamic_cast<StringLiteralNode *>(varDeclNode->initializer.get());
  ASSERT_NE(initializer, nullptr)
      << "Initializer should be a StringLiteralNode";
  ASSERT_EQ(initializer->value, "Hello")
      << "Initializer value should be 'Hello'";
}

// Function Declaration Tests
TEST_F(ParserTest, ParseVoidFunctionDeclaration)
{
  std::string source = R"(
    void function myFunction(int a, float b) {
      print(a+b);
    }
  )";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize();

  Parser parser(tokens);
  auto program = parser.parse();

  ASSERT_NE(program->children.size(), 0)
      << "Program should have children nodes";

  auto functionNode = dynamic_cast<FunctionNode *>(program->children[0].get());
  ASSERT_NE(functionNode, nullptr) << "First child should be a FunctionNode";
  ASSERT_EQ(functionNode->name, "myFunction")
      << "Function name should be 'myFunction'";

  // Test for function parameters
  ASSERT_EQ(functionNode->parameters.size(), 2)
      << "Function should have two parameters";

  auto param1 =
      dynamic_cast<FunctionParameterNode *>(functionNode->parameters[0].get());
  ASSERT_NE(param1, nullptr)
      << "First parameter should be a FunctionParameterNode";
  ASSERT_EQ(param1->name, "a") << "First parameter name should be 'param1'";
  ASSERT_EQ(param1->type->typeName, "int")
      << "First parameter type should be 'int'";

  auto param2 =
      dynamic_cast<FunctionParameterNode *>(functionNode->parameters[1].get());
  ASSERT_NE(param2, nullptr)
      << "Second parameter should be a FunctionParameterNode";
  ASSERT_EQ(param2->name, "b") << "Second parameter name should be 'param2'";
  ASSERT_EQ(param2->type->typeName, "float")
      << "Second parameter type should be 'float'";

  // Optionally, test for function body
  auto bodyNode = dynamic_cast<BlockStatementNode *>(functionNode->body.get());
  ASSERT_NE(bodyNode, nullptr) << "Function should have a body";
}

TEST_F(ParserTest, ParseFunctionWithReturnType)
{
  std::string source = R"(
    int function sum(int a, int b) {
      return a + b;
    }
  )";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize();

  Parser parser(tokens);
  auto program = parser.parse();

  auto functionNode = dynamic_cast<FunctionNode *>(program->children[0].get());
  ASSERT_NE(functionNode, nullptr) << "First child should be a FunctionNode";
  ASSERT_EQ(functionNode->returnType, "int")
      << "Function return type should be 'int'";
  ASSERT_EQ(functionNode->name, "sum") << "Function name should be 'sum'";
  ASSERT_EQ(functionNode->parameters.size(), 2)
      << "Function should have two parameters";
  auto param1 =
      dynamic_cast<FunctionParameterNode *>(functionNode->parameters[0].get());
  ASSERT_NE(param1, nullptr) << "First parameter should not be null";
  ASSERT_EQ(param1->type->typeName, "int")
      << "First parameter type should be 'int'";
  ASSERT_EQ(param1->name, "a") << "First parameter name should be 'a'";

  auto param2 =
      dynamic_cast<FunctionParameterNode *>(functionNode->parameters[1].get());
  ASSERT_NE(param2, nullptr) << "Second parameter should not be null";
  ASSERT_EQ(param2->type->typeName, "int")
      << "Second parameter type should be 'int'";
  ASSERT_EQ(param2->name, "b") << "Second parameter name should be 'b'";
}

TEST_F(ParserTest, ParseFunctionComplexBody)
{
  std::string source = R"(
    void function complexFunction() {
      int a = 10;
      if (a > 5) {
        print(a);
      }
    }
  )";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize();

  Parser parser(tokens);
  auto program = parser.parse();

  auto functionNode = dynamic_cast<FunctionNode *>(program->children[0].get());
  ASSERT_NE(functionNode, nullptr) << "First child should be a FunctionNode";
  ASSERT_EQ(functionNode->name, "complexFunction")
      << "Function name should be 'complexFunction'";
  ASSERT_EQ(functionNode->returnType, "void")
      << "Function return type should be 'void'";
  auto bodyNode = dynamic_cast<BlockStatementNode *>(functionNode->body.get());
  ASSERT_NE(bodyNode, nullptr) << "Function should have a body";

  // Check for variable declaration inside the function body
  ASSERT_GT(bodyNode->statements.size(), 0)
      << "Function body should have statements";
  auto varDeclNode =
      dynamic_cast<VariableDeclarationNode *>(bodyNode->statements[0].get());
  ASSERT_NE(varDeclNode, nullptr)
      << "First statement should be a VariableDeclarationNode";
  ASSERT_EQ(varDeclNode->name, "a") << "Variable name should be 'a'";
  ASSERT_EQ(varDeclNode->typeName, "int") << "Variable type should be 'int'";

  // Check for if statement inside the function body
  auto ifStatementNode =
      dynamic_cast<IfStatementNode *>(bodyNode->statements[1].get());
  ASSERT_NE(ifStatementNode, nullptr)
      << "Second statement should be an IfStatementNode";
}

// Class declaration tests

TEST_F(ParserTest, ParseBasicClassDeclaration)
{
  std::string source = R"(
    class MyClass {
      // Class body
    }
  )";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize();

  Parser parser(tokens);
  auto program = parser.parse();

  ASSERT_NE(program->children.size(), 0)
      << "Program should have children nodes";

  auto classNode = dynamic_cast<ClassNode *>(program->children[0].get());
  ASSERT_NE(classNode, nullptr) << "First child should be a ClassNode";
  ASSERT_EQ(classNode->name, "MyClass") << "Class name should be 'MyClass'";
}

TEST_F(ParserTest, ParseClassWithProperties)
{
  std::string source = R"(
    class MyClass {
      int myProperty;
    }
  )";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize();

  Parser parser(tokens);
  auto program = parser.parse();

  auto classNode = dynamic_cast<ClassNode *>(program->children[0].get());
  ASSERT_NE(classNode, nullptr) << "First child should be a ClassNode";

  // Assuming the class has a list of property nodes
  ASSERT_GT(classNode->members.size(), 0) << "Class should have properties";
  auto propertyNode = classNode->members[0].get();
  ASSERT_NE(propertyNode, nullptr) << "Property should be a PropertyNode";
}

TEST_F(ParserTest, ParseClassWithMethods)
{
  std::string source = R"(
    class MyClass {
      void function myMethod() {
        print("Hello");
      }
    }
  )";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize();

  Parser parser(tokens);
  auto program = parser.parse();

  auto classNode = dynamic_cast<ClassNode *>(program->children[0].get());
  ASSERT_NE(classNode, nullptr) << "First child should be a ClassNode";

  // Assuming the class has a list of method nodes
  ASSERT_GT(classNode->members.size(), 0) << "Class should have methods";
  auto methodNode = dynamic_cast<FunctionNode *>(classNode->members[0].get());
  ASSERT_NE(methodNode, nullptr) << "Method should be a MethodNode";
  ASSERT_EQ(methodNode->name, "myMethod") << "Method name should be 'myMethod'";
}
