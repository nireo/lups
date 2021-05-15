#ifndef LUPS_PARSER_H
#define LUPS_PARSER_H

#include "lexer.h"
#include "token.h"
#include <memory>
#include "ast.h"

class Parser {
public:
	Parser(std::unique_ptr<Lexer> lx);
	std::unique_ptr<Program> parse_program();

private:
	std::unique_ptr<Lexer> lx;

	Token current;
	Token peek;
};

#endif
