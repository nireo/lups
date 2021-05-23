#include "lexer.h"
#include "token.h"
#include <cctype>

Lexer::Lexer(const std::string &input) {
	m_input = input;
	m_read_pos = 0;
	read_char();
}

bool is_letter(char ch) {
	return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

bool is_digit(char ch) { return '0' <= ch && ch <= '9'; }

void Lexer::read_char() {
	if (m_read_pos >= (int)m_input.size())
		m_ch = 0;
	else
		m_ch = m_input[m_read_pos];

	m_pos = m_read_pos;
	m_read_pos++;
}

Token new_token(TokenType type, char ch) {
	return Token{type, std::string(1, ch)};
}

TokenType lookup_ident(const std::string &str) {
	auto keyword = tokentypes::KEYWORDS.find(str);
	if (keyword == tokentypes::KEYWORDS.end())
		return tokentypes::IDENT;
	return keyword->second;
}

Token Lexer::next_token() {
	Token tok;

	skip_whitespace();

	switch (m_ch) {
	case '=':
		if (peek_char() == '=') {
			tok = Token{.type=tokentypes::EQ, .literal="=="};
			read_char();
		} else {
			tok = new_token(tokentypes::ASSIGN, m_ch);
		}
		read_char();
		break;
	case ';':
		tok = new_token(tokentypes::SEMICOLON, m_ch);
		read_char();
		break;
	case '(':
		tok = new_token(tokentypes::LPAREN, m_ch);
		read_char();
		break;
	case ')':
		tok = new_token(tokentypes::RPAREN, m_ch);
		read_char();
		break;
	case ',':
		tok = new_token(tokentypes::COMMA, m_ch);
		read_char();
		break;
	case '+':
		tok = new_token(tokentypes::PLUS, m_ch);
		read_char();
		break;
	case '-':
		tok = new_token(tokentypes::MINUS, m_ch);
		read_char();
		break;
	case '!':
		if (peek_char() == '=') {
			tok = Token{.type=tokentypes::NEQ, .literal="!="};
			read_char();
		} else {
			tok = new_token(tokentypes::BANG, m_ch);
		}
		read_char();
		break;
	case '/':
		tok = new_token(tokentypes::SLASH, m_ch);
		read_char();
		break;
	case '*':
		tok = new_token(tokentypes::ASTERISK, m_ch);
		read_char();
		break;
	case '<':
		tok = new_token(tokentypes::LT, m_ch);
		read_char();
		break;
	case '>':
		tok = new_token(tokentypes::GT, m_ch);
		read_char();
		break;
	case '{':
		tok = new_token(tokentypes::LBRACE, m_ch);
		read_char();
		break;
	case '}':
		tok = new_token(tokentypes::RBRACE, m_ch);
		read_char();
		break;
	case '"':
		tok.type = tokentypes::STRING;
		tok.literal = read_string();
		read_char();
		break;
	case 0:
		tok.literal = "";
		tok.type = tokentypes::EOFF;
		read_char();
		break;
	default:
		if (is_letter(m_ch)) {
			tok.literal = read_ident();
			tok.type = lookup_ident(tok.literal);
		} else if (is_digit(m_ch)) {
			tok.type = tokentypes::INT;
			tok.literal = read_number();
		} else {
			tok = new_token(tokentypes::ILLEGAL, m_ch);
			read_char();
		}
		break;
	}

	return tok;
}

std::string Lexer::read_string() {
	int start_pos = m_pos + 1;
	for (;;) {
		read_char();
		if (m_ch == '"' || m_ch == 0) {
			break;
		}
	}

	return m_input.substr(start_pos, m_pos-start_pos);
}

void Lexer::skip_whitespace() {
	while (m_ch == ' ' || m_ch == '\t' || m_ch == '\n' || m_ch == '\r')
		read_char();
}

std::string Lexer::read_ident() {
	int start_pos = m_pos;
	while (is_letter(m_ch)) {
		read_char();
	}

	return m_input.substr(start_pos, m_pos-start_pos);
}

std::string Lexer::read_number() {
	int start_pos = m_pos;
	while (is_digit(m_ch)) {
		read_char();
	}

	return m_input.substr(start_pos, m_pos-start_pos);
}

char Lexer::peek_char() {
	if (m_read_pos >= (int)m_input.size())
		return 0;
	else
		return m_input[m_read_pos];
}
