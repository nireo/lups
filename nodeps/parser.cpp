#include "parser.h"
#include "ast.h"
#include "token.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>

template <typename T> T FromString(const std::string &str) {
	std::istringstream ss(str);
	T ret;
	ss >> ret;
	return ret;
}

using std::unique_ptr;

unique_ptr<Program> Parser::parse_program() {
	auto program = std::make_unique<Program>();
	program->statements = std::vector<unique_ptr<Statement>>();

	while (m_current.type != tokentypes::EOFF) {
		auto stmt = parse_statement();
		if (stmt != nullptr) {
			program->statements.push_back(std::move(stmt));
		}
		next_token();
	}

	return program;
}

Parser::Parser(unique_ptr<Lexer> lx) {
	m_lx = std::move(lx);

	m_prefix_parse_fns = std::unordered_map<TokenType, PrefixParseFn>();
	add_prefix_parse(tokentypes::IDENT, &Parser::parse_identifier);
	add_prefix_parse(tokentypes::INT, &Parser::parse_integer_literal);
	add_prefix_parse(tokentypes::BANG, &Parser::parse_prefix_expression);
	add_prefix_parse(tokentypes::MINUS, &Parser::parse_prefix_expression);

	m_infix_parse_fns = std::unordered_map<TokenType, InfixParseFn>();
	add_infix_parse(tokentypes::PLUS, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::MINUS, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::SLASH, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::ASTERISK, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::EQ, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::NEQ, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::LT, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::GT, &Parser::parse_infix_expression);

	next_token();
	next_token();
}

void Parser::next_token() {
	m_current = m_peek;
	m_peek = m_lx->next_token();
}

unique_ptr<Statement> Parser::parse_statement() {
	if (m_current.type == tokentypes::LET) {
		return parse_let_statement();
	} else if (m_current.type == tokentypes::RETURN) {
		return parse_return_statement();
	} else {
		return parse_expression_statement();
	}
}

bool Parser::current_token_is(TokenType tt) { return m_current.type == tt; }

bool Parser::peek_token_is(TokenType tt) { return m_peek.type == tt; }

bool Parser::expect_peek(TokenType tt) {
	if (peek_token_is(tt)) {
		next_token();
		return true;
	}

	peek_error(tt);
	return false;
}

unique_ptr<Statement> Parser::parse_let_statement() {
	auto letstmt = std::make_unique<LetStatement>();
	letstmt->token = m_current;

	if (!expect_peek(tokentypes::IDENT)) {
		return nullptr;
	}

	auto ident = std::make_unique<Identifier>();
	ident->token = m_current;
	ident->value = m_current.literal;
	letstmt->name = std::move(ident);

	if (!expect_peek(tokentypes::ASSIGN)) {
		return nullptr;
	}

	while (!current_token_is(tokentypes::SEMICOLON)) {
		next_token();
	}

	return letstmt;
}

unique_ptr<Statement> Parser::parse_return_statement() {
	auto returnstmt = std::make_unique<ReturnStatement>();
	returnstmt->token = m_current;

	while (!current_token_is(tokentypes::SEMICOLON)) {
		next_token();
	}

	return returnstmt;
}

unique_ptr<Statement> Parser::parse_expression_statement() {
	auto stmt = std::make_unique<ExpressionStatement>();
	stmt->token = m_current;

	stmt->expression = parse_expression(LOWEST);
	if (peek_token_is(tokentypes::SEMICOLON)) {
		next_token();
	}

	return stmt;
}

unique_ptr<Expression> Parser::parse_expression(Precedence prec) {
	if (m_prefix_parse_fns.find(m_current.type) == m_prefix_parse_fns.end()) {
		return nullptr;
	}

	auto fn = m_prefix_parse_fns[m_current.type];
	auto left = (this->*fn)();

	while (!peek_token_is(tokentypes::SEMICOLON) && prec <= peek_precedence()) {
		auto infix = m_infix_parse_fns[m_peek.type];
		if (infix == nullptr)
			return left;
		next_token();
		left = (this->*infix)(std::move(left));
	}

	return left;
}

unique_ptr<Expression> Parser::parse_identifier() {
	auto identifier = std::make_unique<Identifier>();
	identifier->token = m_current;
	identifier->value = m_current.literal;

	return identifier;
}

unique_ptr<Expression> Parser::parse_integer_literal() {
	auto lit = std::make_unique<IntegerLiteral>();
	lit->token = m_current;

	try {
		int res = std::stoi(m_current.literal);
		lit->value = res;
	} catch (std::invalid_argument e) {
		m_errors.push_back("could not parse integer");
		return nullptr;
	}

	return lit;
}

unique_ptr<Expression> Parser::parse_prefix_expression() {
	auto exp = std::make_unique<PrefixExpression>();
	exp->token = m_current;
	exp->opr = m_current.literal;

	next_token();
	exp->right = parse_expression(PREFIX);

	return exp;
}

Precedence Parser::peek_precedence() {
	if (precedences.find(m_peek.type) != precedences.end())
		return precedences.at(m_peek.type);
	return LOWEST;
}

Precedence Parser::current_precedence() {
	if (precedences.find(m_current.type) != precedences.end())
		return precedences.at(m_current.type);
	return LOWEST;
}

unique_ptr<Expression> Parser::parse_infix_expression(unique_ptr<Expression> left) {
	auto exp = std::make_unique<InfixExpression>();
	exp->token = m_current;
	exp->opr = m_current.literal;
	exp->left = std::move(left);

	auto prec = current_precedence();
	next_token();
	exp->right = std::move(parse_expression(prec));

	return exp;
}

std::vector<std::string> Parser::errors() const { return m_errors; }

void Parser::peek_error(TokenType tt) {
	std::string err =
			"expected next token to be " + tt + " got " + m_peek.type + " instead";
	m_errors.push_back(err);
}

void Parser::add_prefix_parse(TokenType tt, PrefixParseFn fn) {
	m_prefix_parse_fns[tt] = fn;
}

void Parser::add_infix_parse(TokenType tt, InfixParseFn fn) {
	m_infix_parse_fns[tt] = fn;
}
