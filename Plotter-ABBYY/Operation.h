#pragma once
#include "./pugixml/pugixml.hpp"
#include <map>
#include <memory>
#include <string>
#include <functional>

class Operation {
public:
	virtual std::function<double()> build( const pugi::xml_node& currentNode ) const = 0;
};

class PlusOperation : public Operation {
public:
	std::function<double()> build( const pugi::xml_node& currentNode ) const;
private:
};

class MinusOperation : public Operation {
public:
	std::function<double()> build( const pugi::xml_node& currentNode ) const;
private:
};

class TimesOperation : public Operation {
public:
	std::function<double()> build( const pugi::xml_node& currentNode ) const;
private:
};

class Ident : public Operation {
public:
	std::function<double()> build( const pugi::xml_node& currentNode ) const;
private:
};

class Number : public Operation {
public:
	std::function<double()> build( const pugi::xml_node& currentNode ) const;
};

class ApplyOperation : public Operation {
public:
	std::function<double()> build( const pugi::xml_node& currentNode ) const;
private:
};

class OperationHandler {
public:
	static const Operation& getOperation( const std::string& opName );
	static std::map< std::string, std::unique_ptr<Operation> > fillOperations();
	static std::map<std::string, double> fillVars();
	static double getVar( const std::string& varName );
	static void setVar( const std::string& varName, double value );
	static std::map< std::string, std::unique_ptr<Operation> > operations;
	static std::map<std::string, double> vars;
};