#ifndef LUPS_CODE_H
#define LUPS_CODE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace code {
typedef std::vector<char> Instructions;
typedef char Opcode;

enum Opcodes {
	OpConstant,
	OpAdd,
	OpPop,
	OpSub,
	OpMul,
	OpDiv,
	OpTrue,
	OpFalse,
	OpEqual,
	OpNotEqual,
	OpGreaterThan,
	OpMinus,
	OpBang,
	OpJumpNotTruthy,
	OpJump,
	OpNull,
	OpGetGlobal,
	OpSetGlobal,
	OpArray,
	OpHash,
	OpIndex,
	OpCall,
	OpReturnValue,
	OpReturn,
	OpGetLocal,
	OpSetLocal,
	OpGetBuiltin
};

struct Definition {
	std::string name;
	std::vector<int> operand_widths;
};

static const std::unordered_map<Opcode, Definition *> definitions{
		{Opcodes::OpConstant, new Definition{"OpConstant", std::vector<int>{2}}},
		{Opcodes::OpAdd, new Definition{"OpAdd", std::vector<int>{}}},
		{Opcodes::OpPop, new Definition{"OpPop", std::vector<int>{}}},
		{Opcodes::OpSub, new Definition{"OpSub", std::vector<int>{}}},
		{Opcodes::OpMul, new Definition{"OpMul", std::vector<int>{}}},
		{Opcodes::OpDiv, new Definition{"OpDiv", std::vector<int>{}}},
		{Opcodes::OpTrue, new Definition{"OpTrue", std::vector<int>{}}},
		{Opcodes::OpFalse, new Definition{"OpFalse", std::vector<int>{}}},
		{Opcodes::OpEqual, new Definition{"OpEqual", std::vector<int>{}}},
		{Opcodes::OpNotEqual, new Definition{"OpNotEqual", std::vector<int>{}}},
		{Opcodes::OpGreaterThan,
		 new Definition{"OpGreaterThan", std::vector<int>{}}},
		{Opcodes::OpMinus, new Definition{"OpMinus", std::vector<int>{}}},
		{Opcodes::OpBang, new Definition{"OpBang", std::vector<int>{}}},
		{Opcodes::OpJumpNotTruthy, new Definition{"OpJumpNotTruthy", std::vector<int>{2}}},
		{Opcodes::OpJump, new Definition{"OpJump", std::vector<int>{2}}},
		{Opcodes::OpNull, new Definition{"OpNull", std::vector<int>{}}},
		{Opcodes::OpGetGlobal, new Definition{"OpGetGlobal", std::vector<int>{2}}},
		{Opcodes::OpSetGlobal, new Definition{"OpSetGlobal", std::vector<int>{2}}},
		{Opcodes::OpArray, new Definition{"OpArray", std::vector<int>{2}}},
		{Opcodes::OpHash, new Definition{"OpHash", std::vector<int>{2}}},
		{Opcodes::OpIndex, new Definition{"OpIndex", std::vector<int>{}}},
		{Opcodes::OpCall, new Definition{"OpCall", std::vector<int>{1}}},
		{Opcodes::OpReturnValue, new Definition{"OpReturnValue", std::vector<int>{}}},
		{Opcodes::OpReturn, new Definition{"OpReturn", std::vector<int>{}}},
		{Opcodes::OpGetLocal, new Definition{"OpGetLocal", std::vector<int>{1}}},
		{Opcodes::OpSetLocal, new Definition{"OpSetLocal", std::vector<int>{1}}},
		{Opcodes::OpGetBuiltin, new Definition{"OpGetBuiltin", std::vector<int>{1}}}};

Definition *look_up(char op_code);
std::vector<char> make(Opcode op, std::vector<int> operands);
std::vector<char> encode_uint16(std::uint16_t val);
std::uint16_t decode_uint16(Instructions inst);
std::string fmt_instructions(Definition *def, std::vector<int> operands);

std::string instructions_to_string(Instructions &inst);
std::pair<std::vector<int>, int> read_operands(Definition *def,
																							 Instructions inst);
} // namespace code

#endif
