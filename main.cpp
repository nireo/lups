#include "compiler.h"
#include "vm.h"
#include <fstream>
#include <iostream>
#include <streambuf>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

std::unique_ptr<Program> parse_compiler_program_helper(std::string input) {
	auto lexer = Lexer(input);
	auto parser = Parser(std::make_unique<Lexer>(lexer));

	return parser.parse_program();
}

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		std::cout << "you need to provide the wanted filename as an argument."
							<< '\n';
		return EXIT_FAILURE;
	}
	const std::string fname = std::string(argv[1]);

	std::ifstream t(fname);
	std::string str;

	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
						 std::istreambuf_iterator<char>());

	auto program = parse_compiler_program_helper(str);

	auto comp = new Compiler();
	auto status = comp->compile(*program);
	if (status.has_value())
		return EXIT_FAILURE;

	auto vm = new VM(comp->bytecode());
	auto vm_status = vm->run();
	if (vm_status.has_value())
		return EXIT_FAILURE;

	auto stack_elem = vm->last_popped_stack_elem();
	if (stack_elem == nullptr)
		return EXIT_FAILURE;

	std::cout << stack_elem->Inspect() << '\n';

	return EXIT_SUCCESS;
}
