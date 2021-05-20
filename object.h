#ifndef LUPS_OBJECT_H
#define LUPS_OBJECT_H

#include <string>

typedef std::string ObjectType;

namespace objecttypes {
const ObjectType INTEGER = "INTEGER";
const ObjectType BOOLEAN = "BOOLEAN";
const ObjectType NULLOBJ = "NULL";
} // namespace objecttypes

class Object {
 public:
	virtual ~Object() { }
	virtual ObjectType Type() = 0;
	virtual std::string Inspect() = 0;
};

class Integer : public Object {
 public:
	Integer(int v) : Object(), value(v) { }
	ObjectType Type() { return objecttypes::INTEGER; }
	std::string Inspect() { return std::to_string(value); }

	int value;
};

class Boolean : public Object {
 public:
	Boolean(bool b) : Object(), value(b) { }
	ObjectType Type() { return objecttypes::BOOLEAN; }
	std::string Inspect() { return value ? "true" : "false"; }

	bool value;
};

class Null : public Object {
 public:
	ObjectType Type() { return NULL; }
	std::string Inspect() { return "NULL"; }
};

#endif
