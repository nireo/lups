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
		cl_ = std::make_unique<Closure>(new CompiledFunction(inst));
		ip_ = -1;
		base_pointer_ = 0;
	}

	Frame(code::Instructions inst, int base_pointer) {
		cl_ = std::make_unique<Closure>(new CompiledFunction(inst));
		ip_ = -1;
		base_pointer_ = base_pointer;
	}

	code::Instructions &instructions() noexcept {
		return cl_->func_->m_instructions;
	}
	const Closure &closure() const noexcept { return *cl_; };

	int ip_;
	int base_pointer_;
	std::unique_ptr<Closure> cl_;
};

class VM {
public:
	VM(Bytecode *bytecode);
	Object *stack_top();
	std::optional<std::string> push(Object *obj);
	Object *pop();
	Object *last_popped_stack_elem();

	// The optional contains a possible error message, meaning that std::optional
	// doesn't have a value if an error has occurred.

	std::optional<std::string> run();

	// binary operations
	std::optional<std::string> execute_binary_operation(code::Opcode op);
	std::optional<std::string> execute_binary_integer_operation(code::Opcode op,
																															Object *left,
																															Object *right);
	std::optional<std::string>
	execute_binary_string_operation(code::Opcode op, Object *left, Object *right);

	// comparisons
	std::optional<std::string> execute_comparison(code::Opcode op);
	std::optional<std::string>
	execute_integer_comparison(code::Opcode op, Object *left, Object *right);

	// prefix expressions
	std::optional<std::string> execute_bang_operator();
	std::optional<std::string> execute_minus_operator();

	// index expressions
	std::optional<std::string> execute_index_expression(Object *left,
																											Object *index);
	std::optional<std::string> execute_array_index(Object *left, Object *index);
	std::optional<std::string> execute_hash_index(Object *left, Object *index);

	// functions
	std::optional<std::string> call_closure(Object *cl, int num_args);
	std::optional<std::string> call_builtin(Object *fn, int num_args);
	std::optional<std::string> execute_call(int num_args);
	std::optional<std::string> push_closure(int const_index, int num_free);

	Object *build_array(int start_index, int end_index);
	Object *build_hash(int start_index, int end_index);

	// frame functions
	Frame &current_frame() { return *frames_[frames_index_ - 1]; }

	void push_frame(std::unique_ptr<Frame> frame) {
		frames_[frames_index_] = std::move(frame);
		++frames_index_;
	}

	Frame &pop_frame() {
		--frames_index_;
		return *frames_[frames_index_];
	}

private:
	int sp_;
	std::vector<Object *> constants_;
	std::vector<Object *> stack_;
	std::vector<Object *> globals_;

	std::array<std::unique_ptr<Frame>, MaxFrames> frames_;
	int frames_index_;
};

#endif
