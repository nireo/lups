#ifndef LUPS_VM_H
#define LUPS_VM_H

#include "code.h"
#include "compiler.h"
constexpr int StackSize = 2048;

class VM {
public:
	VM(Bytecode *bytecode);
	Object *stack_top();
	int run();
	int push(Object *obj);
	Object *pop();
	Object *last_popped_stack_elem();

	int execute_binary_operation(code::Opcode op);
	int execute_binary_integer_operation(code::Opcode op, Object *left, Object *right);

	int execute_comparison(code::Opcode op);
	int execute_integer_comparison(code::Opcode op, Object *left, Object *right);

private:
	int m_sp;
	code::Instructions m_instructions;
	std::vector<Object *> m_constants;
	std::vector<Object *> m_stack;
};

#endif
