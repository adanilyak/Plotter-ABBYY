#include "calculator.h"
#include "number.h"
#include <string>
#include <exception>
#include <stack>

Calculator::Calculator(const std::string &_formula) :
	formula(_formula)
{
}

//получение числа для выполнения операции
Number Calculator::getOperand() {
	if (numbers.empty()) {
		throw std::logic_error("wrong expression");
	}
	Number result = numbers.top();
	numbers.pop();

	return result;
}

//подсчет верхней в стеке операции
void Calculator::countOperation() {
	std::string operation;
	if (!(operations.empty())) {
		operation = operations.top();
		operations.pop();
	} else {
		throw std::logic_error("wrong expression"); //попытка выполнить несуществующую операцию
	}

	if (operation == "(") {
		throw std::logic_error("mismatched brackets");
	}

	Number secondOperand = getOperand();

	if (operation == "!") {
		numbers.push( secondOperand.getFactorial() );
		return;
	}

	Number firstOperand = getOperand();

	//подсчет значения операции для полученных операндов
	if (operation == "+") {
		numbers.push(firstOperand + secondOperand);
	} else if (operation == "-") {
		numbers.push(firstOperand - secondOperand);
	} else if (operation == "*") {
		numbers.push(firstOperand * secondOperand);
	} else if (operation == "/") {
		numbers.push(firstOperand / secondOperand);
	}
}

//получение приоритета операции
int Calculator::getPriority(const std::string &operation) {
	if ((operation == "+") || (operation == "-")) {
		return 1;
	} else if ((operation == "*") || (operation == "/")) {
		return 2;
	} else {
		return 0;
	}
}

//подсчет значения выражения
Number Calculator::countExpression(Number &xArgument, Number &yArgument) {
	size_t posInArgument = 0;
	while (posInArgument < formula.length()) {
		std::string currentToken = "";
		currentToken += formula[posInArgument];

		if ((currentToken >= "0") && (currentToken <= "9")) {
			while ((posInArgument + 1 < formula.length()) && ((formula[posInArgument + 1] == '.')
				|| ((formula[posInArgument + 1] >= '0') && (formula[posInArgument + 1] <= '9')))) {
				currentToken += formula[posInArgument + 1];
				++posInArgument;
			}
			Number currentNumber = Number(stod(currentToken));
			numbers.push(currentNumber);
		} else if (currentToken == "x") {
			numbers.push(xArgument);
		} else if (currentToken == "y") {
			numbers.push(yArgument);
		} else if (currentToken == "(") {
			operations.push("(");
		} else if (currentToken == ")") {
			while (!(operations.empty()) && !(operations.top() == "(")) {
				countOperation();
			}

			if (!(operations.empty())) {
				operations.pop();
			} else {
				throw std::logic_error("mismatched brackets");
			}
		} else if ((currentToken == "+") || (currentToken == "-") || (currentToken == "*") || (currentToken == "/")) {
			while (!(operations.empty()) && (getPriority(operations.top()) >= getPriority(currentToken))) {
				countOperation();
			}
			operations.push(currentToken);
		} else if (currentToken == "!") {
			operations.push("!");
		} else if (!(currentToken == " ")) {
			throw std::logic_error("wrong expression");
		}

		++posInArgument;
	}

	while (!operations.empty()) {
		countOperation();
	}

	if (numbers.empty()) {
		throw std::logic_error("wrong expression");
	} else {
		Number result = numbers.top();
		numbers.pop();
		if (!numbers.empty()) {
			throw std::logic_error("wrong expression");
		} else {
			return result;
		}
	}
}