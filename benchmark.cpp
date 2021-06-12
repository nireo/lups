#include "ast.h"
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include <iostream>
#include <memory>
#include <string>
#include <sys/time.h>

typedef unsigned long long timestamp_t;

static timestamp_t get_timestamp() {
	struct timeval now;
	gettimeofday(&now, NULL);
	return now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

std::unique_ptr<Program> parse_compiler_program_helper(std::string input) {
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	return parser.parse_program();
}

static const std::string input = "let fib = func(n) {"
																 "    if (n == 0) {"
																 "        return 0;"
																 "    } else {"
																 "        if (n == 1) {"
																 "            return 1;"
																 "        } else {"
																 "            return fib(n-1) + fib(n-2);"
																 "        }"
																 "    }"
																 "};"
																 "fib(25);";

int main() {
	std::string engine;
	std::cout << "which engine (vm|eval): ";
	std::cin >> engine;

	auto program = parse_compiler_program_helper(input);

	if (engine == "vm") {
		auto comp = new Compiler();
		auto status = comp->compile(program.get());
		if (status.has_value()) {
			std::cout << "compilation unsuccessful";
			return -1;
		}

		auto vm = new VM(comp->bytecode());

		timestamp_t t0 = get_timestamp();

		auto vm_status = vm->run();
		timestamp_t t1 = get_timestamp();
		if (vm_status != 0) {
			std::cout << "running unsuccessful";
			return -1;
		}

		auto stack_elem = vm->last_popped_stack_elem();
		if (stack_elem == nullptr) {
			std::cout << "running unsuccessful";
			return -1;
		}
		double secs = (t1 - t0) / 1000000.0L;
		std::cout << "the vm took: " << secs << '\n';
		std::cout << "result is: " << stack_elem->Inspect() << '\n';

		return 0;
	}
}
