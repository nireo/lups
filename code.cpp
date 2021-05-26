#include "code.h"

using namespace code;
Definition *code::look_up(char op_code) {
	auto def = definitions.find(op_code);
	if (def == definitions.end())
		return nullptr;

	return definitions.at(op_code);
}

std::vector<char> code::make(Opcode op, std::vector<int> operands) {
	auto def = definitions.at(op);
	if (def == nullptr)
		return std::vector<char>();

	int instruction_size = 1;
	for (const auto& w : def->operand_widths)
		instruction_size += w;
	int offset = 1;
	for (int i = 0; i < (int)operands.size(); ++i) {
		auto width = def->operand_widths[i];
		if (width == 2) {
		}
	}
}
