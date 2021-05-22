#ifndef LUPS_OBJECT_H
#define LUPS_OBJECT_H

#include <string>
#include <memory>
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
	Environment() {
		m_store = std::unordered_map<std::string, Object*>();
	}

	void set(const std::string &identifier, Object *value) {
		m_store[identifier] = value;
	}

	Object *get(const std::string &identifier) {
		if (m_store.count(identifier) == 0) {
			return nullptr;
		}

		return m_store[identifier];
	}
private:
	// mapping the identifier to the object
	std::unordered_map<std::string, Object*> m_store;
};

#endif
