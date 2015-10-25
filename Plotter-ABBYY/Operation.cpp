#include "operation.h"

std::function<double()> PlusOperation::build( const pugi::xml_node& currentNode ) const
{
	pugi::xml_node curArg = currentNode.next_sibling();
	if (curArg.empty()) {
		//error
	}
	return [=]() {
		double res = 0;
		pugi::xml_node currentNode = curArg;
		while( !currentNode.empty() ) {
			res += OperationHandler::getOperation( currentNode.name() ).build( currentNode ) ();
			currentNode = currentNode.next_sibling();
		}
		return res;
	};
}

std::function<double()> MinusOperation::build( const pugi::xml_node& currentNode ) const
{
	pugi::xml_node minuend = currentNode.next_sibling();
	if (minuend.empty()) {
		//error
	}
	pugi::xml_node subtrahend = minuend.next_sibling();
	if (subtrahend.empty()) {
		//error
	}
	return [=]() {
		return OperationHandler::getOperation( minuend.name() ).build( minuend ) () -
			OperationHandler::getOperation( subtrahend.name() ).build( subtrahend ) ();
	};
}

std::function<double()> TimesOperation::build( const pugi::xml_node& currentNode ) const
{
	pugi::xml_node curArg = currentNode.next_sibling();
	if (curArg.empty()) {
		//error
	}
	return [=] () {
		double res = 1;
		pugi::xml_node curNode = curArg;
		while( !curNode.empty( ) ) {
			res *= OperationHandler::getOperation( curNode.name() ).build( curNode ) ();
			curNode = curNode.next_sibling();
		}
		return res;
	};
}


std::function<double()> Ident::build( const pugi::xml_node& currentNode ) const
{
	//TODO: var is not found
	return [=] {
		return OperationHandler::getVar( currentNode.text().as_string() );
	};
}

std::function<double()> Number::build( const pugi::xml_node& currentNode ) const
{
	//TODO: var is not found
	return [=] {
		return currentNode.text().as_double();
	};
}

std::function<double()> ApplyOperation::build( const pugi::xml_node& currentNode ) const
{
	const pugi::xml_node opNode = currentNode.first_child();
	return OperationHandler::getOperation(opNode.name()).build(opNode);
}

const Operation& OperationHandler::getOperation(const std::string& opName) {
	auto foundOperationIterator = operations.find(opName);
	if (foundOperationIterator == operations.end()) {
		//error
	}
	return *(foundOperationIterator->second);
}


std::map<  std::string, std::unique_ptr<Operation> > OperationHandler::fillOperations() {
	std::map<  std::string, std::unique_ptr<Operation> > operationsToFill;
	operationsToFill.emplace("plus", std::unique_ptr<Operation>(new PlusOperation()) );
	operationsToFill.emplace("minus", std::unique_ptr<Operation>(new MinusOperation()));
	operationsToFill.emplace("times", std::unique_ptr<Operation>(new TimesOperation()));
	operationsToFill.emplace("ci", std::unique_ptr<Operation>(new Ident()));
	operationsToFill.emplace("cn", std::unique_ptr<Operation>(new Number()));
	operationsToFill.emplace("apply", std::unique_ptr<Operation>(new ApplyOperation()));
	return operationsToFill;
}

std::map<std::string, double> OperationHandler::fillVars() {
	std::map<std::string, double> varsToFill;
	varsToFill["x"] = 0;
	varsToFill["y"] = 0;
	varsToFill["z"] = 0;
	varsToFill["t"] = 0;
	return varsToFill;
}

double OperationHandler::getVar(const std::string& varName) {
	return vars[varName];
}

void OperationHandler::setVar(const std::string& varName, double value) {
	vars[varName] = value;
}

std::map< std::string, std::unique_ptr<Operation> > OperationHandler::operations = OperationHandler::fillOperations();
std::map<std::string, double> OperationHandler::vars = OperationHandler::fillVars();