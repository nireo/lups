#ifndef LUPS_TOKEN_H
#define LUPS_TOKEN_H

#include <string>
#include <unordered_map>

typedef std::string TokenType;
struct Token {
	TokenType type;
	std::string literal;
};

namespace tokentypes {
const TokenType ILLEGAL = "ILLEGAL";
const TokenType EOFF = "EOF";
const TokenType IDENT = "IDENT";
const TokenType INT = "INT";

const TokenType ASSIGN = "=";
const TokenType PLUS = "+";
const TokenType COMMA = ",";
const TokenType SEMICOLON = ";";

const TokenType LPAREN = "(";
const TokenType RPAREN = ")";
const TokenType LBRACE = "{";
const TokenType RBRACE = "}";

const TokenType FUNCTION = "FUNCTION";
const TokenType LET = "LET";

const std::unordered_map<std::string, TokenType> KEYWORDS = {
		{"func", FUNCTION},
		{"let", LET},
};
} // namespace tokentypes

#endif
