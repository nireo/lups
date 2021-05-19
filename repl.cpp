#include "lexer.h"
#include "token.h"
#include "parser.h"
#include <iostream>
#include <string>
#include <memory>

int main() {
	std::cout << "welcome to the lupslang repl!" << '\n';

	for (;;) {
		std::string input;
		std::cout << "input:" << '\n';
		std::getline(std::cin, input);

		auto lexer = Lexer(input);
		auto parser = Parser(std::make_unique<Lexer>(lexer));
		auto program = parser.parse_program();

		if (parser.errors().size() != 0) {
			std::cout << "there were " << parser.errors().size() << " errors." << '\n';
			continue;
		}

		std::cout << program->String() << '\n';
	}

	return 0;
}
