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
	void next_token();
	std::unique_ptr<Lexer> m_lx;

	Token m_current;
	Token m_peek;

	std::unique_ptr<Statement> parse_statement();
	std::unique_ptr<Statement> parse_let_statement();

	bool expect_peek(TokenType tt);
	bool peek_token_is(TokenType tt);
	bool current_token_is(TokenType tt);
};

#endif
