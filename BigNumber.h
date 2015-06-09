#pragma once

#include <cstddef>
#include <iostream>
using namespace std;

const int PREC = 4;
class BigNumber
{
	//пази цифрите в 10-ична бройна система
	char* digits;
	//пази цифри след десетичната запетая, ако имаме дроб
	char* fraction;
	//пази цифрите в 16-ична бройна система
	char* digits_bin;
	//пази шестнайсетични цифри след запетаята
	char* fraction_bin;
	//пази знака
	short int sign;
	//пазят брой цифри 
	int numberOfDigits;
	int numberOfDigits2;
	int numberOfFracDigits;
	int numberOfFracDigits2;
	//пази текущата основа на бройната система
	int currentBase;
	//пази точността, при която да се спира при деление
	static int precision;
public:
	//конструктор, приемащ цяло число, опционален параметър, указващ
	//брой нули, които да се добавят в края, т.е. по коя степен на 10 да се умножи
	//и параметър, указващ основата на бройната система, 10 по подразбиране
	BigNumber(long=0 , int=0, int=10);
	//конструктор, конструиращ число по подадена десетична дроб
	//вторият параметър оказва точността в брой цифри след десетичната запетая
	BigNumber(double,int);
	//конструктор за копиране с втори параметър, 0 по подразбиране - можем да
	//поискаме да се копира числото, но умножено по някоя степен на базата
	BigNumber(BigNumber const&, int =0);

	BigNumber& operator=(BigNumber const&);
	~BigNumber();

	//оператори за аритметични действия; ако числата са в различни бройни системи
	//шестнайсетичното се трансформира до десетично
	BigNumber operator-() const;
	BigNumber operator+(BigNumber const&) const;
	BigNumber& operator+=(BigNumber const&);
	BigNumber operator-(BigNumber const&) const;
	BigNumber& operator-=(BigNumber const&);

	BigNumber operator*(BigNumber const&) const;
	BigNumber& operator*=(BigNumber const&);
	BigNumber operator/(BigNumber const&) const;
	BigNumber& operator/=(BigNumber const&);
	BigNumber operator%(BigNumber const&) const;
	BigNumber& operator%=(BigNumber const&);

	bool isPositive() const { return sign==1; }
	bool isNegative() const { return sign==-1; }
	bool isZero() const { return sign == 0; }
	bool isInteger() const { return numberOfFracDigits==0; }
	BigNumber abs(BigNumber const&) const;

	void convertToDecimal();
	void convertToBase();
	BigNumber getDecimal() const;
	BigNumber getHex() const;

	bool operator==(BigNumber const&) const;
	bool operator<(BigNumber const&) const;
	bool operator<=(BigNumber const&) const;
	bool operator>(BigNumber const&) const;
	bool operator>=(BigNumber const&) const;
	bool operator!=(BigNumber const&) const;

	BigNumber operator&(BigNumber const&) const;
	BigNumber& operator&=(BigNumber const&);
	BigNumber operator|(BigNumber const&) const;
	BigNumber& operator|=(BigNumber const&);
	BigNumber operator^(BigNumber const&) const;
	BigNumber& operator^=(BigNumber const&);
	BigNumber operator~() const;
	BigNumber operator<<(int) const;
	BigNumber& operator<<=(int);
	BigNumber operator>>(int) const;
	BigNumber& operator>>=(int);

	char& operator[](int);
	char at(int) const;

	int getNumberOfDigits(int base=10) const {
		if(base==10)
			return numberOfDigits;
		if(numberOfDigits2%4!=0)
			return (numberOfDigits2/4 + 1);
		return numberOfDigits2/4;
	}

	void setPrecision(int prec) {
		precision = prec;
	}

	//операторът за вход въвежда число, като:
	//ако започва с -, то е отрицателно
	//ако в началото или след минус има 0х, значи е в 16-ична система
	friend istream& operator>>(istream& is, BigNumber&);
	//извежда число на екрана в текущата бройна система
	friend ostream& operator<<(ostream& os, BigNumber const&);
private:
	//private конструктор, който конструира число по подаден
	//низ от цифри в обратен ред
	BigNumber(const char*, int,int=0,int=10);
	
	//помощна функция за копиране; използва функцията strcpy от стандартната библиотека
	void copyFrom(BigNumber const&, int =0);
	//помощна функция за зачистване; вторият параметър показва дали 
	//се прилага извън деструктор - тогава инциализира всички полета
	void clean(bool =true);
	
	//помощни зачистващи функциии за отделните полета
	void clean_digits();
	void clean_digits_bin();
	void clean_frac();
	void clean_frac_bin();

	int compare(BigNumber const&) const;
	int compareMods(BigNumber const&) const;

	void setDigits10(char*,int,bool =true, bool =true);
	void setDigits16(char*,int,bool =true, bool =false);
	void setFraction10(char*,int, bool =true);
	void setFraction16(char*,int, bool =false);

	int countDigits(long, int =10) const;

	//помощна функция, която по подадено число и padding
	//записва цифрите му в масив
	void buildFromNumber(long, int, int);
	void buildFromString(const char*, int, int=10);

	//помощна функция за buildFromNumber; четвъртият параметър 
	//показва дали става въпрос за дробната част, която трябва да се
	//запише наобратно
	void buildNumber(long, int, int,bool =false);

	//помощна функция, която приема низ, разделя го на две части, ако има точка
	//и записва в съответните масиви въпросните низове, представяща цяла и дробна част
	//за целта приема указатели кои функции да извика
	void buildString(const char* str,void (BigNumber::*setDigits)(char*,int,bool,bool), void (BigNumber::*cleanDigits)(),
					void (BigNumber::*setFraction)(char*,int,bool),void (BigNumber::*cleanFraction)(),
					void (BigNumber::*cleanIfInt)());
	//функция, която инциализира обекта т.че стойността му да се интерпретира като нула
	void buildZero();


	//помощни функции за аритметичните действия с числа и дроби
	int addNumbers(char*&, const char*, int,const char*, int, int, int=0) const;
	int subtractNumbers(char*&, const char*, int, const char*, int, int, int=0) const;
	int addFraction(char*&, const char*, int, const char*, int, int, int&) const;
	int removeFraction(char*&, const char*, int, const char*, int, int, int&) const;
	BigNumber multiplyNumbers(BigNumber const&, BigNumber const&, int = 10) const;
	BigNumber divideNumbers(BigNumber const&, BigNumber const&,BigNumber&, int=10) const;

	char* getBinary(BigNumber const&, int&) const;
	BigNumber fromBinary(const char*, int) const;
	BigNumber applyBinOp(BigNumber const&, BigNumber const&, char (*op)(char,char)) const;

	//помощна функция, която построява масив
	//от цяла и дробна част
	char* buildArr(BigNumber const&, int&, int&) const;
	//по масив и посочена прецизност построява обратно число
	BigNumber buildNumberFromStr(BigNumber const&, int, int) const;

	BigNumber multiplyBaseCaseHelper(BigNumber const&, BigNumber const&,
											char* BigNumber::*, int BigNumber::*,
											char* BigNumber::*, int BigNumber::*,
											int base) const;
	void buildNumberFromStrHelper(const char*, void (BigNumber::*setDigits)(char*,int,bool,bool),
							void (BigNumber::*setFraction)(char*,int,bool),int, int);
	char* buildArrHelper(char*& res, const char* fraction, const char* digits,
					int& size, int numberOfDigits,
					int numberOfFracDigits, int& precision) const;
	BigNumber abs_floor(BigNumber const&) const;
};

