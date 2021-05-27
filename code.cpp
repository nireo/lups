#include "code.h"
#include <bits/stdint-uintn.h>
#include <stdarg.h>
#include <cstdint>

using namespace code;
Definition *code::look_up(char op_code) {
	auto def = definitions.find(op_code);
	if (def == definitions.end())
		return nullptr;

	return definitions.at(op_code);
}

std::vector<char> code::encode_uint16(std::uint16_t val) {
	std::vector<char> big_endian;
	for (int i = 1; i >= 0; i--) {
		big_endian.push_back((val >> (8 * i)) & 0xff);
	}

	return big_endian;
}
std::string string_format(const std::string fmt, ...) {
		int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
		std::string str;
		va_list ap;
		while (1) {     // Maximum two passes on a POSIX system...
				str.resize(size);
				va_start(ap, fmt);
				int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
				va_end(ap);
				if (n > -1 && n < size) {  // Everything worked
						str.resize(n);
						return str;
				}
				if (n > -1)  // Needed size returned
						size = n + 1;   // For null char
				else
						size *= 2;      // Guess at a larger size (OS specific)
		}
		return str;
}

std::uint16_t code::decode_uint16(Instructions inst) {
	std::uint16_t num = (std::uint16_t)inst[1] | (int)inst[0] << 8;
	return num;
}

std::vector<char> code::make(Opcode op, std::vector<int> operands) {
	auto def = definitions.at(op);
	if (def == nullptr)
		return std::vector<char>();

	int instruction_size = 1;
	for (const auto &w : def->operand_widths)
		instruction_size += w;

	std::vector<char> instructions(1, op);

	for (int i = 0; i < (int)operands.size(); ++i) {
		auto width = def->operand_widths[i];
		if (width == 2) {
			auto encoded = code::encode_uint16((std::uint16_t)operands[i]);
			instructions.insert(instructions.end(), encoded.begin(), encoded.end());
		}
	}

	return instructions;
}

std::string code::instructions_to_string(Instructions &instructions) {
	std::string res;

	int i = 0;
	while (i < instructions.size()) {
		auto def = code::look_up(instructions[i]);
		if (def == nullptr) {
			res += "ERROR: definition not found\n";
			continue;
		}

		auto read_res = code::read_operands(def, Instructions(instructions.begin()+i+1, instructions.end()));
		res += string_format("%04d %s\n", i, code::fmt_instructions(def, read_res.first).c_str());
		i += 1 + read_res.second;
	}

	return res;
}

std::pair<std::vector<int>, int> code::read_operands(Definition *def,
																										 code::Instructions inst) {
	std::vector<int> operands(def->operand_widths.size(), 0);

	int offset = 0;
	for (int i = 0; i < (int)def->operand_widths.size(); ++i) {
		auto width = def->operand_widths[i];
		if (width == 2) {
			operands[i] = (int)code::decode_uint16(Instructions(inst.begin()+offset, inst.end()));
		}

		offset += width;
	}

	return std::make_pair(operands, offset);
}

std::string code::fmt_instructions(Definition *def, std::vector<int> operands) {
	auto operand_count = def->operand_widths.size();
	if (operand_count != operands.size())
		return "ERROR operand len does not match defined\n";

	switch (operand_count) {
	case 1:
		return def->name + " " + std::to_string(operands[0]);
		break;
	}

	return "ERROR: unhandled operand count for " + def->name + '\n';
}
