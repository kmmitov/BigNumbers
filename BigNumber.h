#pragma once

#include <cstddef>
#include <iostream>
using namespace std;

const int PREC = 4;
class BigNumber
{
	//���� ������� � 10-���� ������ �������
	char* digits;
	//���� ����� ���� ����������� �������, ��� ����� ����
	char* fraction;
	//���� ������� � 16-���� ������ �������
	char* digits_bin;
	//���� �������������� ����� ���� ���������
	char* fraction_bin;
	//���� �����
	short int sign;
	//����� ���� ����� 
	int numberOfDigits;
	int numberOfDigits2;
	int numberOfFracDigits;
	int numberOfFracDigits2;
	//���� �������� ������ �� �������� �������
	int currentBase;
	//���� ���������, ��� ����� �� �� ����� ��� �������
	static int precision;
public:
	//�����������, ������� ���� �����, ���������� ���������, �������
	//���� ����, ����� �� �� ������� � ����, �.�. �� ��� ������ �� 10 �� �� ������
	//� ���������, ������� �������� �� �������� �������, 10 �� ������������
	BigNumber(long=0 , int=0, int=10);
	//�����������, ����������� ����� �� �������� ��������� ����
	//������� ��������� ������ ��������� � ���� ����� ���� ����������� �������
	BigNumber(double,int);
	//����������� �� �������� � ����� ���������, 0 �� ������������ - ����� ��
	//�������� �� �� ������ �������, �� �������� �� ����� ������ �� ������
	BigNumber(BigNumber const&, int =0);

	BigNumber& operator=(BigNumber const&);
	~BigNumber();

	//��������� �� ����������� ��������; ��� ������� �� � �������� ������ �������
	//���������������� �� ������������ �� ���������
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

	//���������� �� ���� ������� �����, ����:
	//��� ������� � -, �� � �����������
	//��� � �������� ��� ���� ����� ��� 0�, ����� � � 16-���� �������
	friend istream& operator>>(istream& is, BigNumber&);
	//������� ����� �� ������ � �������� ������ �������
	friend ostream& operator<<(ostream& os, BigNumber const&);
private:
	//private �����������, ����� ���������� ����� �� �������
	//��� �� ����� � ������� ���
	BigNumber(const char*, int,int=0,int=10);
	
	//������� ������� �� ��������; �������� ��������� strcpy �� ������������ ����������
	void copyFrom(BigNumber const&, int =0);
	//������� ������� �� ����������; ������� ��������� ������� ���� 
	//�� ������� ����� ���������� - ������ ����������� ������ ������
	void clean(bool =true);
	
	//������� ���������� �������� �� ��������� ������
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

	//������� �������, ����� �� �������� ����� � padding
	//������� ������� �� � �����
	void buildFromNumber(long, int, int);
	void buildFromString(const char*, int, int=10);

	//������� ������� �� buildFromNumber; ���������� ��������� 
	//������� ���� ����� ������ �� �������� ����, ����� ������ �� ��
	//������ ���������
	void buildNumber(long, int, int,bool =false);

	//������� �������, ����� ������ ���, ������� �� �� ��� �����, ��� ��� �����
	//� ������� � ����������� ������ ���������� ������, ����������� ���� � ������ ����
	//�� ����� ������ ��������� ��� ������� �� ������
	void buildString(const char* str,void (BigNumber::*setDigits)(char*,int,bool,bool), void (BigNumber::*cleanDigits)(),
					void (BigNumber::*setFraction)(char*,int,bool),void (BigNumber::*cleanFraction)(),
					void (BigNumber::*cleanIfInt)());
	//�������, ����� ����������� ������ �.�� ���������� �� �� �� ������������ ���� ����
	void buildZero();


	//������� ������� �� ������������� �������� � ����� � �����
	int addNumbers(char*&, const char*, int,const char*, int, int, int=0) const;
	int subtractNumbers(char*&, const char*, int, const char*, int, int, int=0) const;
	int addFraction(char*&, const char*, int, const char*, int, int, int&) const;
	int removeFraction(char*&, const char*, int, const char*, int, int, int&) const;
	BigNumber multiplyNumbers(BigNumber const&, BigNumber const&, int = 10) const;
	BigNumber divideNumbers(BigNumber const&, BigNumber const&,BigNumber&, int=10) const;

	char* getBinary(BigNumber const&, int&) const;
	BigNumber fromBinary(const char*, int) const;
	BigNumber applyBinOp(BigNumber const&, BigNumber const&, char (*op)(char,char)) const;

	//������� �������, ����� ��������� �����
	//�� ���� � ������ ����
	char* buildArr(BigNumber const&, int&, int&) const;
	//�� ����� � �������� ���������� ��������� ������� �����
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

