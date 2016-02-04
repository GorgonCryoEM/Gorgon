/*
Encapsulation on an integer handling infinity.

Author: Sasakthi S. Abeysinghe
Date  : 02/05/2006
*/
#ifndef SPECIALINT_H
#define SPECIALINT_H

const int MAXINT = 2147483647;


class SpecialInt {
public:
	SpecialInt();
	SpecialInt(int number);
	SpecialInt(int number, bool isInfinity);
	SpecialInt operator+(SpecialInt &other);
	SpecialInt operator-(SpecialInt &other);
	SpecialInt operator*(SpecialInt &other);
	SpecialInt operator/(SpecialInt &other);
	SpecialInt operator+=(SpecialInt &other);

public:
	int number;
	bool isInfinity;
};

SpecialInt::SpecialInt() {
	this->number = 0;
	this->isInfinity = false;
}

SpecialInt::SpecialInt(int number) {
	this->number = number;
	this->isInfinity = false;
}

SpecialInt::SpecialInt(int number, bool isInfinity) {
	this->number = number;
	this->isInfinity = isInfinity;
}

SpecialInt SpecialInt::operator+(SpecialInt &other) {
	return( SpecialInt(number + other.number, (isInfinity || other.isInfinity)));
}

SpecialInt SpecialInt::operator-(SpecialInt &other) {
	return( SpecialInt(number - other.number, (isInfinity || other.isInfinity)));
}

SpecialInt SpecialInt::operator*(SpecialInt &other) {
	return( SpecialInt(number * other.number, (isInfinity || other.isInfinity)));
}

SpecialInt SpecialInt::operator/(SpecialInt &other) {
	return( SpecialInt(number / other.number, (isInfinity || other.isInfinity)));
}

SpecialInt SpecialInt::operator+=(SpecialInt &other) {
	return( SpecialInt(number + other.number, (isInfinity || other.isInfinity)));
}
#endif