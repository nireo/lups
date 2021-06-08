#ifndef LUPS_VM_H
#define LUPS_VM_H

#include "code.h"
#include "compiler.h"
#include <array>
#include <memory>
static constexpr int StackSize = 2048;
static constexpr int GlobalsSize = 65536;
static constexpr int MaxFrames = 1024;

class Frame {
public:
	Frame(code::Instructions inst) {
		func_ = std::make_unique<CompiledFunction>(inst);
		ip_ = -1;
		base_pointer_ = 0;
	}

	Frame(code::Instructions inst, int base_pointer) {
		func_ = std::make_unique<CompiledFunction>(inst);
		ip_ = -1;
		base_pointer_ = base_pointer;
	}

	code::Instructions &instructions() { return func_->m_instructions; }

	int ip_;
	int base_pointer_;
	std::unique_ptr<CompiledFunction> func_;
};

class VM {
public:
	VM(Bytecode *bytecode);
	Object *stack_top();
	int run();
	int push(Object *obj);
	Object *pop();
	Object *last_popped_stack_elem();

	// binary operations
	int execute_binary_operation(code::Opcode op);
	int execute_binary_integer_operation(code::Opcode op, Object *left, Object *right);
	int execute_binary_string_operation(code::Opcode op, Object *left, Object *right);

	// comparisons
	int execute_comparison(code::Opcode op);
	int execute_integer_comparison(code::Opcode op, Object *left, Object *right);

	// prefix expressions
	int execute_bang_operator();
	int execute_minus_operator();

	// index expressions
	int execute_index_expression(Object *left, Object *index);
	int execute_array_index(Object *left, Object *index);
	int execute_hash_index(Object *left, Object *index);

	int call_function(int num_args);

	Object *build_array(int start_index, int end_index);
	Object *build_hash(int start_index, int end_index);

	// frame functions
	Frame& current_frame() { return *frames_[frames_index_-1]; }

	void push_frame(std::unique_ptr<Frame> frame) {
		frames_[frames_index_] = std::move(frame);
		++frames_index_;
	}

	Frame& pop_frame() {
		--frames_index_;
		return *frames_[frames_index_];
	}

private:
	int m_sp;
	std::vector<Object *> m_constants;
	std::vector<Object *> m_stack;
	std::vector<Object *> m_globals;

	std::array<std::unique_ptr<Frame>, MaxFrames> frames_;
	int frames_index_;
};

#endif
