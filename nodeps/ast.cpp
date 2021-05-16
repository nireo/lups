#include "ast.h"

std::string Program::TokenLiteral() {
	if (statements.size() > 0) {
		return statements[0].get()->TokenLiteral();
	}

	return "";
}

std::string Program::String() {
	std::string res;
	for (int i = 0; i < (int)statements.size(); ++i) {
		res += statements[i]->String() + "\n";
	}

	return res;
}

std::string LetStatement::String() {
	std::string res = TokenLiteral() + " " + name->String() + " = ";

	if (value != nullptr) {
		res += value->String();
	}

	res += ";";
	return res;
}