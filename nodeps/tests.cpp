#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <gtest/gtest.h>
#include <memory>
#include <tuple>
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
	std::string input = "let a = 102;"
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
	std::string input = "let add = func(x, y) {"
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
	std::string input = "10 == 10;"
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
	std::string input = "let x = 5;"
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

	std::string names[3] = {"x", "y", "foobar"};
	for (int i = 0; i < (int)program->statements.size(); ++i) {
		EXPECT_EQ(program->statements[i]->Type(), "LetStatement");
		EXPECT_EQ(program->statements[i]->TokenLiteral(), "let");

		auto let_class = dynamic_cast<LetStatement *>(program->statements[i].get());
		EXPECT_NE(let_class, nullptr);
		EXPECT_EQ(let_class->name->value, names[i]);
		EXPECT_EQ(let_class->name->TokenLiteral(), names[i]);
	}
}

TEST(ParserTest, ReturnStatements) {
	std::string input = "return 5;"
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

		auto return_class =
				dynamic_cast<ReturnStatement *>(program->statements[i].get());
		EXPECT_NE(return_class, nullptr);
	}
}

TEST(ParserTest, IdentifierExpression) {
	std::string input = "foobar;";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	// check the the program ins't a nullptr
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
	EXPECT_EQ(program->statements.size(), 1) << "Wrong amount of statements";

	auto stmt = dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(stmt, nullptr);

	auto ident = dynamic_cast<Identifier *>(stmt->expression.get());
	EXPECT_NE(ident, nullptr);

	EXPECT_EQ(ident->value, "foobar");
	EXPECT_EQ(ident->TokenLiteral(), "foobar");
}

TEST(ParserTest, IntegerLiteralExpression) {
	std::string input = "5;";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	// check the the program ins't a nullptr
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
	EXPECT_EQ(program->statements.size(), 1) << "Wrong amount of statements";

	auto stmt = dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(stmt, nullptr) << "Statement is not a expression statement";

	auto lit = dynamic_cast<IntegerLiteral *>(stmt->expression.get());
	EXPECT_NE(lit, nullptr) << "Expression is not a integer literal";

	EXPECT_EQ(lit->value, 5) << "Value is not five";
	EXPECT_EQ(lit->TokenLiteral(), "5") << "Literal value is wrong";
}

TEST(ParserTest, PrefixExpressionTest) {
	std::vector < std::tuple<std::string, std::string, int> test_cases;
	test_cases.push_back(std::make_tuple("!5;", "!", 5));
	test_cases.push_back(std::make_tuple("-15;", "-", 15));

	for (auto &tc : test_cases) {
		auto lexer = Lexer(std::get<0>(tc));
		auto parser = Parser(std::make_unique<Lexer>(lexer));

		auto program = parser.parse_program();
		// check the the program ins't a nullptr
		EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
		EXPECT_EQ(program->statements.size(), 1) << "Wrong amount of statements";

		auto stmt =
				dynamic_cast<ExpressionStatement *>(program->statements[0].get());
		EXPECT_NE(stmt, nullptr) << "Statement is not a expression statement";

		auto pre = dynamic_cast<PrefixExpression *>(stmt->expression.get());
		EXPECT_NE(lit, nullptr) << "Expression is not a prefix expression";

		EXPECT_EQ(pre->opr, std::get<1>(tc));

		auto lit = dynamic_cast<IntegerLiteral *>(pre->right.get());
		EXPECT_NE(lit, nullptr) << "Expression is not a integer literal";

		EXPECT_EQ(lit->value, std::get<2>(tc)) << "Value is not five";
	}
}
