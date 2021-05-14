#ifndef LUPS_LEXER_H
#define LUPS_LEXER_H

#include <string>
#include "token.h"

class Lexer {
public:
	Lexer(const std::string &input);
	Token next_token();

private:
	std::string m_input;
	int m_pos;
	char m_ch;
	int m_read_pos;

	std::string read_ident();
	std::string read_number();
	void read_char();
	void skip_whitespace();
};

#endif
