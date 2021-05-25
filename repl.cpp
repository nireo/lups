#include "eval.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"
#include "token.h"
#include <iostream>
#include <memory>
#include <string>

Object *eval_input(const std::string &input, Environment *env) {
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));
	auto program = parser.parse_program();
	return eval::Eval(program.get(), env);
}

int main() {
	std::cout << "welcome to the lupslang repl!" << '\n';

	auto env = new Environment();
	for (;;) {
		std::string input;
		std::cout << "> ";
		std::getline(std::cin, input);

		if (input == ".quit")
			break;

		auto obj = eval_input(input, env);
		if (obj != nullptr) {
			std::cout << obj->Inspect() << '\n';
		}

		for (auto &v : env->m_store) {
			std::cout << v.first << ": " << v.second->Inspect() << '\n';
		}
	}
}
