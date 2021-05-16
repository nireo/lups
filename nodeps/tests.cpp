#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "parser.h"
#include <gtest/gtest.h>
#include <memory>
#include <utility>

TEST(LexerTest, TestNextToken) {
	std::string input = "=+(){},;";

	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::ASSIGN, "="));
	expected.push_back(std::make_pair(tokentypes::PLUS, "+"));
	expected.push_back(std::make_pair(tokentypes::LPAREN, "("));
	expected.push_back(std::make_pair(tokentypes::RPAREN, ")"));
	expected.push_back(std::make_pair(tokentypes::LBRACE, "{"));
	expected.push_back(std::make_pair(tokentypes::RBRACE, "}"));
	expected.push_back(std::make_pair(tokentypes::COMMA, ","));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));

	auto lexer = Lexer(input);
	for (int i = 0; i < (int)expected.size(); ++i) {
		auto token = lexer.next_token();

		EXPECT_EQ(expected[i].first, token.type) << "token types don't match";
		EXPECT_EQ(expected[i].second, token.literal)
				<< "token literals don't match";
	}
}

TEST(LexerTest, IfStatements) {
	std::string input = "if (5 < 10) {"
											"    return true;"
											"} else {"
											"    return false;"
											"}";


	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::IF, "if"));
	expected.push_back(std::make_pair(tokentypes::LPAREN, "("));
	expected.push_back(std::make_pair(tokentypes::INT, "5"));
	expected.push_back(std::make_pair(tokentypes::LT, "<"));
	expected.push_back(std::make_pair(tokentypes::INT, "10"));
	expected.push_back(std::make_pair(tokentypes::RPAREN, ")"));
	expected.push_back(std::make_pair(tokentypes::LBRACE, "{"));
	expected.push_back(std::make_pair(tokentypes::RETURN, "return"));
	expected.push_back(std::make_pair(tokentypes::TRUE, "true"));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));
	expected.push_back(std::make_pair(tokentypes::RBRACE, "}"));
	expected.push_back(std::make_pair(tokentypes::ELSE, "else"));
	expected.push_back(std::make_pair(tokentypes::LBRACE, "{"));
	expected.push_back(std::make_pair(tokentypes::RETURN, "return"));
	expected.push_back(std::make_pair(tokentypes::FALSE, "false"));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));
	expected.push_back(std::make_pair(tokentypes::RBRACE, "}"));

	auto lexer = Lexer(input);
	for (int i = 0; i < (int)expected.size(); ++i) {
		auto token = lexer.next_token();

		EXPECT_EQ(expected[i].first, token.type) << "token types don't match";
		EXPECT_EQ(expected[i].second, token.literal)
				<< "token literals don't match";
	}
}

TEST(LexerTest, Variables) {
	std::string input =
		"let a = 102;"
		"a = 5;";

	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::LET, "let"));
	expected.push_back(std::make_pair(tokentypes::IDENT, "a"));
	expected.push_back(std::make_pair(tokentypes::ASSIGN, "="));
	expected.push_back(std::make_pair(tokentypes::INT, "102"));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));
	expected.push_back(std::make_pair(tokentypes::IDENT, "a"));
	expected.push_back(std::make_pair(tokentypes::ASSIGN, "="));
	expected.push_back(std::make_pair(tokentypes::INT, "5"));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));

	auto lexer = Lexer(input);
	for (int i = 0; i < (int)expected.size(); ++i) {
		auto token = lexer.next_token();

		EXPECT_EQ(expected[i].first, token.type) << "token types don't match";
		EXPECT_EQ(expected[i].second, token.literal)
				<< "token literals don't match";
	}
}

TEST(LexerTest, FunctionDeclaration) {
	std::string input =
		"let add = func(x, y) {"
		"    return x + y;"
		"}";

	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::LET, "let"));
	expected.push_back(std::make_pair(tokentypes::IDENT, "add"));
	expected.push_back(std::make_pair(tokentypes::ASSIGN, "="));
	expected.push_back(std::make_pair(tokentypes::FUNCTION, "func"));
	expected.push_back(std::make_pair(tokentypes::LPAREN, "("));
	expected.push_back(std::make_pair(tokentypes::IDENT, "x"));
	expected.push_back(std::make_pair(tokentypes::COMMA, ","));
	expected.push_back(std::make_pair(tokentypes::IDENT, "y"));
	expected.push_back(std::make_pair(tokentypes::RPAREN, ")"));
	expected.push_back(std::make_pair(tokentypes::LBRACE, "{"));
	expected.push_back(std::make_pair(tokentypes::RETURN, "return"));
	expected.push_back(std::make_pair(tokentypes::IDENT, "x"));
	expected.push_back(std::make_pair(tokentypes::PLUS, "+"));
	expected.push_back(std::make_pair(tokentypes::IDENT, "y"));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));
	expected.push_back(std::make_pair(tokentypes::RBRACE, "}"));

	auto lexer = Lexer(input);
	for (int i = 0; i < (int)expected.size(); ++i) {
		auto token = lexer.next_token();

		EXPECT_EQ(expected[i].first, token.type) << "token types don't match";
		EXPECT_EQ(expected[i].second, token.literal)
				<< "token literals don't match";
	}
}

TEST(LexerTest, EqualNonEqual) {
	std::string input =
		"10 == 10;"
		"10 != 5;";

	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::INT, "10"));
	expected.push_back(std::make_pair(tokentypes::EQ, "=="));
	expected.push_back(std::make_pair(tokentypes::INT, "10"));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));

	expected.push_back(std::make_pair(tokentypes::INT, "10"));
	expected.push_back(std::make_pair(tokentypes::NEQ, "!="));
	expected.push_back(std::make_pair(tokentypes::INT, "5"));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));

	auto lexer = Lexer(input);
	for (int i = 0; i < (int)expected.size(); ++i) {
		auto token = lexer.next_token();

		EXPECT_EQ(expected[i].first, token.type) << "token types don't match";
		EXPECT_EQ(expected[i].second, token.literal)
				<< "token literals don't match";
	}
}

TEST(ParserTest, LetStatements) {
	std::string input =
		"let x = 5;"
		"let y = 10;"
		"let foobar = 123456;";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	// check the the program ins't a nullptr
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
	EXPECT_EQ(program->statements.size(), 3) << "Wrong amount of statements";

	// there should be no errors
	auto errors = parser.errors();
	EXPECT_EQ(errors.size(), 0) << "There are errors in the parsing";

	std::string names[3] = { "x", "y", "foobar"};
	for (int i = 0; i < (int)program->statements.size(); ++i) {
		EXPECT_EQ(program->statements[i]->Type(), "LetStatement");
		EXPECT_EQ(program->statements[i]->TokenLiteral(), "let");

		auto let_class = dynamic_cast<LetStatement*>(program->statements[i].get());
		EXPECT_NE(let_class, nullptr);
		EXPECT_EQ(let_class->name->value, names[i]);
		EXPECT_EQ(let_class->name->TokenLiteral(), names[i]);
	}
}

TEST(ParserTest, ReturnStatements) {
	std::string input =
		"return 5;"
		"return = 10;"
		"return = 123456;";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	// check the the program ins't a nullptr
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
	EXPECT_EQ(program->statements.size(), 3) << "Wrong amount of statements";

	// there should be no errors
	auto errors = parser.errors();
	EXPECT_EQ(errors.size(), 0) << "There are errors in the parsing";

	for (int i = 0; i < program->statements.size(); ++i) {
		EXPECT_EQ(program->statements[i]->Type(), "ReturnStatement");
		EXPECT_EQ(program->statements[i]->TokenLiteral(), "return");

		auto return_class = dynamic_cast<ReturnStatement*>(program->statements[i].get());
		EXPECT_NE(return_class, nullptr);
	}
}
