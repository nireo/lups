#include "eval.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"
#include "token.h"
#include "vm.h"
#include "compiler.h"
#include <iostream>
#include <memory>
#include <string>

Object *eval_input(const std::string &input, Environment *env) {
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));
	auto program = parser.parse_program();
	return eval::Eval(program.get(), env);
}


std::unique_ptr<Program> parse_compiler_program_helper(std::string input) {
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	return parser.parse_program();
}

int main() {
	std::cout << "welcome to the lupslang repl!" << '\n';

	auto env = new Environment();
//	for (;;) {
//		std::string input;
//		std::cout << "> ";
//		std::getline(std::cin, input);

//		if (input == ".quit")
//			break;

//		auto obj = eval_input(input, env);
//		if (obj != nullptr) {
//			std::cout << obj->Inspect() << '\n';
//		}

//		for (auto &v : env->m_store) {
//			std::cout << v.first << ": " << v.second->Inspect() << '\n';
//		}
//	}

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
	};

	for (auto const &tt : test_cases) {
		std::unique_ptr<Program> program = parse_compiler_program_helper(tt.input);
		auto comp = new Compiler();
		auto status = comp->compile(program.get());
		if (status != 0)
			std::cout << "compilation not successful" << '\n';

		auto vm = new VM(comp->bytecode());
		auto vm_status = vm->run();
		if (status != 0)
			std::cout << "vm running was not successful" << '\n';

		auto stack_elem = vm->last_popped_stack_elem();
		if (stack_elem == nullptr) {
			std::cout << "stack element is null" << '\n';
		}
	}
}
