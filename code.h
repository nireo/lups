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
	OpGetBuiltin,
	OpClosure,
	OpGetFree,
};

struct Definition {
	std::string name;
	std::vector<int> operand_widths;
};

static const std::unordered_map<Opcode, Definition *> definitions{
		{Opcodes::OpConstant, new Definition{"OpConstant", std::vector<int>{2}}},
		{Opcodes::OpAdd, new Definition{"OpAdd", {}}},
		{Opcodes::OpPop, new Definition{"OpPop", {}}},
		{Opcodes::OpSub, new Definition{"OpSub", {}}},
		{Opcodes::OpMul, new Definition{"OpMul", {}}},
		{Opcodes::OpDiv, new Definition{"OpDiv", {}}},
		{Opcodes::OpTrue, new Definition{"OpTrue", {}}},
		{Opcodes::OpFalse, new Definition{"OpFalse", {}}},
		{Opcodes::OpEqual, new Definition{"OpEqual", {}}},
		{Opcodes::OpNotEqual, new Definition{"OpNotEqual", {}}},
		{Opcodes::OpGreaterThan,
		 new Definition{"OpGreaterThan", {}}},
		{Opcodes::OpMinus, new Definition{"OpMinus", {}}},
		{Opcodes::OpBang, new Definition{"OpBang", {}}},
		{Opcodes::OpJumpNotTruthy, new Definition{"OpJumpNotTruthy", {2}}},
		{Opcodes::OpJump, new Definition{"OpJump", {2}}},
		{Opcodes::OpNull, new Definition{"OpNull", {}}},
		{Opcodes::OpGetGlobal, new Definition{"OpGetGlobal", {2}}},
		{Opcodes::OpSetGlobal, new Definition{"OpSetGlobal", {2}}},
		{Opcodes::OpArray, new Definition{"OpArray", {2}}},
		{Opcodes::OpHash, new Definition{"OpHash", {2}}},
		{Opcodes::OpIndex, new Definition{"OpIndex", {}}},
		{Opcodes::OpCall, new Definition{"OpCall", {1}}},
		{Opcodes::OpReturnValue, new Definition{"OpReturnValue", {}}},
		{Opcodes::OpReturn, new Definition{"OpReturn", {}}},
		{Opcodes::OpGetLocal, new Definition{"OpGetLocal", {1}}},
		{Opcodes::OpSetLocal, new Definition{"OpSetLocal", {1}}},
		{Opcodes::OpGetBuiltin, new Definition{"OpGetBuiltin", {1}}},
		{Opcodes::OpClosure, new Definition{"OpClosure", {2, 1}}},
		{Opcodes::OpGetFree, new Definition{"OpGetFree", {1}}}};

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
