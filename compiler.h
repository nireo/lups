#ifndef LUPS_COMPILER_H
#define LUPS_COMPILER_H

#include "code.h"
#include "object.h"

struct Bytecode {
	code::Instructions instructions;
	std::vector<Object *> constants;
};

struct EmittedInstruction {
	code::Opcode op;
	int pos;
};

class Compiler {
public:
	Compiler() {
		m_instructions = code::Instructions();
		m_constants = std::vector<Object *>();
		last_inst = nullptr;
		prev_inst = nullptr;
	}

	// int is the statuscode
	int compile(Node *node);


	int add_constant(Object *obj);
	int emit(code::Opcode op, std::vector<int> operands);
	int emit(code::Opcode op);
	int add_instruction(std::vector<char> inst);
	void set_last_instruction(code::Opcode, int pos);

	bool is_last_inst_pop();
	void remove_last_pop();
	void replace_instructions(int pos, code::Instructions new_inst);
	void change_operand(int op_pos, int operand);

	Bytecode *bytecode() {
		return new Bytecode{m_instructions, m_constants};
	}

private:
	code::Instructions m_instructions;
	std::vector<Object *> m_constants;

	EmittedInstruction *last_inst;
	EmittedInstruction *prev_inst;
};

#endif
