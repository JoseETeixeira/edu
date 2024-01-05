#include "../tokenizer.h" // Include your tokenizer header file
#include <gtest/gtest.h>

class TokenizerTest : public ::testing::Test {
protected:
  Tokenizer *tokenizer;

  void SetUp() override {
    // This can be left empty if no general setup is needed.
  }

  void TearDown() override { delete tokenizer; }

  Token tokenizeSingleToken(const std::string &source) {
    tokenizer = new Tokenizer(source);
    return tokenizer->nextToken();
  }
};

TEST_F(TokenizerTest, IdentifiesKeywords) {
  Token token = tokenizeSingleToken("class");
  EXPECT_EQ(token.type, TokenType::Keyword);
  EXPECT_EQ(token.value, "class");
}

TEST_F(TokenizerTest, IdentifiesIdentifiers) {
  Token token = tokenizeSingleToken("myVariable");
  EXPECT_EQ(token.type, TokenType::Identifier);
  EXPECT_EQ(token.value, "myVariable");
}

TEST_F(TokenizerTest, IdentifiesNumbers) {
  Token token = tokenizeSingleToken("1234");
  EXPECT_EQ(token.type, TokenType::Number);
  EXPECT_EQ(token.value, "1234");
}

TEST_F(TokenizerTest, IdentifiesStrings) {
  Token token = tokenizeSingleToken("\"Hello, World!\"");
  EXPECT_EQ(token.type, TokenType::String);
  EXPECT_EQ(token.value, "Hello, World!");
}

TEST_F(TokenizerTest, IdentifiesOperators) {
  Token token = tokenizeSingleToken("+");
  EXPECT_EQ(token.type, TokenType::Operator);
  EXPECT_EQ(token.value, "+");
}

TEST_F(TokenizerTest, IdentifiesPunctuators) {
  Token token = tokenizeSingleToken(";");
  EXPECT_EQ(token.type, TokenType::Punctuator);
  EXPECT_EQ(token.value, ";");
}

TEST_F(TokenizerTest, IdentifiesEndOfFile) {
  Token token = tokenizeSingleToken("");
  EXPECT_EQ(token.type, TokenType::EndOfFile);
  EXPECT_EQ(token.value, "");
}

TEST_F(TokenizerTest, HandlesUnknownCharacters) {
  Token token = tokenizeSingleToken("@");
  EXPECT_EQ(token.type, TokenType::Unknown);
  EXPECT_EQ(token.value, "@");
}

TEST_F(TokenizerTest, HandlesStringsWithEscapeSequences) {
  Token token = tokenizeSingleToken("\"Line\\nBreak\"");
  EXPECT_EQ(token.type, TokenType::String);
  EXPECT_EQ(token.value, "Line\nBreak");
}

TEST_F(TokenizerTest, HandlesUnterminatedStrings) {
  Token token = tokenizeSingleToken("\"Unterminated");
  EXPECT_EQ(token.type, TokenType::Unknown);
}
