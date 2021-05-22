#ifndef LUPS_OBJECT_H
#define LUPS_OBJECT_H

#include "eval.h"
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
} // namespace objecttypes

class Object {
public:
	virtual ~Object() {}
	virtual ObjectType Type() = 0;
	virtual std::string Inspect() = 0;
};

class Integer : public Object {
public:
	Integer(int v) : Object(), value(v) {}
	ObjectType Type() { return objecttypes::INTEGER; }
	std::string Inspect() { return std::to_string(value); }

	int value;
};

class Boolean : public Object {
public:
	Boolean(bool b) : Object(), value(b) {}
	ObjectType Type() { return objecttypes::BOOLEAN; }
	std::string Inspect() { return value ? "true" : "false"; }

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
	Object* set(std::string name, Object* val) {
		m_store[name] = val;
		return nullptr;
	}

	Object *get(std::string name) {
		if (m_store.find(name) == m_store.end()) {
			return new Error("identifier not found: " + name);
		}

		return m_store[name];
	}

	std::unordered_map<std::string, Object *> m_store;
};

#endif
