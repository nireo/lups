#include "ast.h"
#include "code.h"
#include "compiler.h"
#include "eval.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"
#include "token.h"
#include "vm.h"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

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

TEST(LexerTest, Strings) {
	std::string input = "\"foobar\""
											"\"foo bar\"";

	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::STRING, "foobar"));
	expected.push_back(std::make_pair(tokentypes::STRING, "foo bar"));

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

TEST(LexerTest, ArrayTest) {
	std::string input = "[1, 2];";
	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::LBRACKET, "["));
	expected.push_back(std::make_pair(tokentypes::INT, "1"));
	expected.push_back(std::make_pair(tokentypes::COMMA, ","));
	expected.push_back(std::make_pair(tokentypes::INT, "2"));
	expected.push_back(std::make_pair(tokentypes::RBRACKET, "]"));
	expected.push_back(std::make_pair(tokentypes::SEMICOLON, ";"));

	auto lexer = Lexer(input);
	for (int i = 0; i < (int)expected.size(); ++i) {
		auto token = lexer.next_token();

		EXPECT_EQ(expected[i].first, token.type) << "token types don't match";
		EXPECT_EQ(expected[i].second, token.literal)
				<< "token literals don't match";
	}
}

TEST(LexerTest, HashTest) {
	std::string input = "{\"foo\": \"bar\"}";
	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::LBRACE, "{"));
	expected.push_back(std::make_pair(tokentypes::STRING, "foo"));
	expected.push_back(std::make_pair(tokentypes::COLON, ":"));
	expected.push_back(std::make_pair(tokentypes::STRING, "bar"));
	expected.push_back(std::make_pair(tokentypes::RBRACE, "}"));

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
	std::vector<std::tuple<std::string, std::string, int>> test_cases;
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
		EXPECT_NE(pre, nullptr) << "Expression is not a prefix expression";

		EXPECT_EQ(pre->opr, std::get<1>(tc));

		auto lit = dynamic_cast<IntegerLiteral *>(pre->right.get());
		EXPECT_NE(lit, nullptr) << "Expression is not a integer literal";

		EXPECT_EQ(lit->value, std::get<2>(tc)) << "Value is not five";
	}
}

TEST(ParsetTest, BooleanPrefixExpressionTest) {
	struct Testcase {
		std::string input;
		std::string opr;
		bool value;
	};

	std::vector<Testcase> test_cases = {
			{"!true;", "!", true},
			{"!false;", "!", false},
	};

	for (auto &tc : test_cases) {
		auto lexer = Lexer(tc.input);
		auto parser = Parser(std::make_unique<Lexer>(lexer));
		auto program = parser.parse_program();

		// check the the program ins't a nullptr
		EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
		EXPECT_EQ(program->statements.size(), 1) << "Wrong amount of statements";

		auto stmt =
				dynamic_cast<ExpressionStatement *>(program->statements[0].get());
		EXPECT_NE(stmt, nullptr) << "Statement is not a expression statement";

		auto pre = dynamic_cast<PrefixExpression *>(stmt->expression.get());
		EXPECT_NE(pre, nullptr) << "Expression is not a prefix expression";

		auto boolexp = dynamic_cast<BooleanExpression *>(pre->right.get());
		EXPECT_NE(boolexp, nullptr) << "Expression is not a integer literal";
		EXPECT_EQ(boolexp->value, tc.value);
	}
}

bool test_integer_literal(int expected, std::unique_ptr<Expression> exp) {
	auto lit = dynamic_cast<IntegerLiteral *>(exp.get());
	if (lit == nullptr)
		return false;

	if (lit->value != expected)
		return false;

	return true;
}

bool test_bool_literal(bool expected, std::unique_ptr<Expression> exp) {
	auto lit = dynamic_cast<BooleanExpression *>(exp.get());
	if (lit == nullptr)
		return false;

	if (lit->value != expected)
		return false;

	return true;
}

TEST(ParserTest, HashLiteralStrings) {
	std::string input = "{\"one\": 1, \"two\": 2, \"three\": 3}";
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr)
			<< "The first statement is not an expression statement";

	auto hash = dynamic_cast<HashLiteral *>(expstmt->expression.get());
	EXPECT_NE(hash, nullptr) << "The expression statement is not an hash literal";

	std::map<std::string, int> expected{
			{"one", 1},
			{"two", 2},
			{"three", 3},
	};

	for (const auto &pr : hash->pairs) {
		auto keystring = dynamic_cast<StringLiteral *>(pr.first.get());
		EXPECT_NE(keystring, nullptr);

		auto expected_value = expected[keystring->TokenLiteral()];
		auto integer = dynamic_cast<IntegerLiteral *>(pr.second.get());
		EXPECT_EQ(expected_value, integer->value);
	}
}

TEST(ParserTest, EmptyHashLiteral) {
	std::string input = "{}";
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr)
			<< "The first statement is not an expression statement";

	auto hash = dynamic_cast<HashLiteral *>(expstmt->expression.get());
	EXPECT_NE(hash, nullptr) << "The expression statement is not an hash literal";
	EXPECT_EQ(hash->pairs.size(), 0);
}

TEST(ParserTest, HashLiteralWithExpressions) {
	std::string input = "{\"one\": 0 + 1, \"two\": 10 - 8, \"three\": 15 / 5}";
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr)
			<< "The first statement is not an expression statement";

	auto hash = dynamic_cast<HashLiteral *>(expstmt->expression.get());
	EXPECT_NE(hash, nullptr) << "The expression statement is not an hash literal";

	struct ExpectedInfixes {
		std::string key;
		std::string opr;
		int left;
		int right;
	};

	std::vector<ExpectedInfixes> expected{
			{"one", "+", 0, 1},
			{"two", "-", 10, 8},
			{"three", "/", 15, 5},
	};
}

TEST(ParserTest, InfixExpressionTest) {
	struct Testcase {
		std::string input;
		int left_value;
		std::string opr;
		int right_value;
	};

	std::vector<Testcase> test_cases = {
			{"5 + 5;", 5, "+", 5},   {"5 - 5;", 5, "-", 5},   {"5 * 5;", 5, "*", 5},
			{"5 / 5;", 5, "/", 5},   {"5 > 5;", 5, ">", 5},   {"5 < 5;", 5, "<", 5},
			{"5 == 5;", 5, "==", 5}, {"5 != 5;", 5, "!=", 5},
	};

	for (auto &tc : test_cases) {
		auto lexer = Lexer(tc.input);
		auto parser = Parser(std::make_unique<Lexer>(lexer));

		auto program = parser.parse_program();
		EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
		EXPECT_EQ(program->statements.size(), 1) << "Wrong amount of statements";

		auto stmt =
				dynamic_cast<ExpressionStatement *>(program->statements[0].get());
		EXPECT_NE(stmt, nullptr) << "Statement is not a expression statement";

		auto exp = dynamic_cast<InfixExpression *>(stmt->expression.get());
		EXPECT_NE(exp, nullptr) << "Statement is not a infix expression";

		EXPECT_EQ(tc.opr, exp->opr);
		EXPECT_TRUE(test_integer_literal(tc.left_value, std::move(exp->left)));
		EXPECT_TRUE(test_integer_literal(tc.right_value, std::move(exp->right)));
	}
}

TEST(ParserTest, BooleanInfixExpression) {
	struct Testcase {
		std::string input;
		bool left_value;
		std::string opr;
		bool right_value;
	};

	std::vector<Testcase> test_cases = {
			{"true == true", true, "==", true},
			{"true != false", true, "!=", false},
			{"false == false", false, "==", false},
	};

	for (auto &tc : test_cases) {
		auto lexer = Lexer(tc.input);
		auto parser = Parser(std::make_unique<Lexer>(lexer));

		auto program = parser.parse_program();
		EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
		EXPECT_EQ(program->statements.size(), 1) << "Wrong amount of statements";

		auto stmt =
				dynamic_cast<ExpressionStatement *>(program->statements[0].get());
		EXPECT_NE(stmt, nullptr) << "Statement is not a expression statement";

		auto exp = dynamic_cast<InfixExpression *>(stmt->expression.get());
		EXPECT_NE(exp, nullptr) << "Statement is not a infix expression";

		EXPECT_EQ(tc.opr, exp->opr);
		EXPECT_TRUE(test_bool_literal(tc.left_value, std::move(exp->left)));
		EXPECT_TRUE(test_bool_literal(tc.right_value, std::move(exp->right)));
	}
}

TEST(ParserTest, OperatorPrecedenceParsing) {
	struct Testcase {
		std::string input;
		std::string expected;
	};

	std::vector<Testcase> test_cases = {
			{
					"-a * b",
					"((-a) * b)",
			},
			{
					"!-a",
					"(!(-a))",
			},
			{
					"a + b + c",
					"((a + b) + c)",
			},
			{
					"a + b - c",
					"((a + b) - c)",
			},
			{
					"a * b * c",
					"((a * b) * c)",
			},
			{
					"a * b / c",
					"((a * b) / c)",
			},
			{
					"a + b / c",
					"(a + (b / c))",
			},
			{
					"a + b * c + d / e - f",
					"(((a + (b * c)) + (d / e)) - f)",
			},
			{
					"3 + 4; -5 * 5",
					"(3 + 4)((-5) * 5)",
			},
			{
					"5 > 4 == 3 < 4",
					"((5 > 4) == (3 < 4))",
			},
			{
					"5 < 4 != 3 > 4",
					"((5 < 4) != (3 > 4))",
			},
			{
					"5 > 4 == 3 < 4",
					"((5 > 4) == (3 < 4))",
			},
			{
					"5 < 4 != 3 > 4",
					"((5 < 4) != (3 > 4))",
			},
			{
					"3 + 4 * 5 == 3 * 1 + 4 * 5",
					"((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
			},
			{
					"true",
					"true",
			},
			{
					"false",
					"false",
			},
			{
					"3 > 5 == false",
					"((3 > 5) == false)",
			},
			{
					"3 < 5 == true",
					"((3 < 5) == true)",
			},
			{
					"1 + (2 + 3) + 4",
					"((1 + (2 + 3)) + 4)",
			},
			{
					"(5 + 5) * 2",
					"((5 + 5) * 2)",
			},
			{
					"2 / (5 + 5)",
					"(2 / (5 + 5))",
			},
			{
					"-(5 + 5)",
					"(-(5 + 5))",
			},
			{
					"!(true == true)",
					"(!(true == true))",
			}};

	for (auto &tc : test_cases) {
		auto lexer = Lexer(tc.input);
		auto parser = Parser(std::make_unique<Lexer>(lexer));

		auto program = parser.parse_program();
		EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

		auto actual = program->String();
		EXPECT_EQ(actual, tc.expected);
	}
}

TEST(ParserTest, IfExpressionTest) {
	std::string input = "if (x < y) { x }";
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	EXPECT_EQ(program->statements.size(), 1);

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr) << "The statement wasn't a expression statement";

	auto ifexp = dynamic_cast<IfExpression *>(expstmt->expression.get());
	EXPECT_NE(ifexp, nullptr);
}

TEST(ParserTest, FunctionLiteralParsing) {
	std::string input = "func(x, y) { x + y; }";
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";
	EXPECT_EQ(program->statements.size(), 1);

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr) << "The statement wasn't a expression statement";

	auto funcexp = dynamic_cast<FunctionLiteral *>(expstmt->expression.get());
	EXPECT_NE(funcexp, nullptr);

	EXPECT_EQ(funcexp->params.size(), 2);
}

TEST(ParserTest, FunctionParameterTest) {
	std::string input = "func(x, y, z) {};";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr) << "The statement wasn't a expression statement";

	auto funcexp = dynamic_cast<FunctionLiteral *>(expstmt->expression.get());
	EXPECT_NE(funcexp, nullptr);

	EXPECT_EQ(funcexp->params.size(), 3);

	EXPECT_EQ(funcexp->params[0]->value, "x");
	EXPECT_EQ(funcexp->params[1]->value, "y");
	EXPECT_EQ(funcexp->params[2]->value, "z");
}

TEST(ParserTest, CallExpressionParsing) {
	std::string input = "add(1, 2 * 3, 4 + 5);";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr) << "The statement wasn't a expression statement";

	auto callexp = dynamic_cast<CallExpression *>(expstmt->expression.get());
	EXPECT_NE(callexp, nullptr)
			<< "The expression statement wasn't a call expression";

	EXPECT_TRUE(test_integer_literal(1, std::move(callexp->arguments[0])));

	auto exp1 = dynamic_cast<InfixExpression *>(callexp->arguments[1].get());
	EXPECT_NE(exp1, nullptr) << "Statement is not a infix expression";

	EXPECT_EQ("*", exp1->opr);
	EXPECT_TRUE(test_integer_literal(2, std::move(exp1->left)));
	EXPECT_TRUE(test_integer_literal(3, std::move(exp1->right)));

	auto exp2 = dynamic_cast<InfixExpression *>(callexp->arguments[2].get());
	EXPECT_NE(exp2, nullptr) << "Statement is not a infix expression";

	EXPECT_EQ("+", exp2->opr);
	EXPECT_TRUE(test_integer_literal(4, std::move(exp2->left)));
	EXPECT_TRUE(test_integer_literal(5, std::move(exp2->right)));
}

TEST(ParserTest, StringLiteralTest) {
	std::string input = "\"hello world\";";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr) << "The statement wasn't a expression statement";

	EXPECT_EQ(((StringLiteral *)expstmt)->String(), "hello world");
}

TEST(ParserTest, ArrayLiteralParsing) {
	std::string input = "[1, 2 * 2, 3 + 3]";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr) << "The statement wasn't a expression statement";

	auto arraylit = dynamic_cast<ArrayLiteral *>(expstmt->expression.get());
	EXPECT_NE(arraylit, nullptr);

	EXPECT_EQ(arraylit->elements.size(), 3);
	EXPECT_TRUE(test_integer_literal(1, std::move(arraylit->elements[0])));

	auto exp1 = dynamic_cast<InfixExpression *>(arraylit->elements[1].get());
	EXPECT_NE(exp1, nullptr) << "Statement is not a infix expression";

	EXPECT_EQ("*", exp1->opr);
	EXPECT_TRUE(test_integer_literal(2, std::move(exp1->left)));
	EXPECT_TRUE(test_integer_literal(2, std::move(exp1->right)));

	auto exp2 = dynamic_cast<InfixExpression *>(arraylit->elements[2].get());
	EXPECT_NE(exp2, nullptr) << "Statement is not a infix expression";

	EXPECT_EQ("+", exp2->opr);
	EXPECT_TRUE(test_integer_literal(3, std::move(exp2->left)));
	EXPECT_TRUE(test_integer_literal(3, std::move(exp2->right)));
}

TEST(ParserTest, IndexExpression) {
	std::string input = "myArray[1 + 1]";

	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	auto program = parser.parse_program();
	EXPECT_NE(program, nullptr) << "Parsing program returns a nullptr";

	auto expstmt =
			dynamic_cast<ExpressionStatement *>(program->statements[0].get());
	EXPECT_NE(expstmt, nullptr) << "The statement wasn't a expression statement";

	auto idexp = dynamic_cast<IndexExpression *>(expstmt->expression.get());
	EXPECT_NE(idexp, nullptr);

	auto ident = dynamic_cast<Identifier *>(idexp->left.get());
	EXPECT_NE(ident, nullptr);

	auto infexp = dynamic_cast<InfixExpression *>(idexp->index.get());
	EXPECT_NE(infexp, nullptr) << "Statement is not a infix expression";

	EXPECT_EQ("+", infexp->opr);
	EXPECT_TRUE(test_integer_literal(1, std::move(infexp->left)));
	EXPECT_TRUE(test_integer_literal(1, std::move(infexp->right)));
}

bool test_integer_object(Object *obj, int expected) {
	auto res = dynamic_cast<Integer *>(obj);
	if (res == nullptr)
		return false;

	if (res->value != expected)
		return false;

	return true;
}

Object *eval_test(const std::string &input) {
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));
	auto program = parser.parse_program();
	return eval::Eval(program.get(), new Environment());
}

TEST(EvalTest, IntegerExpressions) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"5", 5},
			{"10", 10},
			{"-5", -5},
			{"-10", -10},
			{"5 + 5 + 5 + 5 - 10", 10},
			{"2 * 2 * 2 * 2 * 2", 32},
			{"-50 + 100 + -50", 0},
			{"5 * 2 + 10", 20},
			{"5 + 2 * 10", 25},
			{"20 + 2 * -10", 0},
			{"50 / 2 * 2 + 10", 60},
			{"2 * (5 + 10)", 30},
			{"3 * 3 * 3 + 10", 37},
			{"3 * (3 * 3) + 10", 37},
			{"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
	};

	for (auto &tc : test_cases) {
		auto obj = eval_test(tc.input);

		EXPECT_NE(obj, nullptr);
		test_integer_object(obj, tc.expected);

		auto res = (Integer *)obj;
		EXPECT_NE(res, nullptr) << "The object is not an integer";
		EXPECT_EQ(res->value, tc.expected) << "The values are not equal";
	}
}

bool test_boolean_object(Object *obj, bool expected) {
	auto res = dynamic_cast<Boolean *>(obj);
	if (res == nullptr)
		return false;

	if (res->value != expected)
		return false;

	return true;
}

TEST(EvalTest, BooleanExpression) {
	struct Testcase {
		std::string input;
		bool expected;
	};

	std::vector<Testcase> test_cases{
			{"true", true},
			{"false", false},
			{"1 < 2", true},
			{"1 > 2", false},
			{"1 < 1", false},
			{"1 > 1", false},
			{"1 == 1", true},
			{"1 != 1", false},
			{"1 == 2", false},
			{"1 != 2", true},
			{"true == true", true},
			{"false == false", true},
			{"true == false", false},
			{"true != false", true},
			{"false != true", true},
			{"(1 < 2) == true", true},
			{"(1 < 2) == false", false},
			{"(1 > 2) == true", false},
			{"(1 > 2) == false", true},
	};

	for (auto &tc : test_cases) {
		auto obj = eval_test(tc.input);

		EXPECT_NE(obj, nullptr);
		test_boolean_object(obj, tc.expected);
	}
}

TEST(EvalTest, PrefixExpressionTest) {
	struct Testcase {
		std::string input;
		bool expected;
	};

	std::vector<Testcase> test_cases{
			{"!true", false}, {"!false", true},   {"!5", false},
			{"!!true", true}, {"!!false", false}, {"!!5", true},
	};

	for (auto &tc : test_cases) {
		auto obj = eval_test(tc.input);

		EXPECT_NE(obj, nullptr);
		test_boolean_object(obj, tc.expected);
	}
}

TEST(EvalTest, IfElseTest) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"if (true) { 10 }", 10},
			{"if (false) { 10 }", 0},
			{"if (1) { 10 }", 10},
			{"if (1 < 2) { 10 }", 10},
			{"if (1 > 2) { 10 }", 0},
			{"if (1 > 2) { 10 } else { 20 }", 20},
			{"if (1 < 2) { 10 } else { 20 }", 10},
	};

	for (auto &tc : test_cases) {
		auto obj = eval_test(tc.input);
		EXPECT_NE(obj, nullptr);

		if (obj == object_constant::null) {
			EXPECT_EQ(0, tc.expected)
					<< "The value should expected value should be 0";
		} else {
			auto res = (Integer *)obj;
			EXPECT_NE(res, nullptr) << "The object is not an integer";
			EXPECT_EQ(res->value, tc.expected) << "The values are not equal";
		}
	}
}

TEST(EvalTest, ReturnTest) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"return 10;", 10},
			{"return 10; 9;", 10},
			{"return 2 * 5; 9;", 10},
			{"9; return 2 * 5; 9;", 10},
	};

	for (auto &tc : test_cases) {
		auto obj = eval_test(tc.input);
		EXPECT_NE(obj, nullptr);

		if (obj == object_constant::null) {
			EXPECT_EQ(0, tc.expected)
					<< "The value should expected value should be 0";
		} else {
			auto res = (Integer *)obj;
			EXPECT_NE(res, nullptr) << "The object is not an integer";
			EXPECT_EQ(res->value, tc.expected) << "The values are not equal";
		}
	}
}

TEST(EvalTest, ReturnComplex) {
	std::string input = "if (10 > 1) {"
											"  if (10 > 1) {"
											"     return 10;"
											"   }"
											"  return 1;"
											"}";

	auto obj = eval_test(input);
	EXPECT_NE(obj, nullptr);

	auto res = (Integer *)obj;
	EXPECT_NE(res, nullptr) << "The object is not an integer";
	EXPECT_EQ(res->value, 10) << "The values are not equal";
}

TEST(EvalTest, ErrorHandling) {
	struct Testcase {
		std::string input;
		std::string expected_msg;
	};

	std::vector<Testcase> test_cases{
			{"5 + true;", "wrong types: INTEGER + BOOLEAN"},
			{"5 + true; 5;", "wrong types: INTEGER + BOOLEAN"},
			{"-true", "unknown operation: -BOOLEAN"},
			{"true + false;", "unknown operation: BOOLEAN + BOOLEAN"},
			{"5; true + false; 5", "unknown operation: BOOLEAN + BOOLEAN"},
			{"if (10 > 1) { return true + false; }",
			 "unknown operation: BOOLEAN + BOOLEAN"},
			{"if (10 > 1) {"
			 "  if (10 > 1) {"
			 "     return true + false;"
			 "   }"
			 "  return 1;"
			 "}",
			 "unknown operation: BOOLEAN + BOOLEAN"},
			{"foobar", "identifier not found: foobar"}};

	for (auto &tc : test_cases) {
		auto obj = eval_test(tc.input);
		EXPECT_NE(obj, nullptr) << "The evaluated object was null.";
		EXPECT_EQ(obj->Type(), objecttypes::ERROR) << "The object wasn't an error";
	}
}

TEST(EvalTest, LetStatements) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"let a = 5; a;", 5},
			{"let a = 5 * 5; a;", 25},
			{"let a = 5; let b = a; b;", 5},
			{"let a = 5; let b = a; let c = a + b + 5; c;", 15},
	};

	for (auto &tc : test_cases) {
		auto obj = eval_test(tc.input);
		EXPECT_NE(obj, nullptr) << "The evaluated object was null.";

		auto res = (Integer *)obj;
		EXPECT_NE(res, nullptr) << "The object is not an integer";
		EXPECT_EQ(res->value, tc.expected) << "The values are not equal";
	}
}

TEST(EvalTest, FunctionObject) {
	std::string input = "func(x) { x + 2; };";
	auto obj = eval_test(input);
	EXPECT_NE(obj, nullptr);

	auto func = dynamic_cast<Function *>(obj);
	EXPECT_NE(func, nullptr);

	if (func != nullptr) {
		EXPECT_EQ(1, func->params.size());
		EXPECT_EQ("x", func->params[0]->value);
	}
}

TEST(EvalTest, FunctionUsage) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"let identity = func(x) { x; }; identity(5);", 5},
			{"let identity = func(x) { return x; }; identity(5);", 5},
			{"let double = func(x) { x * 2; }; double(5);", 10},
			{"let add = func(x, y) { x + y; }; add(5, 5);", 10},
			{"let add = func(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
			{"func(x) { x; }(5)", 5},
	};

	for (const auto &tc : test_cases) {
		auto obj = eval_test(tc.input);
		EXPECT_NE(obj, nullptr);

		auto res = dynamic_cast<Integer *>(obj);
		EXPECT_NE(res, nullptr) << "The object is not an integer";
		EXPECT_EQ(res->value, tc.expected) << "The values are not equal";
	}
}

TEST(EvalTest, StringLiteralTest) {
	std::vector<std::string> inputs{
			"\"Hello world!\"",
			"\"Hello\" + \" \" + \"world!\"",
	};

	for (const auto &input : inputs) {
		auto obj = eval_test(input);
		EXPECT_NE(obj, nullptr);

		auto res = dynamic_cast<String *>(obj);
		EXPECT_NE(res, nullptr);
		EXPECT_EQ(res->value, "Hello world!");
	}
}

TEST(EvalTest, BuiltInFunction) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"len(\"\")", 0},
			{"len(\"four\")", 4},
			{"len(\"hello world\")", 11},
			// -1 just means that it should return an error
			{"len(1)", -1},
			{"len(\"one\", \"two\")", -1}};

	for (const auto &tc : test_cases) {
		auto obj = eval_test(tc.input);
		EXPECT_NE(obj, nullptr);

		if (tc.expected == -1) {
			EXPECT_EQ(obj->Type(), objecttypes::ERROR);
		} else {
			auto res = dynamic_cast<Integer *>(obj);
			EXPECT_NE(res, nullptr);
			EXPECT_EQ(res->value, tc.expected);
		}
	}
}

TEST(EvalTest, ArrayLiteralEvaluation) {
	std::string input = "[1, 2 * 2, 3 + 3]";
	auto obj = eval_test(input);
	EXPECT_NE(obj, nullptr);

	auto arr = dynamic_cast<Array *>(obj);
	EXPECT_NE(arr, nullptr);

	EXPECT_EQ(arr->Type(), objecttypes::ARRAY_OBJ);

	EXPECT_TRUE(test_integer_object(arr->elements[0], 1))
			<< "first value is not 1";
	EXPECT_TRUE(test_integer_object(arr->elements[1], 4))
			<< "second value is not 4";
	EXPECT_TRUE(test_integer_object(arr->elements[2], 6))
			<< "third value is not 6";
}

TEST(EvalTest, ArrayIndexExpressions) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"[1, 2, 3][0]", 1},
			{"[1, 2, 3][1]", 2},
			{"[1, 2, 3][2]", 3},
			{"let i = 0; [1][i]", 1},
			{"[1, 2, 3][1+1]", 3},
			{"let arr = [1, 2, 3]; arr[2];", 3},
			{"let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", 6},
			{"let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]", 2},
			{"[1, 2, 3][3]", -1},
			{"[1, 2, 3][-1]", -1}};

	for (const auto &tc : test_cases) {
		auto obj = eval_test(tc.input);
		EXPECT_NE(obj, nullptr);

		if (tc.expected == -1) {
			EXPECT_EQ(objecttypes::NULLOBJ, obj->Type())
					<< "The object isn't of type null";
		} else {
			EXPECT_TRUE(test_integer_object(obj, tc.expected))
					<< "The value don't match";
		}
	}
}

TEST(ObjectTest, HashKeyTest) {
	auto hello1 = new String("hello world");
	auto hello2 = new String("hello world");
	auto value1 = new String("world hello");
	auto value2 = new String("world hello");

	EXPECT_EQ(hello1->hash_key().value, hello2->hash_key().value)
			<< "Strings with same content have different hash keys";
	EXPECT_EQ(value1->hash_key().value, value2->hash_key().value)
			<< "Strings with same content have different hash keys";
	EXPECT_NE(hello1->hash_key().value, value1->hash_key().value)
			<< "Strings with different content have same hash keys";
}

TEST(EvalTest, HashEvaluationTest) {
	std::string input = "let two = \"two\";"
											"{"
											"\"one\": 10 - 9,"
											"two: 1 + 1,"
											"\"thr\" + \"ee\": 6 / 2,"
											"4: 4,"
											"true: 5,"
											"false: 6"
											"}";

	auto obj = eval_test(input);
	EXPECT_NE(obj, nullptr);

	auto hash = dynamic_cast<Hash *>(obj);
	EXPECT_NE(hash, nullptr);

	struct Testcase {
		HashKey hk;
		int expected;
	};

	std::vector<Testcase> expected{
			{(new String("one"))->hash_key(), 1},
			{(new String("two"))->hash_key(), 2},
			{(new String("three"))->hash_key(), 3},
			{(new Integer(4))->hash_key(), 4},
			{(new Boolean(true))->hash_key(), 5},
			{(new Boolean(false))->hash_key(), 6},
	};

	EXPECT_EQ(expected.size(), hash->pairs.size())
			<< "The is a wrong amount of pairs in the hashtable";
	for (const auto &tc : expected) {
		auto pair = hash->pairs[tc.hk.value];
		EXPECT_NE(pair, nullptr) << "the pair shouldn't be a null pointer";

		EXPECT_TRUE(test_integer_object(pair->value, tc.expected));
	}
}

TEST(EvalTest, HashIndexExpressions) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			// -1 means an null return
			{"{\"foo\": 5}[\"foo\"]", 5},
			{"{\"foo\": 5}[\"bar\"]", -1},
			{"let key = \"foo\"; {\"foo\": 5}[key]", 5},
			{"{}[\"foo\"]", -1},
			{"{5: 5}[5]", 5},
			{"{true: 5}[true]", 5},
			{"{false: 5}[false]", 5},
	};

	for (auto const &tc : test_cases) {
		auto obj = eval_test(tc.input);
		EXPECT_NE(obj, nullptr);

		if (tc.expected == -1) {
			EXPECT_EQ(objecttypes::NULLOBJ, obj->Type());
		} else {
			EXPECT_TRUE(test_integer_object(obj, tc.expected));
		}
	}
}

TEST(CodeTest, Make) {
	struct Testcase {
		code::Opcode op;
		std::vector<int> operands;
		std::vector<char> expected;
	};

	std::vector<Testcase> test_cases{
			{code::OpConstant, std::vector<int>{65534},
			 std::vector<char>{code::OpConstant, (char)255, (char)254}},
			{code::OpAdd, std::vector<int>{}, std::vector<char>{code::OpAdd}},
	};

	for (auto &tc : test_cases) {
		auto instructions = code::make(tc.op, tc.operands);

		EXPECT_EQ(instructions.size(), tc.expected.size())
				<< "instruction has wrong length";
		for (int i = 0; i < (int)instructions.size(); ++i) {
			EXPECT_EQ(instructions[i], tc.expected[i])
					<< "wrong byte as position: " << i;
		}
	}
}

std::unique_ptr<Program> parse_compiler_program_helper(std::string input) {
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	return parser.parse_program();
}

code::Instructions concat_instructions(std::vector<code::Instructions> &instr) {
	auto out = code::Instructions();
	for (auto i : instr) {
		out.insert(out.end(), i.begin(), i.end());
	}

	return out;
}

bool test_instructions(std::vector<code::Instructions> inst,
											 code::Instructions actual) {
	auto concatted = concat_instructions(inst);
	std::cerr << code::instructions_to_string(concatted) << '\n';
	std::cerr << code::instructions_to_string(actual) << '\n';

	if (concatted.size() != actual.size())
		return false;

	for (int i = 0; i < (int)concatted.size(); ++i) {
		if (actual[i] != concatted[i]) {
			return false;
		}
	}

	return true;
}

TEST(CompilerTest, IntegerArithmetic) {
	struct CompilerTestCaseInteger {
		std::string input;
		std::vector<int> expected_constants;
		std::vector<code::Instructions> expected_instructions;
	};

	std::vector<CompilerTestCaseInteger> test_cases{
			{"1 + 2",
			 std::vector<int>{1, 2},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpAdd, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"1; 2",
			 std::vector<int>{1, 2},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpPop, std::vector<int>{}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},

			{"1 * 2",
			 std::vector<int>{1, 2},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpMul, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"1 - 2",
			 std::vector<int>{1, 2},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpSub, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"2 / 1",
			 std::vector<int>{2, 1},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpDiv, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"-1",
			 std::vector<int>{1},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpMinus, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
	};

	for (const auto &tt : test_cases) {
		std::unique_ptr<Program> program = parse_compiler_program_helper(tt.input);

		auto compiler = new Compiler();
		auto status = compiler->compile(program.get());
		EXPECT_EQ(status, 0) << "The compilation was successful";

		auto bytecode = compiler->bytecode();
		EXPECT_TRUE(
				test_instructions(tt.expected_instructions, bytecode->instructions));

		EXPECT_EQ(tt.expected_constants.size(), bytecode->constants.size());

		for (int i = 0; i < (int)tt.expected_constants.size(); ++i) {
			EXPECT_TRUE(test_integer_object(bytecode->constants[i],
																			tt.expected_constants[i]));
		}
	}
}

TEST(CompilerTest, ConditionalTest) {
	struct CompilerTestCaseConditional {
		std::string input;
		std::vector<int> expected_constants;
		std::vector<code::Instructions> expected_instructions;
	};

	std::vector<CompilerTestCaseConditional> test_cases{
			{"if (true) { 10 } else { 20 }; 3333;",
			 std::vector<int>{10, 20, 3333},
			 {{
					 // 0000
					 code::make(code::OpTrue, std::vector<int>{}),
					 // 0001
					 code::make(code::OpJumpNotTruthy, std::vector<int>{10}),
					 // 0004
					 code::make(code::OpConstant, std::vector<int>{0}),
					 // 0007
					 code::make(code::OpJump, std::vector<int>{13}),
					 // 0010
					 code::make(code::OpConstant, std::vector<int>{1}),
					 // 0013
					 code::make(code::OpPop, std::vector<int>{}),
					 // 0014
					 code::make(code::OpConstant, std::vector<int>{2}),
					 // 0017
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"if (true) { 10 }; 3333;",
			 std::vector<int>{10, 3333},
			 {{
					 // 0000
					 code::make(code::OpTrue, std::vector<int>{}),
					 // 0001
					 code::make(code::OpJumpNotTruthy, std::vector<int>{10}),
					 // 0004
					 code::make(code::OpConstant, std::vector<int>{0}),
					 // 0007
					 code::make(code::OpJump, std::vector<int>{11}),
					 // 0010
					 code::make(code::OpNull, std::vector<int>{}),
					 // 0011
					 code::make(code::OpPop, std::vector<int>{}),
					 // 0012
					 code::make(code::OpConstant, std::vector<int>{1}),
					 // 0015
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
	};

	for (const auto &tt : test_cases) {
		std::unique_ptr<Program> program = parse_compiler_program_helper(tt.input);

		auto compiler = new Compiler();
		auto status = compiler->compile(program.get());
		EXPECT_EQ(status, 0) << "The compilation was successful";

		auto bytecode = compiler->bytecode();
		EXPECT_TRUE(
				test_instructions(tt.expected_instructions, bytecode->instructions));

		EXPECT_EQ(tt.expected_constants.size(), bytecode->constants.size());

		for (int i = 0; i < (int)tt.expected_constants.size(); ++i) {
			EXPECT_TRUE(
					test_integer_object(bytecode->constants[i], tt.expected_constants[i]))
					<< code::instructions_to_string(bytecode->instructions);
		}
	}
}

TEST(CompilerTest, BooleanExpressions) {
	struct CompilerTestCaseBoolean {
		std::string input;
		std::vector<int> expected_constants;
		std::vector<code::Instructions> expected_instructions;
	};

	std::vector<CompilerTestCaseBoolean> test_cases{
			{"true",
			 std::vector<int>{},
			 {{
					 code::make(code::OpTrue, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"false",
			 std::vector<int>{},
			 {{
					 code::make(code::OpFalse, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"1 > 2",
			 std::vector<int>{1, 2},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpGreaterThan, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"1 < 2",
			 std::vector<int>{2, 1},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpGreaterThan, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"1 == 2",
			 std::vector<int>{1, 2},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpEqual, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"1 != 2",
			 std::vector<int>{1, 2},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpNotEqual, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"true == false",
			 std::vector<int>{},
			 {{
					 code::make(code::OpTrue, std::vector<int>{}),
					 code::make(code::OpFalse, std::vector<int>{}),
					 code::make(code::OpEqual, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"true != false",
			 std::vector<int>{},
			 {{
					 code::make(code::OpTrue, std::vector<int>{}),
					 code::make(code::OpFalse, std::vector<int>{}),
					 code::make(code::OpNotEqual, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{"!true",
			 std::vector<int>{},
			 {{
					 code::make(code::OpTrue, std::vector<int>{}),
					 code::make(code::OpBang, std::vector<int>{}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
	};

	for (const auto &tt : test_cases) {
		std::unique_ptr<Program> program = parse_compiler_program_helper(tt.input);

		auto compiler = new Compiler();
		auto status = compiler->compile(program.get());
		EXPECT_EQ(status, 0) << "The compilation was successful";

		auto bytecode = compiler->bytecode();
		EXPECT_TRUE(
				test_instructions(tt.expected_instructions, bytecode->instructions));

		EXPECT_EQ(tt.expected_constants.size(), bytecode->constants.size());

		for (int i = 0; i < (int)tt.expected_constants.size(); ++i) {
			EXPECT_TRUE(
					test_integer_object(bytecode->constants[i], tt.expected_constants[i]))
					<< "object evaluation fails with input " << tt.input;
		}
	}
}

TEST(CodeTest, InstructionString) {
	std::vector<code::Instructions> instructions{
			{code::make(code::OpAdd, std::vector<int>{})},
			{code::make(code::OpConstant, std::vector<int>{2})},
			{code::make(code::OpConstant, std::vector<int>{65535})}};

	std::string expected =
			"0000 OpAdd\n0001 OpConstant 2\n0004 OpConstant 65535\n";
	auto concatted = concat_instructions(instructions);

	EXPECT_EQ(code::instructions_to_string(concatted), expected);
}

TEST(CodeTest, ReadOperands) {
	struct Testcase {
		code::Opcode op;
		std::vector<int> operands;
		int bytes_read;
	};

	std::vector<Testcase> test_cases{
			{code::OpConstant, std::vector<int>{65535}, 2},
	};

	for (const auto &tc : test_cases) {
		auto instructions = code::make(tc.op, tc.operands);

		auto def = code::look_up(tc.op);
		EXPECT_NE(def, nullptr);

		auto res = code::read_operands(
				def, code::Instructions(instructions.begin() + 1, instructions.end()));
		EXPECT_EQ(res.second, tc.bytes_read);

		for (int i = 0; i < (int)tc.operands.size(); ++i) {
			EXPECT_EQ(res.first[i], tc.operands[i])
					<< "operands wrong got " << res.first[i] << " want "
					<< tc.operands[i];
		}
	}
}

TEST(VMTest, VMIntegerArithmetic) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"1", 1},
			{"2", 2},
			{"1 + 2", 3},
			{"4 / 2", 2},
			{"50 / 2 * 2 + 10 - 5", 55},
			{"5 * (2 + 10)", 60},
			{"5 + 5 + 5 + 5 - 10", 10},
			{"2 * 2 * 2 * 2 * 2", 32},
			{"5 * 2 + 10", 20},
			{"5 + 2 * 10", 25},
			{"5 * (2 + 10)", 60},
			{"-5", -5},
			{"-10", -10},
			{"-50 + 100 + -50", 0},
			{"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
	};

	for (auto const &tt : test_cases) {
		std::unique_ptr<Program> program = parse_compiler_program_helper(tt.input);
		auto comp = new Compiler();
		auto status = comp->compile(program.get());
		EXPECT_EQ(status, 0);

		auto vm = new VM(comp->bytecode());
		auto vm_status = vm->run();
		EXPECT_EQ(vm_status, 0);

		auto stack_elem = vm->last_popped_stack_elem();
		EXPECT_NE(stack_elem, nullptr);

		EXPECT_TRUE(test_integer_object(stack_elem, tt.expected));
	}
}

TEST(VmTest, VMBooleanTest) {
	struct Testcase {
		std::string input;
		bool expected;
	};

	std::vector<Testcase> test_cases{
			{"true", true},
			{"false", false},
			{"1 < 2", true},
			{"1 > 2", false},
			{"1 < 1", false},
			{"1 > 1", false},
			{"1 == 1", true},
			{"1 != 1", false},
			{"1 == 2", false},
			{"1 != 2", true},
			{"true == true", true},
			{"false == false", true},
			{"true == false", false},
			{"true != false", true},
			{"false != true", true},
			{"(1 < 2) == true", true},
			{"(1 < 2) == false", false},
			{"(1 > 2) == true", false},
			{"(1 > 2) == false", true},
			{"!true", false},
			{"!false", true},
			{"!5", false},
			{"!!true", true},
			{"!!false", false},
			{"!!5", true},
	};

	for (auto const &tt : test_cases) {
		std::unique_ptr<Program> program = parse_compiler_program_helper(tt.input);
		auto comp = new Compiler();
		auto status = comp->compile(program.get());
		EXPECT_EQ(status, 0);

		auto vm = new VM(comp->bytecode());
		auto vm_status = vm->run();
		EXPECT_EQ(vm_status, 0);

		auto stack_elem = vm->last_popped_stack_elem();
		EXPECT_NE(stack_elem, nullptr);

		EXPECT_TRUE(test_boolean_object(stack_elem, tt.expected));
	}
}

TEST(VmTest, Conditionals) {
	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"if (true) { 10 }", 10},
			{"if (true) { 10 } else { 20 }", 10},
			{"if (false) { 10 } else { 20 } ", 20},
			{"if (1) { 10 }", 10},
			{"if (1 < 2) { 10 }", 10},
			{"if (1 < 2) { 10 } else { 20 }", 10},
			{"if (1 > 2) { 10 } else { 20 }", 20},
			{"if ((if (false) { 10 })) { 10 } else { 20 }", 20},

			// -1 just means that the returned value should be null in the context of
			// this test.
			{"if (1 > 2) { 10 }", -1},
			{"if (false) { 10 }", -1}};

	for (auto const &tt : test_cases) {
		std::unique_ptr<Program> program = parse_compiler_program_helper(tt.input);
		auto comp = new Compiler();
		auto status = comp->compile(program.get());
		EXPECT_EQ(status, 0);

		auto vm = new VM(comp->bytecode());
		auto vm_status = vm->run();
		EXPECT_EQ(vm_status, 0);

		auto stack_elem = vm->last_popped_stack_elem();
		EXPECT_NE(stack_elem, nullptr);

		if (tt.expected == -1) {
			EXPECT_EQ(stack_elem->Type(), objecttypes::NULLOBJ);
		} else {
			EXPECT_TRUE(test_integer_object(stack_elem, tt.expected));
		}
	}
}

TEST(CompilerTest, GlobalLetStatements) {
	struct CompilerTestCaseLetStatement {
		std::string input;
		std::vector<int> expected_constants;
		std::vector<code::Instructions> expected_instructions;
	};

	std::vector<CompilerTestCaseLetStatement> test_cases{
			{
				"let one = 1;"
				"let two = 2",
				std::vector<int>{1, 2},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpSetGlobal, std::vector<int>{0}),
					 code::make(code::OpConstant, std::vector<int>{1}),
					 code::make(code::OpSetGlobal, std::vector<int>{1}),
			 }}},
			{
				"let one = 1;"
				"one;",
				std::vector<int>{1},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpSetGlobal, std::vector<int>{0}),
					 code::make(code::OpGetGlobal, std::vector<int>{0}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
			{
				"let one = 1;"
				"let two = one;"
				"two;",
				std::vector<int>{1},
			 {{
					 code::make(code::OpConstant, std::vector<int>{0}),
					 code::make(code::OpSetGlobal, std::vector<int>{0}),
					 code::make(code::OpGetGlobal, std::vector<int>{0}),
					 code::make(code::OpSetGlobal, std::vector<int>{1}),
					 code::make(code::OpGetGlobal, std::vector<int>{1}),
					 code::make(code::OpPop, std::vector<int>{}),
			 }}},
	};
}

TEST(SymbolTableTest, TestDefine) {
	auto global = new SymbolTable();

	auto a = global->define("a");
	EXPECT_NE(a, nullptr);

	EXPECT_EQ(a->name, "a");
	EXPECT_EQ(a->scope, scopes::GlobalScope);
	EXPECT_EQ(a->index, 0);

	auto b = global->define("b");
	EXPECT_NE(b, nullptr);

	EXPECT_EQ(b->name, "b");
	EXPECT_EQ(b->scope, scopes::GlobalScope);
	EXPECT_EQ(b->index, 1);
}

TEST(SymbolTableTest, TestResolveGlobal) {
	auto global = new SymbolTable();
	auto a = global->define("a");
	auto b = global->define("b");

	std::vector<Symbol> expected{
		{"a", scopes::GlobalScope, 0},
		{"b", scopes::GlobalScope, 1},
	};

	for (const auto& tt : expected) {
		auto res = global->resolve(tt.name);
		EXPECT_NE(res, nullptr);

		EXPECT_EQ(res->name, tt.name);
		EXPECT_EQ(res->scope, tt.scope);
		EXPECT_EQ(res->index, tt.index);
	}
}
