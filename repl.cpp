#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <iostream>
#include <memory>
#include <string>

int main() {
	std::cout << "welcome to the lupslang repl!" << '\n';

	// for (;;) {
	//	std::string input;
	//	std::cout << "input:" << '\n';
	//	std::getline(std::cin, input);

	//	if (input == ".quit")
	//		break;

	//	auto lexer = Lexer(input);
	//	auto parser = Parser(std::make_unique<Lexer>(lexer));
	//	auto program = parser.parse_program();

	//	if (parser.errors().size() != 0) {
	//		std::cout << "there were " << parser.errors().size() << "
	// errors."
	//							<< '\n';
	//		continue;
	//	}


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
				 "unknown operation: BOOLEAN + BOOLEAN"}};

	for (auto &tc : test_cases) {
		auto lexer = Lexer(tc.input);
		auto parser = Parser(std::make_unique<Lexer>(lexer));
		auto program = parser.parse_program();
		auto obj = eval::Eval(program.get());
		if (obj != nullptr) {
			std::cout << "object is not a nullptr" << '\n';
			std::cout << obj->Type();
		}
	}
}
