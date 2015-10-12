#pragma once
#include <list>
#include <string>

//реализация длинных дробных чисел 
class Number {
public:
	//конструктор из double
	Number(double value);

	//конструктор из строки
	Number(std::string &token);

	void init(std::string &token);

	//сравнение с нулем
	bool equalsZero() const;

	//запись числа в строку
	void serialize(std::string &serialized) const;

	//реализация оператора унарного минуса
	Number operator-() const;

	//реализация оператора сложения
	Number operator+(const Number &secondOperand) const;

	//реализация оператора вычитания
	Number operator-(const Number &secondOperand) const;

	//реализация оператора умножения
	Number operator*(const Number &secondOperand) const;

	//реализация оператора деления
	Number operator/(const Number &secondOperand) const;

	//подсчет факториала
	Number getFactorial() const;

	friend Number;

private:
	int numDigitsAfterPoint;
	bool sign;
	std::list<int> digits;

	//проверка на отрицательность
	bool lessThanZero() const;

	//удаление ведущих нулей
	void delFirstZeros();

	//сложение
	Number sum(const Number &secondOperand) const;

	//вычитание
	Number diff(const Number &firstOperand, const Number &secondOperand) const;

	//умножение
	Number mult(const Number &firstOperand, const Number &secondOperand) const;

	//сравнение по модулю
	bool absoluteLess(const Number &secondOperand) const;

	//проверка на натуральность
	bool isNatural() const;
};