#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <iostream>
#include <memory>
#include <string>

bool test_integer_object(Object* obj, int expected) {
	auto res = dynamic_cast<Integer *>(obj);
	if (res == nullptr)
		return false;

	if (res->value != expected)
		return false;

	return true;
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
	//errors."
	//							<< '\n';
	//		continue;
	//	}

	struct Testcase {
		std::string input;
		int expected;
	};

	std::vector<Testcase> test_cases{
			{"5", 5},
			{"10", 10},
	};

	auto lexer = Lexer(test_cases[0].input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));
	auto program = parser.parse_program();
	auto obj = eval::Eval(program.get());

	std::cout << (test_integer_object(obj, 5) ? "works" : "doesn't");

	return 0;
}
