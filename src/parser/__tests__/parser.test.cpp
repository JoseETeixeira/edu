#include "../parser.h"
#include <gtest/gtest.h>

// Fixture for Parser tests
class ParserTest : public ::testing::Test {
protected:
};

// Variable Declaration Tests
TEST_F(ParserTest, ParseIntVariableDeclaration) {
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

TEST_F(ParserTest, ParseBoolVariableDeclaration) {
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

TEST_F(ParserTest, ParseCharVariableDeclaration) {
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
      dynamic_cast<CharacterLiteralNode *>(varDeclNode->initializer.get());
  ASSERT_NE(initializer, nullptr)
      << "Initializer should be a CharacterLiteralNode";
  ASSERT_EQ(initializer->value, 'a') << "Initializer value should be 'a'";
}

TEST_F(ParserTest, ParseFloatVariableDeclaration) {
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

TEST_F(ParserTest, ParseDoubleVariableDeclaration) {
  std::string source = "double myDouble = 3.14;";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize(); // Use auto& to avoid copying

  Parser parser(tokens); // Pass the const reference to the constructor

  auto program = parser.parse();
  ASSERT_NE(program->children.size(), 0)
      << "Program should have children nodes";

  auto varDeclNode =
      dynamic_cast<VariableDeclarationNode *>(program->children[0].get());

  ASSERT_EQ(varDeclNode->typeName, "double")
      << "Variable type should be 'double'";
  auto initializer =
      dynamic_cast<FloatingPointLiteralNode *>(varDeclNode->initializer.get());
  ASSERT_NE(initializer, nullptr)
      << "Initializer should be a FloatingPointLiteralNode";
  ASSERT_DOUBLE_EQ(initializer->value, 3.14)
      << "Initializer value should be 3.14";
}

TEST_F(ParserTest, ParseStringVariableDeclaration) {
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
