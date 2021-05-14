#include "lexer.h"
#include "token.h"
#include <cctype>

Lexer::Lexer(const std::string &input) {
	m_input = input;

	read_char();

	m_ch = 0;
	m_read_pos = 0;
	m_pos = 0;
}

void Lexer::read_char() {
	if (m_read_pos >= (int)m_input.length())
		m_ch = 0;
	else
		m_ch = m_input[m_read_pos];

	m_pos = m_read_pos;
	++m_read_pos;
}

Token new_token(TokenType type, char ch) {
	return Token{type, std::string(1, ch)};
}

std::string lookup_ident(const std::string &str) {
	if (tokentypes::KEYWORDS.count(str) > 0) {
		return tokentypes::KEYWORDS.at(str);
	}
	return str;
}

Token Lexer::next_token() {
	Token tok;

	switch (m_ch) {
	case '=':
		tok = new_token(tokentypes::ASSIGN, m_ch);
	case ';':
		tok = new_token(tokentypes::SEMICOLON, m_ch);
	case '(':
		tok = new_token(tokentypes::LPAREN, m_ch);
	case ')':
		tok = new_token(tokentypes::RPAREN, m_ch);
	case ',':
		tok = new_token(tokentypes::COMMA, m_ch);
	case '+':
		tok = new_token(tokentypes::PLUS, m_ch);
	case '{':
		tok = new_token(tokentypes::LBRACE, m_ch);
	case '}':
		tok = new_token(tokentypes::RBRACE, m_ch);
	case 0:
		tok.literal = "";
		tok.type = tokentypes::EOFF;
	default:
		if (std::isalpha(m_ch)) {
			tok.literal = read_ident();
			tok.type = lookup_ident(tok.literal);
			return tok;
		} else {
			tok = new_token(tokentypes::ILLEGAL, m_ch);
		}
	}
	read_char();

	return tok;
}

std::string Lexer::read_ident() {
	int start_pos = m_pos;
	while (std::isalpha(m_ch)) {
		read_char();
	}

	return std::string(m_input.begin()+start_pos, m_input.begin()+m_pos);
}
