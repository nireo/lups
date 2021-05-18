#include "lexer.h"
#include "token.h"
#include <iostream>
#include <string>

int main() {
	std::cout << "welcome to the lupslang repl!" << '\n';

	for (;;) {
		std::string input;
		std::cout << "input:" << '\n';
		std::getline(std::cin, input);

		auto lexer = Lexer(input);
		for (;;) {
			auto token = lexer.next_token();
			if (token.type == tokentypes::EOFF) {
				break;
			}

			std::cout << "type: " << token.type << " | literal: " << token.literal << '\n';
		}
	}

	return 0;
}
