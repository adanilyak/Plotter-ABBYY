#include "mathCore.h"
#include "number.h"
#include <string>
#include <iostream>
#include <exception>

MathCore::MathCore(const std::string &formula) :
	calc(formula),
	globalXShift(0),
	globalYShift(0),
	globalZShift(0)
{
}

MathCore::~MathCore() {
}

double MathCore::calculate(double x, double y) {
	try {
		Number result = calc.countExpression(Number(x) + globalXShift, Number(y) + globalYShift) - globalZShift;
		std::string strResult;
		result.serialize(strResult);
		return std::stod(strResult);
	}
	catch (std::logic_error catchedException) {
		std::cout << catchedException.what() << ", impossible to count" << '\n';
	}
	return 0;
}

void MathCore::changeWindowCoordinates(double x, double y, double z) {
	globalXShift = globalXShift + Number(x);
	globalYShift = globalYShift + Number(y);
	globalZShift = globalZShift + Number(z);
}


void MathCore::changeScale(double factor) {
	globalXShift = globalXShift * Number(factor);
	globalYShift = globalXShift * Number(factor);
	globalZShift = globalXShift * Number(factor);
}