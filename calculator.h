#pragma once
#include "number.h"
#include <string>
#include <stack>

class Calculator {
public:
	//конструктор калькулятора
	Calculator(const std::string &_formula);

	//подсчет значения выражения
	Number countExpression(Number &xArgument, Number &yArgument);
	
private:
	const std::string formula;
	std::stack<Number> numbers; //стек для чисел
	std::stack<std::string> operations; //стек для операций

	//получение числа для выполнения операции
	Number getOperand();

	//подсчет верхней в стеке операции
	void countOperation();

	//получение приоритета операции
	int getPriority(const std::string &operation);
};