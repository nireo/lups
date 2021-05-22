#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <iostream>
#include <memory>
#include <string>

Object *eval_test(const std::string &input) {
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));
	auto program = parser.parse_program();
	return eval::Eval(program.get(), new Environment());
}

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
	}
}
