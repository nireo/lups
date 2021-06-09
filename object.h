#ifndef LUPS_OBJECT_H
#define LUPS_OBJECT_H

#include "ast.h"
#include "code.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Boolean;

enum class ObjType {
	Integer,
	Boolean,
	Null,
	Return,
	Error,
	Function,
	String,
	Builtin,
	Array,
	Hash,
	CompiledFunction
};

typedef long long HashValue;
struct HashKey {
	ObjType type;
	HashValue value;
};

class Object {
public:
	virtual ~Object() {}
	virtual ObjType Type() = 0;
	virtual std::string Inspect() = 0;
};

class CompiledFunction : public Object {
public:
	CompiledFunction(code::Instructions inst)
			: Object(), m_instructions(inst), m_num_locals(0) {}

	CompiledFunction(code::Instructions inst, int num_locals)
			: Object(), m_instructions(inst), m_num_locals(num_locals) {}

	ObjType Type() { return ObjType::CompiledFunction; }
	std::string Inspect() { return "compiled-function"; }

	code::Instructions m_instructions;
	int m_num_locals;
	int m_num_parameters;
};

typedef Object *(*built_in)(std::vector<Object *> &);

class Integer : public Object {
public:
	Integer(int v) : Object(), value(v) {}
	ObjType Type() { return ObjType::Integer; }
	std::string Inspect() { return std::to_string(value); }

	HashKey hash_key() { return HashKey{Type(), (HashValue)value}; }
	int value;
};

class Boolean : public Object {
public:
	Boolean(bool b) : Object(), value(b) {}
	ObjType Type() { return ObjType::Boolean; }
	std::string Inspect() { return value ? "true" : "false"; }

	HashKey hash_key() { return HashKey{Type(), (HashValue)(value ? 1 : 0)}; }

	bool value;
};

class Null : public Object {
public:
	ObjType Type() { return ObjType::Null; }
	std::string Inspect() { return "NULL"; }
};

class Return : public Object {
public:
	Return(Object *v) : Object(), value(v) {}
	~Return() { delete value; }
	ObjType Type() { return ObjType::Return; }
	std::string Inspect() { return value->Inspect(); }

	Object *value;
};

class Error : public Object {
public:
	Error(std::string msg) : Object(), message(msg) {}
	std::string Inspect() { return "err: " + message; }
	ObjType Type() { return ObjType::Error; }

	std::string message;
};

class Environment {
public:
	Environment() {
		m_store = std::unordered_map<std::string, Object *>();
		m_outer = nullptr;
	}

	Environment(Environment *outer) {
		m_store = std::unordered_map<std::string, Object *>();
		m_outer = outer;
	}

	~Environment() { delete m_outer; }

	Object *set(std::string name, Object *val) {
		m_store[name] = val;
		return nullptr;
	}

	Object *get(std::string name) {
		if (m_store.find(name) == m_store.end()) {
			if (m_outer != nullptr) {
				auto res = m_outer->get(name);
				return res;
			}
			return new Error("identifier not found: " + name);
		}

		return m_store[name];
	}

	std::unordered_map<std::string, Object *> m_store;
	Environment *m_outer;
};

class Function : public Object {
public:
	~Function() {
		delete env;
		delete body;
		for (auto pr : params)
			delete pr;
	}
	ObjType Type() { return ObjType::Function; }
	std::string Inspect() { return "function"; }

	Environment *env;
	std::vector<Identifier *> params;
	BlockStatement *body;
};

class String : public Object {
public:
	String(const std::string &str) : Object(), value(str) {}
	ObjType Type() { return ObjType::String; }
	std::string Inspect() { return value; }

	HashKey hash_key() {
		return HashKey{Type(), (HashValue)std::hash<std::string>{}(value)};
	}

	std::string value;
};

class Array : public Object {
public:
	Array(std::vector<Object *> &elems) : Object(), elements(elems) {}
	~Array() {
		for (auto elem : elements)
			delete elem;
	}

	ObjType Type() { return ObjType::Array; }
	std::string Inspect() {
		std::string res = "[";
		for (auto elem : elements) {
			res += elem->Inspect() + ", ";
		}
		res += "]";
		return res;
	}

	std::vector<Object *> elements;
};

class Builtin : public Object {
public:
	Builtin(built_in fn) : Object(), func(fn) {}
	ObjType Type() { return ObjType::Builtin; }
	std::string Inspect() { return "builtin function"; }

	built_in func;
};

struct HashPair {
	~HashPair() {
		delete key;
		delete value;
	}

	Object *key;
	Object *value;
};

class Hash : public Object {
public:
	Hash() { pairs = std::unordered_map<HashValue, HashPair *>(); }

	~Hash() {
		for (auto pr : pairs)
			delete pr.second;
	}

	ObjType Type() { return ObjType::Hash; }
	std::string Inspect() {
		std::string result;
		result += "{";
		for (const auto &pr : pairs) {
			result += pr.second->key->Inspect() + ": " + pr.second->value->Inspect();
		}

		result += "}";

		return result;
	}

	std::unordered_map<HashValue, HashPair *> pairs;
};

#endif
