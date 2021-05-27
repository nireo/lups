#include "code.h"
#include <bits/stdint-uintn.h>
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
