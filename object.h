#ifndef LUPS_OBJECT_H
#define LUPS_OBJECT_H

#include "eval.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

typedef std::string ObjectType;

class Boolean;

namespace objecttypes {
const ObjectType INTEGER = "INTEGER";
const ObjectType BOOLEAN = "BOOLEAN";
const ObjectType NULLOBJ = "NULL";
const ObjectType RETURN = "RETURN";
const ObjectType ERROR = "ERROR";
const ObjectType FUNCTION = "FUNCTION";
const ObjectType STRING = "STRING";
const ObjectType BUILTIN = "BUILTIN";
const ObjectType ARRAY_OBJ    = "ARRAY";
} // namespace objecttypes

struct HashKey {
	ObjectType type;
	long long value;
};

class Object {
public:
	virtual ~Object() {}
	virtual ObjectType Type() = 0;
	virtual std::string Inspect() = 0;
};

typedef Object *(*built_in)(std::vector<Object *> &);

class Integer : public Object {
public:
	Integer(int v) : Object(), value(v) {}
	ObjectType Type() { return objecttypes::INTEGER; }
	std::string Inspect() { return std::to_string(value); }

	HashKey hash_key() {
		return HashKey{Type(), (long long)value};
	}

	int value;
};

class Boolean : public Object {
public:
	Boolean(bool b) : Object(), value(b) {}
	ObjectType Type() { return objecttypes::BOOLEAN; }
	std::string Inspect() { return value ? "true" : "false"; }

	HashKey hash_key() {
		long long value;
		value = (value ? 1 : 0);
		return HashKey{Type(), value};
	}

	bool value;
};

class Null : public Object {
public:
	ObjectType Type() { return objecttypes::NULLOBJ; }
	std::string Inspect() { return "NULL"; }
};

class Return : public Object {
public:
	Return(Object *v) : Object(), value(v) {}
	~Return() {
		delete value;
	}
	ObjectType Type() { return objecttypes::RETURN; }
	std::string Inspect() { return value->Inspect(); }

	Object *value;
};

class Error : public Object {
public:
	Error(std::string msg) : Object(), message(msg) {}
	std::string Inspect() { return "err: " + message; }
	ObjectType Type() { return objecttypes::ERROR; }

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

	~Environment() {
		delete m_outer;
	}

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
	ObjectType Type() { return objecttypes::FUNCTION; }
	std::string Inspect() { return "function :D"; }

	Environment *env;
	std::vector<Identifier *> params;
	BlockStatement *body;
};

class String : public Object {
public:
	String(const std::string &str) : Object(), value(str) {}
	ObjectType Type() { return objecttypes::STRING; }
	std::string Inspect() { return value; }

	HashKey hash_key() {
		return HashKey{Type(), (long long)std::hash<std::string>{}(value)};
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
	ObjectType Type() { return objecttypes::ARRAY_OBJ; }
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
	ObjectType Type() { return objecttypes::BUILTIN; }
	std::string Inspect() { return "builtin function"; }

	built_in func;
};


#endif
