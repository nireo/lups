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

private:
	int m_sp;
	code::Instructions m_instructions;
	std::vector<Object *> m_constants;
	std::vector<Object *> m_stack;
};

#endif
