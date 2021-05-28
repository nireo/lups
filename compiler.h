#ifndef LUPS_COMPILER_H
#define LUPS_COMPILER_H

#include "code.h"
#include "object.h"

struct Bytecode {
	code::Instructions instructions;
	std::vector<Object *> constants;
};

class Compiler {
public:
	Compiler() {
		m_instructions = code::Instructions();
		m_constants = std::vector<Object *>();
	}

	// int is the statuscode
	int compile(Node *node);


	int add_constant(Object *obj);
	int emit(code::Opcode op, std::vector<int> operands);
	int add_instruction(std::vector<char> inst);

	Bytecode *bytecode() {
		return new Bytecode{m_instructions, m_constants};
	}

private:
	code::Instructions m_instructions;
	std::vector<Object *> m_constants;
};

#endif
