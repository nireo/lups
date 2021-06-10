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
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

const std::string
run_lexer_tests(const std::string &input,
								std::vector<std::pair<TokenType, std::string>> expected) {
	auto lexer = Lexer(input);
	for (int i = 0; i < (int)expected.size(); ++i) {
		auto token = lexer.next_token();

		if (expected[i].first != token.type) {
			return "token types don't match at index: " + std::to_string(i);
		}

		if (expected[i].second != token.literal) {
			return "token literals don't match at index: " + std::to_string(i);
		}
	}

	// no error
	return "";
}

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

	auto err = run_lexer_tests(input, expected);
	EXPECT_EQ(err, "") << err;
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

	auto err = run_lexer_tests(input, expected);
	EXPECT_EQ(err, "") << err;
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

	auto err = run_lexer_tests(input, expected);
	EXPECT_EQ(err, "") << err;
}

TEST(LexerTest, Strings) {
	std::string input = "\"foobar\""
											"\"foo bar\"";

	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::STRING, "foobar"));
	expected.push_back(std::make_pair(tokentypes::STRING, "foo bar"));

	auto err = run_lexer_tests(input, expected);
	EXPECT_EQ(err, "") << err;
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

	auto err = run_lexer_tests(input, expected);
	EXPECT_EQ(err, "") << err;
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

	auto err = run_lexer_tests(input, expected);
	EXPECT_EQ(err, "") << err;
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

	auto err = run_lexer_tests(input, expected);
	EXPECT_EQ(err, "") << err;
}

TEST(LexerTest, HashTest) {
	std::string input = "{\"foo\": \"bar\"}";
	std::vector<std::pair<TokenType, std::string>> expected;
	expected.push_back(std::make_pair(tokentypes::LBRACE, "{"));
	expected.push_back(std::make_pair(tokentypes::STRING, "foo"));
	expected.push_back(std::make_pair(tokentypes::COLON, ":"));
	expected.push_back(std::make_pair(tokentypes::STRING, "bar"));
	expected.push_back(std::make_pair(tokentypes::RBRACE, "}"));

	auto err = run_lexer_tests(input, expected);
	EXPECT_EQ(err, "") << err;
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
		EXPECT_EQ(program->statements[i]->Type(), AstType::LetStatement);
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
		EXPECT_EQ(program->statements[i]->Type(), AstType::ReturnStatement);
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
		EXPECT_EQ(obj->Type(), ObjType::Error) << "The object wasn't an error";
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
			EXPECT_EQ(obj->Type(), ObjType::Error);
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

	EXPECT_EQ(arr->Type(), ObjType::Array);

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
			EXPECT_EQ(ObjType::Null, obj->Type())
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
			EXPECT_EQ(ObjType::Null, obj->Type());
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
			{code::OpGetLocal, {255}, {code::OpGetLocal, (char)255}},
			{code::OpClosure, {65534, 255}, {code::OpClosure, (char)255, (char)254, (char)255 }},
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

	if (concatted.size() != actual.size())
		return false;

	for (int i = 0; i < (int)concatted.size(); ++i) {
		if (actual[i] != concatted[i]) {
			return false;
		}
	}

	return true;
}

bool test_string_constant(const std::string &expected, Object *obj) {
	auto res = dynamic_cast<String *>(obj);
	if (res == nullptr)
		return false;

	if (res->value != expected)
		return false;

	return true;
}

template <typename T> struct CompilerTestcase {
	std::string input;
	std::vector<T> expected_constants;
	std::vector<code::Instructions> expected_instructions;
};

template <typename T>
const std::string
run_compiler_tests(const std::vector<CompilerTestcase<T>> &tests) {
	for (const auto &test : tests) {
		auto program = parse_compiler_program_helper(test.input);

		auto compiler = new Compiler();
		auto status = compiler->compile(program.get());
		if (status != 0)
			return "The compilation was unsuccessful";

		auto bytecode = compiler->bytecode();
		if (!test_instructions(test.expected_instructions, bytecode->instructions))
			return "The instructions don't match";
		if (test.expected_constants.size() != bytecode->constants.size())
			return "The amount of constants differs";

		if constexpr (std::is_same<int, T>::value) {
			for (int i = 0; i < (int)test.expected_constants.size(); ++i) {
				if (!test_integer_object(bytecode->constants[i],
																 test.expected_constants[i]))
					return "integer constants don't match";
			}
		} else if constexpr (std::is_same<std::string, T>::value) {
			for (int i = 0; i < (int)test.expected_constants.size(); ++i) {
				if (!test_string_constant(test.expected_constants[i],
																	bytecode->constants[i]))
					return "string constants don't match";
			}
		} else if constexpr (std::is_same<Object *, T>::value) {
			for (int i = 0; i < (int)test.expected_constants.size(); ++i) {
				if (test.expected_constants[i]->Type() ==
						ObjType::CompiledFunction) {
					auto expected_fn =
							dynamic_cast<CompiledFunction *>(test.expected_constants[i]);
					if (expected_fn == nullptr)
						return "Cannot convert object into compiled function.";

					auto actual_fn =
							dynamic_cast<CompiledFunction *>(bytecode->constants[i]);
					if (actual_fn == nullptr)
						return "Cannot convert object into compiled function.";

					if (!test_instructions({expected_fn->m_instructions},
																 actual_fn->m_instructions))
						return "The function instructions don't match";
				} else if (test.expected_constants[i]->Type() == ObjType::Integer) {
					auto expected_int =
							dynamic_cast<Integer *>(test.expected_constants[i]);
					if (expected_int == nullptr)
						return "Cannot convert object into compiled function.";

					auto actual_int = dynamic_cast<Integer *>(bytecode->constants[i]);
					if (actual_int == nullptr)
						return "Cannot convert object into compiled function.";

					if (actual_int->value != expected_int->value)
						return "Integer values don't match";
				}
			}
		}
	}

	return "";
}

template <typename T> struct VMTestcase {
	std::string input;
	T expected;
};

template <typename T>
const std::string run_vm_tests(const std::vector<VMTestcase<T>> &tests) {
	for (auto const &tt : tests) {
		auto program = parse_compiler_program_helper(tt.input);
		auto comp = new Compiler();
		auto status = comp->compile(program.get());
		if (status != 0)
			return "The compilation was unsuccessful";

		auto vm = new VM(comp->bytecode());
		auto vm_status = vm->run();
		if (vm_status != 0)
			return "Running the vm was unsuccessful";

		auto stack_elem = vm->last_popped_stack_elem();
		if (stack_elem == nullptr)
			return "The stack element is a null pointer.";

		if constexpr (std::is_same<int, T>::value) {
			// In some test cases -1 is used to indicate that it should return null.
			if (tt.expected == -1) {
				if (stack_elem->Type() != ObjType::Null)
					return "stack_elem is not null even though it should";
			} else if (tt.expected == -2) {
				if (stack_elem->Type() != ObjType::Error)
					return "stack_elem is not an error even though it should be.";
			} else {
				if (!test_integer_object(stack_elem, tt.expected))
					return "The integer object is invalid";
			}
		} else if constexpr (std::is_same<std::string, T>::value) {
			if (!test_string_constant(tt.expected, stack_elem))
				return "The string object is invalid";
		} else if constexpr (std::is_same<bool, T>::value) {
			if (!test_boolean_object(stack_elem, tt.expected))
				return "The boolean object is invalid";
		} else if constexpr (std::is_same<std::vector<int>, T>::value) {
			auto arr = dynamic_cast<Array *>(stack_elem);
			if (arr->elements.size() != tt.expected.size())
				return "The amount of elements differs in the array.";
			for (int i = 0; i < arr->elements.size(); ++i) {
				if (!test_integer_object(arr->elements[i], tt.expected[i]))
					return "The integer object is invalid";
			}
		} else if constexpr (std::is_same<std::map<int, int>, T>::value) {
			auto hash = dynamic_cast<Hash *>(stack_elem);
			if (hash == nullptr)
				return "The class is not a Hash object";

			if (hash->pairs.size() != tt.expected.size())
				return "The amount of elements differ in the hash class";

			for (const auto &e : tt.expected) {
				auto pr = hash->pairs[e.first];
				if (pr == nullptr)
					return "A pair was not found in the map";

				if (!test_integer_object(pr->value, e.second))
					return "The integer object is invalid";
			}
		}
	}

	return "";
}

TEST(CompilerTest, IntegerArithmetic) {
	std::vector<CompilerTestcase<int>> test_cases{
			{"1 + 2",
			 {1, 2},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpAdd, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"1; 2",
			 {1, 2},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpPop, {}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpPop, {}),
			 }}},

			{"1 * 2",
			 {1, 2},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpMul, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"1 - 2",
			 {1, 2},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpSub, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"2 / 1",
			 {2, 1},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpDiv, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"-1",
			 {1},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpMinus, {}),
					 code::make(code::OpPop, {}),
			 }}},
	};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, ConditionalTest) {
	std::vector<CompilerTestcase<int>> test_cases{
			{"if (true) { 10 } else { 20 }; 3333;",
			 {10, 20, 3333},
			 {{
					 code::make(code::OpTrue, {}),
					 code::make(code::OpJumpNotTruthy, {10}),
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpJump, {13}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpPop, {}),
					 code::make(code::OpConstant, {2}),
					 code::make(code::OpPop, {}),
			 }}},
			{"if (true) { 10 }; 3333;",
			 {10, 3333},
			 {{
					 code::make(code::OpTrue, {}),
					 code::make(code::OpJumpNotTruthy, {10}),
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpJump, {11}),
					 code::make(code::OpNull, {}),
					 code::make(code::OpPop, {}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpPop, {}),
			 }}},
	};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, BooleanExpressions) {
	std::vector<CompilerTestcase<int>> test_cases{
			{"true",
			 {},
			 {{
					 code::make(code::OpTrue, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"false",
			 {},
			 {{
					 code::make(code::OpFalse, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"1 > 2",
			 {1, 2},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpGreaterThan, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"1 < 2",
			 {2, 1},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpGreaterThan, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"1 == 2",
			 {1, 2},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpEqual, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"1 != 2",
			 {1, 2},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpNotEqual, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"true == false",
			 {},
			 {{
					 code::make(code::OpTrue, {}),
					 code::make(code::OpFalse, {}),
					 code::make(code::OpEqual, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"true != false",
			 {},
			 {{
					 code::make(code::OpTrue, {}),
					 code::make(code::OpFalse, {}),
					 code::make(code::OpNotEqual, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"!true",
			 {},
			 {{
					 code::make(code::OpTrue, {}),
					 code::make(code::OpBang, {}),
					 code::make(code::OpPop, {}),
			 }}},
	};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CodeTest, InstructionString) {
	std::vector<code::Instructions> instructions{
			{code::make(code::OpAdd, {})},
			{code::make(code::OpGetLocal, {1})},
			{code::make(code::OpConstant, {2})},
			{code::make(code::OpConstant, {65535})},
			{code::make(code::OpClosure, {65535, 255})}};

	std::string expected = "0000 OpAdd\n0001 OpGetLocal 1\n0003 OpConstant "
												 "2\n0006 OpConstant 65535\n0009 OpClosure 65535 255\n";
	std::cerr << expected;
	auto concatted = concat_instructions(instructions);

	EXPECT_EQ(code::instructions_to_string(concatted), expected);
	std::cerr << code::instructions_to_string(concatted);
}

TEST(CodeTest, ReadOperands) {
	struct Testcase {
		code::Opcode op;
		std::vector<int> operands;
		int bytes_read;
	};

	std::vector<Testcase> test_cases{{code::OpConstant, {65535}, 2},
																	 {code::OpGetLocal, {255}, 1},
																	 {code::OpClosure, {65535, 255}, 3}
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
	std::vector<VMTestcase<int>> test_cases{
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

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, VMBooleanTest) {
	std::vector<VMTestcase<bool>> test_cases{
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

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, Conditionals) {
	std::vector<VMTestcase<int>> test_cases{
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

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, StringExpressions) {
	std::vector<CompilerTestcase<std::string>> test_cases{
			{"\"lups\"",
			 {"lups"},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"\"lu\" + \"ps\"",
			 {"lu", "ps"},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpAdd, {}),
					 code::make(code::OpPop, {}),
			 }}}};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, GlobalLetStatements) {
	std::vector<CompilerTestcase<int>> test_cases{
			{"let one = 1;"
			 "let two = 2;",
			 {1, 2},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpSetGlobal, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpSetGlobal, {1}),
			 }}},
			{"let one = 1;"
			 "one;",
			 {1},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpSetGlobal, {0}),
					 code::make(code::OpGetGlobal, {0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"let one = 1;"
			 "let two = one;"
			 "two;",
			 {1},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpSetGlobal, {0}),
					 code::make(code::OpGetGlobal, {0}),
					 code::make(code::OpSetGlobal, {1}),
					 code::make(code::OpGetGlobal, {1}),
					 code::make(code::OpPop, {}),
			 }}},
	};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(SymbolTableTest, TestDefine) {
	std::map<std::string, Symbol> ex{
			{"a", {"a", scopes::GlobalScope, 0}},
			{"b", {"b", scopes::GlobalScope, 1}},
			{"c", {"c", scopes::LocalScope, 0}},
			{"d", {"d", scopes::LocalScope, 1}},
			{"e", {"e", scopes::LocalScope, 0}},
			{"f", {"f", scopes::LocalScope, 1}},
	};

	auto global = new SymbolTable();

	const auto& a = global->define("a");

	EXPECT_EQ(a.name, ex["a"].name);
	EXPECT_EQ(a.scope, ex["a"].scope);
	EXPECT_EQ(a.index, ex["a"].index);

	const auto& b = global->define("b");

	EXPECT_EQ(b.name, ex["b"].name);
	EXPECT_EQ(b.scope, ex["b"].scope);
	EXPECT_EQ(b.index, ex["b"].index);

	auto local1 = new SymbolTable(global);

	const auto& c = local1->define("c");

	EXPECT_EQ(c.name, ex["c"].name);
	EXPECT_EQ(c.scope, ex["c"].scope);
	EXPECT_EQ(c.index, ex["c"].index);

	const auto& d = local1->define("d");

	EXPECT_EQ(d.name, ex["d"].name);
	EXPECT_EQ(d.scope, ex["d"].scope);
	EXPECT_EQ(d.index, ex["d"].index);

	auto local2 = new SymbolTable(local1);
	const auto& e = local2->define("e");

	EXPECT_EQ(e.name, ex["e"].name);
	EXPECT_EQ(e.scope, ex["e"].scope);
	EXPECT_EQ(e.index, ex["e"].index);

	const auto& f = local2->define("f");

	EXPECT_EQ(f.name, ex["f"].name);
	EXPECT_EQ(f.scope, ex["f"].scope);
	EXPECT_EQ(f.index, ex["f"].index);
}

TEST(SymbolTableTest, TestResolveGlobal) {
	auto global = new SymbolTable();
	auto a = global->define("a");
	auto b = global->define("b");

	std::vector<Symbol> expected{
			{"a", scopes::GlobalScope, 0},
			{"b", scopes::GlobalScope, 1},
	};

	for (const auto &tt : expected) {
		auto res = global->resolve(tt.name);
		EXPECT_NE(res, nullptr);

		EXPECT_EQ(res->name, tt.name);
		EXPECT_EQ(res->scope, tt.scope);
		EXPECT_EQ(res->index, tt.index);
	}
}

TEST(SymbolTableTest, TestResolveLocal) {
	auto global = new SymbolTable();
	auto a = global->define("a");
	auto b = global->define("b");

	auto local = new SymbolTable(global);
	auto c = local->define("c");
	auto d = local->define("d");

	std::vector<Symbol> expected{
			{"a", scopes::GlobalScope, 0},
			{"b", scopes::GlobalScope, 1},
			{"c", scopes::LocalScope, 0},
			{"d", scopes::LocalScope, 1},
	};

	for (const auto &tt : expected) {
		const auto &res = local->resolve(tt.name);
		EXPECT_NE(res, nullptr);

		EXPECT_EQ(res->name, tt.name);
		EXPECT_EQ(res->scope, tt.scope);
		EXPECT_EQ(res->index, tt.index);
	}
}

TEST(SymbolTableTest, ResolveNestedLocal) {
	auto global = new SymbolTable();
	global->define("a");
	global->define("b");

	auto local1 = new SymbolTable(global);
	local1->define("c");
	local1->define("d");

	auto local2 = new SymbolTable(local1);
	local2->define("e");
	local2->define("f");

	struct Testcase {
		SymbolTable *table;
		std::vector<Symbol> expected;
	};

	std::vector<Testcase> test_cases{{local1,
																		{
																				{"a", scopes::GlobalScope, 0},
																				{"b", scopes::GlobalScope, 1},
																				{"c", scopes::LocalScope, 0},
																				{"d", scopes::LocalScope, 1},
																		}},
																	 {local2,
																		{
																				{"a", scopes::GlobalScope, 0},
																				{"b", scopes::GlobalScope, 1},
																				{"e", scopes::LocalScope, 0},
																				{"f", scopes::LocalScope, 1},
																		}}};
	for (const auto &tt : test_cases) {
		for (const auto &sm : tt.expected) {
			const auto &res = tt.table->resolve(sm.name);
			EXPECT_NE(res, nullptr);

			EXPECT_EQ(res->name, sm.name);
			EXPECT_EQ(res->scope, sm.scope);
			EXPECT_EQ(res->index, sm.index);
		}
	}
}

TEST(VMTest, GlobalLetStatements) {
	std::vector<VMTestcase<int>> test_cases{
			{"let one = 1; one", 1},
			{"let one = 1; let two = 2; one + two", 3},
			{"let one = 1; let two = one + one; one + two", 3},
	};

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, StringExpressions) {
	std::vector<VMTestcase<std::string>> test_cases{
			{"\"lups\"", "lups"},
			{"\"lu\" + \"ps\"", "lups"},
			{"\"lu\" + \"ps\" + \"programming\"", "lupsprogramming"},
	};

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, ArrayLiterals) {
	std::vector<CompilerTestcase<int>> test_cases{
			{"[]",
			 {},
			 {{
					 code::make(code::OpArray, {0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"[1, 2, 3]",
			 {1, 2, 3},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpConstant, {2}),
					 code::make(code::OpArray, {3}),
					 code::make(code::OpPop, {}),
			 }}},
			{"[1 + 2, 3 - 4, 5 * 6]",
			 std::vector<int>{1, 2, 3, 4, 5, 6},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpAdd, {}),
					 code::make(code::OpConstant, {2}),
					 code::make(code::OpConstant, {3}),
					 code::make(code::OpSub, {}),
					 code::make(code::OpConstant, {4}),
					 code::make(code::OpConstant, {5}),
					 code::make(code::OpMul, {}),
					 code::make(code::OpArray, {3}),
					 code::make(code::OpPop, {}),
			 }}}};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, ArrayLiterals) {
	std::vector<VMTestcase<std::vector<int>>> test_cases{
			{"[]", {}},
			{"[1, 2, 3]", {1, 2, 3}},
			{"[1 + 2, 3 * 4, 5 + 6]", {3, 12, 11}}};
	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, HashLiterals) {
	std::vector<CompilerTestcase<int>> test_cases{
			{"{}",
			 {},
			 {{
					 code::make(code::OpHash, {0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"{1: 2, 3: 4, 5: 6}",
			 {1, 2, 3, 4, 5, 6},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpConstant, {2}),
					 code::make(code::OpConstant, {3}),
					 code::make(code::OpConstant, {4}),
					 code::make(code::OpConstant, {5}),
					 code::make(code::OpHash, {6}),
					 code::make(code::OpPop, {}),
			 }}},
			{"{1: 2 + 3, 4: 5 * 6}",
			 {1, 2, 3, 4, 5, 6},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpConstant, {2}),
					 code::make(code::OpAdd, {}),
					 code::make(code::OpConstant, {3}),
					 code::make(code::OpConstant, {4}),
					 code::make(code::OpConstant, {5}),
					 code::make(code::OpMul, {}),
					 code::make(code::OpHash, {4}),
					 code::make(code::OpPop, {}),
			 }}}};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, CIndexExpression) {
	std::vector<CompilerTestcase<int>> test_cases{
			{"[1, 2, 3][1 + 1]",
			 {1, 2, 3, 1, 1},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpConstant, {2}),
					 code::make(code::OpArray, {3}),
					 code::make(code::OpConstant, {3}),
					 code::make(code::OpConstant, {4}),
					 code::make(code::OpAdd, {}),
					 code::make(code::OpIndex, {}),
					 code::make(code::OpPop, {}),
			 }}},
			{"{1: 2}[2 - 1]",
			 {1, 2, 2, 1},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpHash, {2}),
					 code::make(code::OpConstant, {2}),
					 code::make(code::OpConstant, {3}),
					 code::make(code::OpSub, {}),
					 code::make(code::OpIndex, {}),
					 code::make(code::OpPop, {}),
			 }}},
	};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, HashLiterals) {
	std::vector<VMTestcase<std::map<int, int>>> test_cases{
			{
					"{}",
			},
			{"{1: 2, 2: 3}",
			 {
					 {(new Integer(1))->hash_key().value, 2},
					 {(new Integer(2))->hash_key().value, 3},
			 }},
			{"{1 + 1: 2 * 2, 3 + 3: 4 * 4}",
			 {
					 {(new Integer(2))->hash_key().value, 4},
					 {(new Integer(6))->hash_key().value, 16},
			 }}};

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, IndexExpressions) {
	std::vector<VMTestcase<int>> test_cases{
			{"[1, 2, 3][1]", 2},
			{"[1, 2, 3][0 + 2]", 3},
			{"[[1, 1, 1]][0][0]", 1},
			// -1 means null
			{"[][0]", -1},
			{"[1, 2, 3][99]", -1},
			{"[1][-1]", -1},
			{"{1: 1, 2: 2}[1]", 1},
			{"{1: 1, 2: 2}[2]", 2},
			{"{1: 1}[0]", -1},
			{"{}[0]", -1},
	};

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, CompilerScopes) {
	auto compiler = new Compiler();
	EXPECT_EQ(compiler->scope_index, 0);

	auto global_symbol_table = compiler->m_symbol_table;
	compiler->emit(code::OpMul);

	compiler->enter_scope();
	EXPECT_EQ(compiler->scope_index, 1);

	compiler->emit(code::OpSub);
	EXPECT_EQ(compiler->scopes[compiler->scope_index].instructions.size(), 1);

	auto last = compiler->scopes[compiler->scope_index].last_inst;
	EXPECT_EQ(last.op, code::OpSub);

	EXPECT_EQ(compiler->m_symbol_table->outer_, global_symbol_table);

	compiler->leave_scope();
	EXPECT_EQ(compiler->scope_index, 0);

	EXPECT_EQ(compiler->m_symbol_table, global_symbol_table);
	EXPECT_EQ(compiler->m_symbol_table->outer_, nullptr);

	compiler->emit(code::OpAdd);

	EXPECT_EQ(compiler->scopes[compiler->scope_index].instructions.size(), 2);

	last = compiler->scopes[compiler->scope_index].last_inst;
	EXPECT_EQ(last.op, code::OpAdd);

	auto previous = compiler->scopes[compiler->scope_index].prev_inst;
	EXPECT_EQ(previous.op, code::OpMul);
}

TEST(CompilerTest, Functions) {
	std::vector<code::Instructions> func_insts1{{
			code::make(code::OpConstant, {0}),
			code::make(code::OpConstant, {1}),
			code::make(code::OpAdd, {}),
			code::make(code::OpReturnValue, {}),
	}};

	std::vector<code::Instructions> func_insts2{{
			code::make(code::OpConstant, {0}),
			code::make(code::OpReturnValue, {}),
	}};

	std::vector<code::Instructions> func_insts3{{
			code::make(code::OpReturn, {}),
	}};

	std::vector<CompilerTestcase<Object *>> test_cases{
			{"func() { return 5 + 10 }",
			 {new Integer(5), new Integer(10),
				new CompiledFunction(concat_instructions(func_insts1))},
			 {{
					 code::make(code::OpClosure, {2, 0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"func() { 24 }();",
			 {new Integer(24),
				new CompiledFunction(concat_instructions(func_insts2))},
			 {{
					 code::make(code::OpClosure, {1, 0}),
					 code::make(code::OpCall, {0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"let noArg = func() { 24 };"
			 "noArg();",
			 {new Integer(24),
				new CompiledFunction(concat_instructions(func_insts2))},
			 {{
					 code::make(code::OpClosure, {1, 0}),
					 code::make(code::OpSetGlobal, {0}),
					 code::make(code::OpGetGlobal, {0}),
					 code::make(code::OpCall, {0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"func () { }",
			 {new CompiledFunction(concat_instructions(func_insts3))},
			 {{
					 code::make(code::OpClosure, {0, 0}),
					 code::make(code::OpPop, {}),
			 }}},
	};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, Callingfunctions) {
	std::vector<VMTestcase<int>> tests{
			{
					"let fivePlusTen = func() { 5 + 10; };"
					"fivePlusTen()",
					15,
			},
			{
					"let one = func() { 1; };"
					"let two = func() { 2; };"
					"one() + two()",
					3,
			},
			{"let a = func() { 1 };"
			 "let b = func() { a() + 1};"
			 "let c = func() { b() + 1};"
			 "c();",
			 3},
			{"let earlyExit = func() { return 99; 100; };"
			 "earlyExit();",
			 99},
			{"let earlyExit = func() { return 99; return 100; };"
			 "earlyExit();",
			 99},
			{// -1 is null
			 "let noReturn = func() { };"
			 "noReturn();",
			 -1},
			{"let noReturn = func() { };"
			 "let noReturnTwo = func() { noReturn(); };"
			 "noReturn();"
			 "noReturnTwo();",
			 -1},
			{"let returnsOne = func() { 1; };"
			 "let returnsOneReturner = func() { returnsOne; };"
			 "returnsOneReturner()();",
			 1}};

	auto err = run_vm_tests(tests);
	EXPECT_EQ(err, "") << err;
}

TEST(CompilerTest, LetStatementScopes) {
	std::vector<code::Instructions> func_insts1{{
			code::make(code::OpGetGlobal, {0}),
			code::make(code::OpReturnValue, {}),
	}};

	std::vector<code::Instructions> func_insts2{{
			code::make(code::OpConstant, {0}),
			code::make(code::OpSetLocal, {0}),
			code::make(code::OpGetLocal, {0}),
			code::make(code::OpReturnValue, {}),
	}};

	std::vector<code::Instructions> func_insts3{{
			code::make(code::OpConstant, {0}),
			code::make(code::OpSetLocal, {0}),
			code::make(code::OpConstant, {1}),
			code::make(code::OpSetLocal, {1}),
			code::make(code::OpGetLocal, {0}),
			code::make(code::OpGetLocal, {1}),
			code::make(code::OpAdd, {}),
			code::make(code::OpReturnValue, {}),
	}};

	std::vector<code::Instructions> func_insts4{{
			code::make(code::OpGetLocal, {0}),
			code::make(code::OpReturnValue, {}),
	}};

	std::vector<code::Instructions> func_insts5{{
			code::make(code::OpGetLocal, {0}),
			code::make(code::OpPop, {}),
			code::make(code::OpGetLocal, {1}),
			code::make(code::OpPop, {}),
			code::make(code::OpGetLocal, {2}),
			code::make(code::OpReturnValue, {}),
	}};

	std::vector<CompilerTestcase<Object *>> test_cases{
			{"let num = 55;"
			 "func() { num }",
			 {new Integer(55),
				new CompiledFunction(concat_instructions(func_insts1))},
			 {{
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpSetGlobal, {0}),
					 code::make(code::OpClosure, {1, 0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"func() { "
			 "   let num = 55;"
			 "   num"
			 "}",
			 {new Integer(55),
				new CompiledFunction(concat_instructions(func_insts2))},
			 {{
					 code::make(code::OpClosure, {1, 0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"func() { "
			 "   let a = 55;"
			 "   let b = 77;"
			 "   a + b"
			 "}",
			 {new Integer(55), new Integer(77),
				new CompiledFunction(concat_instructions(func_insts3))},
			 {{
					 code::make(code::OpClosure, {2, 0}),
					 code::make(code::OpPop, {}),
			 }}},
			{"let oneArg = func(a) { a };"
			 "oneArg(24);",
			 {new CompiledFunction(concat_instructions(func_insts4)),
				new Integer(24)},
			 {{
					 code::make(code::OpClosure, {0, 0}),
					 code::make(code::OpSetGlobal, {0}),
					 code::make(code::OpGetGlobal, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpCall, {1}),
					 code::make(code::OpPop, {}),
			 }}},
			{"let manyArg = func(a, b, c) { a; b; c };"
			 "manyArg(24, 25, 26);",
			 {new CompiledFunction(concat_instructions(func_insts5)), new Integer(24),
				new Integer(25), new Integer(26)},
			 {{
					 code::make(code::OpClosure, {0, 0}),
					 code::make(code::OpSetGlobal, {0}),
					 code::make(code::OpGetGlobal, {0}),
					 code::make(code::OpConstant, {1}),
					 code::make(code::OpConstant, {2}),
					 code::make(code::OpConstant, {3}),
					 code::make(code::OpCall, {3}),
					 code::make(code::OpPop, {}),
			 }}}};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, CallingFunctionsWithBindings) {
	std::vector<VMTestcase<int>> test_cases{
			{"let one = func() { let one = 1; one };"
			 "one();",
			 1},
			{"let oneAndTwo = func() { let one = 1; let two = 2; one + two; };"
			 "oneAndTwo();",
			 3},
			{"let oneAndTwo = func() { let one = 1; let two = 2; one + two; };"
			 "let threeAndFour = func() { let three = 3; let four = 4; three + four; "
			 "};"
			 "oneAndTwo() + threeAndFour();",
			 10},
			{"let firstFoobar = func() { let foobar = 50; foobar; };"
			 "let secondFoobar = func() { let foobar = 100; foobar; };"
			 "firstFoobar() + secondFoobar();",
			 150},
			{"let globalSeed = 50;"
			 "let minusOne = func() {"
			 "    let num = 1;"
			 "    globalSeed - num;"
			 "}"
			 "let minusTwo = func() {"
			 "    let num = 2;"
			 "    globalSeed - num;"
			 "}"
			 "minusOne() + minusTwo();",
			 97}};

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, CallingFunctionsWithArguments) {
	std::vector<VMTestcase<int>> test_cases{
			{"let identity = func(a) { a; }"
			 "identity(4);",
			 4},
			{"let sum = func(a, b) { a + b; };"
			 "sum(1, 2);",
			 3},
			{"let sum = func(a, b) { let c = a + b; c;}; sum(1, 2);", 3},
			{
					"let sum = func(a, b) { let c = a + b; c; }; sum(1, 2) + sum(3, 4)",
					10,
			}};

	auto err = run_vm_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(VMTest, WrongParameterCountReturnsError) {
	std::vector<std::string> inputs{{"func() { 1; }(1);"},
																	{"func(a) { a; }();"},
																	{"func(a, b) { a + b; }(1);"}};

	for (const auto &tt : inputs) {
		auto program = parse_compiler_program_helper(tt);
		auto comp = new Compiler();
		auto status = comp->compile(program.get());

		// The compiler does return successful since handling the amount of
		// parameters isn't the compiler's job.
		EXPECT_EQ(status, 0);

		auto vm = new VM(comp->bytecode());
		auto vm_status = vm->run();
		EXPECT_NE(vm_status, 0)
				<< "Virtual machine run was successful even though it shouldn't be";
	}
}

TEST(CompilerTest, BuiltinFunctions) {
	std::vector<code::Instructions> func_inst1{
			{code::make(code::OpGetBuiltin, {0}), code::make(code::OpArray, {0}),
			 code::make(code::OpCall, {1}), code::make(code::OpReturnValue, {})}};

	std::vector<CompilerTestcase<Object *>> test_cases{
			{"len([]);"
			 "push([], 1);",
			 {new Integer(1)},
			 {{
					 code::make(code::OpGetBuiltin, {0}),
					 code::make(code::OpArray, {0}),
					 code::make(code::OpCall, {1}),
					 code::make(code::OpPop, {}),
					 code::make(code::OpGetBuiltin, {5}),
					 code::make(code::OpArray, {0}),
					 code::make(code::OpConstant, {0}),
					 code::make(code::OpCall, {2}),
					 code::make(code::OpPop, {}),
			 }}},
			{"func() { len([]) }",
			 {new CompiledFunction(concat_instructions(func_inst1))},
			 {{
					 code::make(code::OpClosure, {0, 0}),
					 code::make(code::OpPop, {}),
			 }}}};

	auto err = run_compiler_tests(test_cases);
	EXPECT_EQ(err, "") << err;
}

TEST(SymbolTableTest, DefineResolveBuiltins) {
	auto global = new SymbolTable();
	auto local1 = new SymbolTable(global);
	auto local2 = new SymbolTable(local1);

	std::vector<Symbol> expected{
		{"a", scopes::BuiltinScope, 0},
		{"b", scopes::BuiltinScope, 1},
		{"c", scopes::BuiltinScope, 2},
		{"d", scopes::BuiltinScope, 3},
	};

	for (const auto &sm : expected)
		global->define_builtin(sm.index, sm.name);

	std::vector<SymbolTable*> tables {global, local1, local2};
	for (const auto &tb : tables) {
		for (const auto &sm : expected) {
			const auto &res = tb->resolve(sm.name);
			EXPECT_NE(res, nullptr);

			EXPECT_EQ(res->name, sm.name);
			EXPECT_EQ(res->scope, sm.scope);
			EXPECT_EQ(res->index, sm.index);
		}
	}
}

TEST(VMTest, BuiltinFunctions) {
	// -2 means that it should return an error object
	std::vector<VMTestcase<int>> test_cases {
		{"len(\"\")", 0},
		{"len(\"four\")", 4},
		{"len(\"hello world\")", 11},
		{"len(1)", -2},
		{"len(\"one\", \"two\")", -2},
		{"len([1, 2, 3])", 3},

		{"println(\"hello\", \"world\")", -1},

		{"first([1, 2, 3])", 1},
		{"first([])", -1},
		{"first(1)", -2},

		{"last([1, 2, 3])", 3},
		{"last([])", -1},
		{"last(1)", -2},

		{"tail([])", -1},
		{"push(1, 1)", -2},
	};
}
