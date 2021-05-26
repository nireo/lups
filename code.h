#ifndef LUPS_CODE_H
#define LUPS_CODE_H

#include <unordered_map>
#include <vector>
#include <string>

namespace code {
typedef std::vector<char> Instructions;
typedef char Opcode;

enum Opcodes  {
	OpConstant
};

struct Definition {
	std::string name;
	std::vector<int> operand_widths;
};

const std::unordered_map<Opcode, Definition*> definitions {
	{
		Opcodes::OpConstant, new Definition{
			"OpConstant", std::vector<int> { 2 }
		}
	}
};

 Definition *look_up(char op_code);
 std::vector<char> make(Opcode op, std::vector<int> operands);
}


#endif
