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

  std::vector<Token> tokenizeSource(const std::string &source) {
    tokenizer = new Tokenizer(source);
    std::vector<Token> tokens;
    Token token;
    do {
      token = tokenizer->nextToken();
      tokens.push_back(token);
    } while (token.type != TokenType::EndOfFile);
    return tokens;
  }
};

TEST_F(TokenizerTest, IdentifiesKeywords) {
  Token token = tokenizeSingleToken("for");
  EXPECT_EQ(token.type, TokenType::Keyword);
  EXPECT_EQ(token.value, "for");
}

TEST_F(TokenizerTest, IdentifiesDeclarations) {
  Token token = tokenizeSingleToken("class");
  EXPECT_EQ(token.type, TokenType::Declaration);
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

TEST_F(TokenizerTest, TokenizesComplexSourceCode) {
  std::string source = R"(
    import { asyncFunction } from "module-name";

    class MyClass {
      constructor(int a, float b) {
        print("Constructor");
      }

      async int asyncMethod() {
        int result = await asyncFunction();
        print(result);
      }
    }

    try {
      MyClass obj = MyClass(10, 3.14);
    } catch (Error error) {
      console.error(error);
    }
  )";

  auto tokens = tokenizeSource(source);

  // Example assertions (not exhaustive):
  EXPECT_EQ(tokens[0].type, TokenType::Keyword);
  EXPECT_EQ(tokens[1].type, TokenType::Punctuator);
  EXPECT_EQ(tokens[2].type, TokenType::Identifier); // asyncFunction

  // Check for specific keywords
  auto classToken =
      std::find_if(tokens.begin(), tokens.end(),
                   [](const Token &t) { return t.value == "class"; });
  EXPECT_NE(classToken, tokens.end());
  EXPECT_EQ(classToken->type, TokenType::Declaration);
}

TEST_F(TokenizerTest, TokenizersVariableDeclaration) {
  std::string source = "int myVar = 42;";

  auto tokens = tokenizeSource(source);

  // Example assertions (not exhaustive):
  EXPECT_EQ(tokens[0].type, TokenType::Keyword);
  EXPECT_EQ(tokens[1].type, TokenType::Identifier);
  EXPECT_EQ(tokens[2].type, TokenType::Operator);
  EXPECT_EQ(tokens[3].type, TokenType::Number);
}

TEST_F(TokenizerTest, TokenizesCharacterVariable) {
  std::string source = "char myVar = 'a';";

  auto tokens = tokenizeSource(source);

  // Example assertions (not exhaustive):
  EXPECT_EQ(tokens[0].type, TokenType::Keyword);
  EXPECT_EQ(tokens[1].type, TokenType::Identifier);
  EXPECT_EQ(tokens[2].type, TokenType::Operator);
  EXPECT_EQ(tokens[3].type, TokenType::Character);
}