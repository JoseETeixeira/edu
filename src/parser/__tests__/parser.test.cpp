#include "../parser.h"
#include <gtest/gtest.h>

// Fixture for Parser tests
class ParserTest : public ::testing::Test {
protected:
};

TEST_F(ParserTest, ParseVariableDeclaration) {
  std::string source = "int myVar = 42;";
  Tokenizer tokenizer(source);
  const auto &tokens = tokenizer.tokenize(); // Use auto& to avoid copying

  Parser parser(tokens); // Pass the const reference to the constructor

  auto program = parser.parse();

  // Assertions to check the structure of the AST
  // Example: ASSERT_NE(program->children.size(), 0);
}
