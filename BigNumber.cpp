#include "BigNumber.h"
#include <cstring>
#include <cmath>

const int BUF_SIZE = 100;
/*���������, ����� ��������� ����������� �� ���������� ����� �����;
�������� �����������, �������� � ����, ���������� � ����������� �� ������*/
struct String_buf
{
	char* str;
	int len;
	int capacity;

	String_buf(const char* _str)
	{
		int length = strlen(_str);
		str = new char[length+1];
		strcpy(str,_str);
		capacity = len = length;
	}

	~String_buf()
	{
		delete [] str;
	}

	void append(const char* _str)
	{
		int sLen = strlen(_str);
		if(len + sLen + 1>capacity)
		{
			resize(max(2*capacity,len+sLen+1));
		}
		strcpy(str+len,_str);
		len += sLen;
		str[len] = '\0';
	}

	void resize(int new_cap)
	{
		char* tmp = str;
		capacity = new_cap;
		str = new char[new_cap];
		strcpy(str,tmp);
		delete [] tmp;
	}

	char& operator[](int index)
	{
		return str[index];
	}
};

//�������� ���� ������ � ����� � 16-���� ������� ��� . - ��� �����, �� �� �����
//���� �� ����� �� ������ ������
bool is_digit(char c)
{
	return (c>='0'&&c<='9')||(c>='A'&&c<='F')||(c=='.');
}

//�� ����� ����� �������, �� ����� �������� � 16-���� �������
char get_char(int digit)
{
	if(digit>=0&&digit<10)
		return '0' + digit;
	if(digit>=10&&digit<16)
		return 'A' + digit - 10;
}

//�� ����� �� 2 ����� � 10-���� ������� ����� ������� � 16-����,
//�� ����� ��������
char get_char(const char* dig, int num) 
{
	if(num==1)
	{
		return dig[0];
	}
	switch(dig[0])
	{
	case '0': return 'A';
	case '1': return 'B';
	case '2': return 'C';
	case '3': return 'D';
	case '4': return 'E';
	case '5': return 'F';
	}
}

//�������� �������������� ����� � ����� �������, �������� ���� ������
//��� �������, ������ �������� ����� ������ ��� ������ �� ����������
int cmp(const char* f, const char* s, int len)
{
	int index = len-1;
	while(index>=0&&f[index]==s[index])
	{
		index--;
	}
	if(index>=0)
	{
		return f[index] - s[index];
	}
	return 0;
}

//�� ����� ������ ����� ����������, �� ����� ��������
int get_value(char c)
{
	if(c>='0'&&c<='9')
		return c-'0';
	if(c>='A'&&c<='F')
		return c-'A'+10;
}

//������� �� �������� �, ��� � ���������� ��� �� ��� �������
char op_mult(char c1, char c2)
{
	return get_char(get_value(c1)&get_value(c2));
}

char op_sum(char c1, char c2)
{
	return get_char(get_value(c1)|get_value(c2));
}

char op_pow(char c1, char c2)
{
	return get_char(get_value(c1)^get_value(c2));
}

//������� ������� �����, ���� �� �� �������� ��������� ������ � �� ������� � ����
//��� ����������� shift ��� �� �� �������� �������, ����������� ����� ��� �����������
void shift(char*& str,int& len, int shift)
{
	char* tmp = str;
	if(shift<=-len)
	{
		delete [] tmp;
		str = new char[2];
		str[0] = '0';
		str[1] = '\0';
		len=1;
		return;
	}
	str = new char[len + shift + 1];
	if(shift>=0)
	{
		for(int i=0;i<shift;i++)
			str[i] = '0';
		strcpy(str+shift,tmp);
	}
	else
	{
		strcpy(str,tmp-shift);
	}
	len+=shift;
	delete [] tmp;
}

BigNumber::BigNumber(long number, int padding, int base)
	:digits(NULL),digits_bin(NULL),
	fraction(NULL), fraction_bin(NULL),
	numberOfDigits(0),numberOfFracDigits(0),
	numberOfDigits2(0),numberOfFracDigits2(0)
{
	buildFromNumber(number,padding,base);
}

BigNumber::BigNumber(double fr, int numberOfDecimal)
	:digits(NULL),digits_bin(NULL),
	fraction(NULL),fraction_bin(NULL),
	numberOfDigits(0),numberOfDigits2(0),
	numberOfFracDigits(0),numberOfFracDigits2(0)
{
	//������� ����� �� ������� � ������ ����
	long arg;
	if(fr<0)
	{
		sign = -1;
		fr = -fr;
		arg = floor(fr);
		//�� ������ ���� ����������� ������ digits ���� ��� ���� �����
		buildFromNumber(-arg,0,10);
	}
	else
	{
		arg = floor(fr);
		buildFromNumber(arg,0,10);
	}

	//�������� �������� ���� � ���������� �� ������ �� 10, �������� � ���������
	//��������� ���� ����, ������ ���������� ���� �����, ����� � �������� ����,
	//�������� ���� ��������� ����
	fr-= arg;
	fr *= pow(10,numberOfDecimal);
	long frPart = floor(fr+0.5);
	
	//�� ��� ������ ������ ������� �� ���������, ����� ��������� ������ ���� �� ������� �� �����
	//��������� � � ���������� true
	buildNumber(frPart,10,0,true);
}

BigNumber::BigNumber(BigNumber const& other, int padding)
	:digits(NULL),digits_bin(NULL),
	fraction(NULL),fraction_bin(NULL),
	numberOfDigits(0),numberOfDigits2(0),
	numberOfFracDigits(0),numberOfFracDigits2(0)
{
	copyFrom(other, padding);
}

BigNumber& BigNumber::operator=(BigNumber const& other)
{
	if(this!=&other)
	{
		clean(true);
		copyFrom(other);
	}
	return *this;
}


BigNumber::~BigNumber()
{
	clean(false);
}



istream& operator>>(istream& is, BigNumber& target)
{
	/*������ ����� � ��������� ��������� � ������ ���������� ������ � ����� ��� .
	������������ �� ��������� �����. ���� ���� �� ������� ���������� �������*/
	char buf[BUF_SIZE+1];
	is.get(buf,BUF_SIZE);
	String_buf buffer(buf);
	while(is_digit(is.peek()))
	{
		is.get(buf,BUF_SIZE);
		buffer.append(buf);
	}
	//�� ���������� �� ��������� � ������ whitespace
	is.ignore();


	//���������� �������� � � ������ 0, ��� ���� � ��������
	target.clean();
	if(strcmp(buffer.str,"0")==0)
	{
		target.buildZero();
		return is;
	}
	//� �������� ������ ����������� ���������� �� ���� � ���������� ������ �������
	int sign=1;
	int base=10;
	if(buffer[0]=='-')
	{
		sign = -1;
		if(buffer[1]=='0'&&buffer[2]=='x')
		{
			base =16;
			target.buildFromString(buffer.str+3,sign,base);
		}
		else
		{
			target.buildFromString(buffer.str+1,sign,base);
		}
	}
	else if(buffer[0]=='0'&&buffer[1]=='x')
	{
		base=16;
		target.buildFromString(buffer.str+2,sign,base);
	}
	else
	{
		target.buildFromString(buffer.str,sign,base);
	}
	return is;
}

//��������� ������� � ���������� �����, ���� ���-������ � �����, � ���� ���� ���������� ����������� ��
//������ �������. ������������ ����� ���� � ������ ���� � .
ostream& operator<<(ostream& os, BigNumber const& number) 
{
	if(number.sign == -1)
	{
		os<<'-';
	}
	if(number.isZero())
	{
		if(number.currentBase ==16)
		{
			os<<"0x";
		}
		os<<'0';
		return os;
	}
	if(number.currentBase==10)
	{
		for(int i=number.numberOfDigits-1;i>=0;i--)
			os<<number.digits[i];
		if(number.numberOfFracDigits>0)
		{
			os<<'.';
			for(int i=0;i<number.numberOfFracDigits;i++)
				os<<number.fraction[i];
		}
		
	}
	else
	{
		os<<"0x";
		for(int i=number.numberOfDigits2-1;i>=0;i--)
			os<<number.digits_bin[i];
		if(number.numberOfFracDigits2>0)
		{
			os<<'.';
			for(int i=0;i<number.numberOfFracDigits2;i++)
				os<<number.fraction_bin[i];
		}
	}
	return os;
	
}


BigNumber BigNumber::operator-() const
{
	BigNumber tmp(*this);
	tmp.sign = -tmp.sign;
	return tmp;
}

BigNumber& BigNumber::operator+=(BigNumber const& other)
{
	//��� � ����, ���������� ��������
	if(this->isZero())
	{
		return this->operator=(other);
	}
	//��� ������ �� � ����, �������� � �����������
	if(this->operator==(-other))
	{
		clean();
		buildZero();
		return *this;
	}
	else
	{
		//a�� ������� � ����, ���������� ������� �� ��������� ��� �������
		if(!other.isZero())
		{
			char* targetD = NULL;
			char* targetF = NULL;
			//����������, � ����� ����� ��������; �� � �������� �� ���������;
			//������ ���� ��� ����������� � ������� �� ������ � �������� ����
			int carries = 0;
			//��� �� � ������� �����, �������� ��������� ������ � �������� �����
			if((this->isPositive()&&other.isPositive())
		||(this->isNegative()&&other.isNegative()))
			{
				//��� ��� ������ ����
				if(!this->isInteger()||!other.isInteger())
				{
					if(this->currentBase==10)
					{
						int numberOfFracDigits = addFraction(targetF,this->fraction,this->numberOfFracDigits, other.fraction,
							other.numberOfFracDigits,10,carries);
						int numberOfDigits = addNumbers(targetD,this->digits,this->numberOfDigits,
							other.digits, other.numberOfDigits, 10, carries);
						setDigits10(targetD,numberOfDigits);
						setFraction10(targetF,numberOfFracDigits,false);
					}
					if(this->currentBase==16)
					{
						int numberOfFracDigits2 = addFraction(targetF,this->fraction_bin,this->numberOfFracDigits2, other.fraction_bin,
							other.numberOfFracDigits2,16,carries);
						int numberOfDigits2 = addNumbers(targetD,this->digits_bin,this->numberOfDigits2,
							other.digits_bin, other.numberOfDigits2, 16, carries);
						setDigits16(targetD,numberOfDigits2);
						setFraction16(targetF,numberOfFracDigits2,false);
					}
				}
				else
					//��� �� ���� �����
				{
					if(this->currentBase==10)
					{
						int numberOfDigits = addNumbers(targetD,this->digits,this->numberOfDigits,
							other.digits, other.numberOfDigits, 10,carries);
						setDigits10(targetD,numberOfDigits);
					}
					if(this->currentBase==16)
					{
						int numberOfDigits2 = addNumbers(targetD,this->digits_bin,this->numberOfDigits2,
							other.digits_bin, other.numberOfDigits2, 16, carries);
						setDigits16(targetD,numberOfDigits2);
					}
					clean_frac();
					clean_frac_bin();
				}
			//������ ������ ���� �� �������, �.�. ����������
			}
			else
			{
			//����������� �������� ����, �� 
			//������� �� � ��-�������� ��-�����, � negative
			//�� � ����������� ���� ��� ������ � �����������
			BigNumber first, second;
			bool negative = false;
			if(this->isPositive()&&other.isNegative())
			{
				if(this->compareMods(other)>0)
				{
					first = *this;
					second = (-other);
					negative = false;
				}
				else
				{
					first = -other;
					second = *this;
					negative = true;
				}
			}
			else if(this->isNegative()&&other.isPositive())
			{
				if(this->compareMods(other)>0)
				{
					first = -(*this);
					second = other;
					negative = true;
				}
				else
				{
					first = other;
					second = -(*this);
					negative = false;
				}
			}
			if(!this->isInteger()||!other.isInteger())
				//��� �� ������
				{
					if(this->currentBase==10)
					{
						int numberOfFracDigits = removeFraction(targetF,first.fraction,first.numberOfFracDigits, second.fraction,
							second.numberOfFracDigits,10,carries);
						int numberOfDigits = subtractNumbers(targetD,first.digits,first.numberOfDigits,
							second.digits, second.numberOfDigits, 10, carries);
						setDigits10(targetD,numberOfDigits);
						setFraction10(targetF,numberOfFracDigits);
					}
					if(this->currentBase==16)
					{
						int numberOfFracDigits2 = removeFraction(targetF,first.fraction_bin,first.numberOfFracDigits2, second.fraction_bin,
							second.numberOfFracDigits2,16,carries);
						int numberOfDigits2 = subtractNumbers(targetD,first.digits_bin,first.numberOfDigits2,
							second.digits_bin, second.numberOfDigits2, 16, carries);
						setDigits16(targetD,numberOfDigits2);
						setFraction16(targetF,numberOfFracDigits2);
					}
				}
				else
				{
			//��� �� ����
					if(this->currentBase==10)
					{
						int numberOfDigits = subtractNumbers(targetD,first.digits,first.numberOfDigits,
							second.digits, second.numberOfDigits, 10);
						setDigits10(targetD,numberOfDigits);
					}
					if(this->currentBase==16)
					{
						int numberOfDigits2 = subtractNumbers(targetD,first.digits_bin,first.numberOfDigits2,
							second.digits_bin, second.numberOfDigits2, 16, carries);
						setDigits16(targetD,numberOfDigits2);
					}
					clean_frac();
					clean_frac_bin();
				}
			  if(negative)
			  {
				  sign = -1;
			  }
		}

		}
	}
	return *this;
	
}

BigNumber& BigNumber::operator-=(BigNumber const& other)
{
	if(this==&other)
	{
		clean();
		buildZero();
		return *this;
	}
	(*this) += (-other);
	return *this;
}

BigNumber BigNumber::operator+(BigNumber const& other) const
{
	BigNumber tmp(*this);
	return tmp += other;
}

BigNumber BigNumber::operator-(BigNumber const& other) const
{
	BigNumber tmp(*this);
	return tmp -= other;
}

BigNumber BigNumber::operator*(BigNumber const& other) const
{
	if(this->isZero()||other.isZero())
	{
		BigNumber res;
		res.buildZero();
		return res;
	}
	//��� �������� ������ ������� �������� �� ���������
	if(this->currentBase!=other.currentBase)
	{
		BigNumber first, second;
		if(this->currentBase==10)
		{
			first = *this;
			second = other.getDecimal();
		}
		else
		{
			first = this->getDecimal();
			second = other;
		}
		return first*second;
	}
	if(!this->isInteger()||!other.isInteger())
	{
		//��� ��� ������ �����, ���������� �� ����������� ������ �� ������
		//�� �� ������ �� ��������� �� ���� ����� � ����� ������� �����������
		//������� �� ������� ��
		int fNum, sNum;
		int prec1, prec2;
		const char* first_arr = buildArr(*this, fNum, prec1);
		const char* second_arr = buildArr(other, sNum, prec2);
		BigNumber a(first_arr,fNum,0,this->currentBase),
			b(second_arr,sNum,0,this->currentBase);
		BigNumber res_cand = a*b;
		res_cand.sign = this->sign*other.sign;
		delete [] first_arr;
		delete [] second_arr;
		return buildNumberFromStr(res_cand, (res_cand.numberOfDigits==0?res_cand.numberOfDigits2:res_cand.numberOfDigits) , prec1 + prec2);
	}
	//��� ���� ����� ������� �� ����������
	BigNumber result = multiplyNumbers(*this,other, this->currentBase);
	result.currentBase = this->currentBase;
	result.sign = this->sign*other.sign;
	return result;
}

BigNumber& BigNumber::operator*=(BigNumber const& other)
{
	*this = (*this*other);
	return *this;
}

BigNumber BigNumber::operator/(BigNumber const& other) const
{
	if(this->isZero())
	{
		return BigNumber();
	}
	if(other.isZero())
	{
		cerr<<"Division by zero!"<<endl;
		return BigNumber();
	}
	//��� �� �� � ���� � ���� ������ �������, ������������ �� ���������
	if(this->currentBase!=other.currentBase)
	{
		BigNumber first, second;
		if(this->currentBase==10)
		{
			first = *this;
			second = other.getDecimal();
		}
		else
		{
			first = this->getDecimal();
			second = other;
		}
		return first/second;
	}
	//��� ��� ������ �����, ����������� ��������� ������� ����, �� ��������� �� ����� ���
	//���� ���� ����� � ������� �� precision
	if(!this->isInteger()||!other.isInteger())
	{
		int fNum, sNum;
		int prec1, prec2;
		const char* first_arr = buildArr(*this, fNum, prec1);
		const char* second_arr = buildArr(other, sNum, prec2);
		int fPadding = BigNumber::precision;
		int sPadding = 0;
		if(prec1>prec2)
		{
			sPadding = prec1-prec2;
		}
		else
		{
			fPadding += (prec2-prec1);
		}
		BigNumber a(first_arr,fNum,fPadding,this->currentBase),
			b(second_arr,sNum,sPadding,this->currentBase);
		BigNumber res_cand = a/b;
		res_cand.clean_digits_bin();
		res_cand.sign = this->sign*other.sign;
		delete [] first_arr;
		delete [] second_arr;
		return buildNumberFromStr(res_cand, (res_cand.numberOfDigits==0?res_cand.numberOfDigits2:res_cand.numberOfDigits) , precision);
	}
	BigNumber remainder;
	BigNumber quotient = divideNumbers(abs(*this),abs(other),remainder,this->currentBase);
	quotient.currentBase = this->currentBase;
	if(quotient.sign!=0)
		quotient.sign = this->sign*other.sign;
	return quotient;
}

BigNumber& BigNumber::operator/=(BigNumber const& other)
{
	*this = *this/other;
	return *this;
}

BigNumber BigNumber::operator%(BigNumber const& other) const
{
	if(this->isZero())
	{
		return BigNumber();
	}
	if(other.isZero())
	{
		cerr<<"Division by zero!"<<endl;
		return BigNumber();
	}
	if(this->currentBase!=other.currentBase)
	{
		BigNumber first, second;
		if(this->currentBase==10)
		{
			first = *this;
			second = other.getDecimal();
		}
		else
		{
			first = this->getDecimal();
			second = other;
		}
		return first/second;
	}
	if(!this->isInteger()||!other.isInteger())
	{
		cerr<<"Operation % is only applicable to integers!"<<endl;
		return BigNumber();
	}
	//���������� �����, �� ��������� �� ��������� �� ��������� �� ��������� �������
	BigNumber remainder;
	divideNumbers(abs(*this),abs(other),remainder,this->currentBase);
	remainder.currentBase = this->currentBase;
	remainder.sign = 1;
	return remainder;
}

BigNumber& BigNumber::operator%=(BigNumber const& other)
{
	if(*this!=0&&other!=0)
	{
		*this = *this%other;
	}
	return *this;
}

//�������� �� �����
BigNumber BigNumber::abs(BigNumber const& num) const
{
	if(num.isNegative())
	{
		BigNumber tmp(num);
		tmp.sign = 1;
		return tmp;
	}
	return num;
}

//����������� �� ��������� ������� ��������� �������
bool BigNumber::operator>=(BigNumber const& other) const
{
	return this->compare(other)>=0;
}

bool BigNumber::operator<=(BigNumber const& other) const
{
	return this->compare(other)<=0;
}

bool BigNumber::operator<(BigNumber const& other) const
{
	return this->compare(other)<0;
}
bool BigNumber::operator>(BigNumber const& other) const
{
	return this->compare(other)>0;
}
bool BigNumber::operator==(BigNumber const& other) const
{
	return this->compare(other)==0;
}
bool BigNumber::operator!=(BigNumber const& other) const
{
	return this->compare(other)!=0;
}

//�� ���������� � ��������� ������ ������� ���������� ������� �� ������ �� ������
//� result ���������� ����������
void BigNumber::convertToDecimal()
{
	if(currentBase==10)
	{
		return;
	}
	int currInd = numberOfDigits2-1;
	BigNumber mult(16),
		result(get_value(digits_bin[currInd--]));
	for(;currInd>=0;currInd--)
	{
		result = result*mult + get_value(digits_bin[currInd]);
	}
	char* m_digits = new char[result.numberOfDigits + 1];
	for(int i=0;i<result.numberOfDigits;i++)
		m_digits[i] = result.digits[i];
	m_digits[result.numberOfDigits] = '\0';
	setDigits10(m_digits,result.numberOfDigits,true,false);
	this->currentBase = 10;
}

//���������� � 16 �������, ���� ���������� ����������� ����� - �������������� ��������� �������� � ����� �� 16
void BigNumber::convertToBase()
{
	if(currentBase==16)
		return;
	int base = 16;
	BigNumber m(1);
	BigNumber cpy;
	//��� ����� ������ �����, ���������� ���� ������ ����
	if(!this->isInteger())
	{
		cerr<<"This is a fractional number! Only whole part will be converted."<<endl;
		cpy = abs_floor(*this);
	}
	else
	{
		cpy = abs(*this);
	}
	//���������� ����� �� �������, ���� ������� 16 �� ������, ������ �� �� �������
	int num_dig=0;
	while(m<*this)
	{
		num_dig++;
		m*=base;
	}
	char* m_digits_bin = new char[num_dig+2];
	int currInd = 0;
	BigNumber divisor(base), remainder;
	BigNumber tmp;
	while(cpy>0)
	{
		cpy = divideNumbers(cpy,divisor,remainder,10);
		m_digits_bin[currInd++] = get_char(remainder.digits, remainder.numberOfDigits);
	}
	m_digits_bin[currInd] = '\0';
	setDigits16(m_digits_bin,currInd,true,false);
	this->currentBase = base;
}

//�������, ����� ��������� ����������� �� ������� �� ����� � ����� ������ �������
BigNumber BigNumber::getHex() const
{
	BigNumber tmp = *this;
	tmp.convertToBase();
	return tmp;
}

BigNumber BigNumber::getDecimal() const
{
	BigNumber tmp = *this;
	tmp.convertToDecimal();
	return tmp;
}

//������� ���������, ���������� ������� �������, �� �� �������� ������� �������� ����� �������
BigNumber BigNumber::operator&(BigNumber const& other) const
{
	char (*op)(char,char) = &op_mult;
	BigNumber tmp = applyBinOp(*this,other,op);
	if(this->currentBase==10)
	{
		tmp.convertToDecimal();
	}
	return tmp;
}

BigNumber& BigNumber::operator&=(BigNumber const& other)
{
	BigNumber tmp = this->operator&(other);
	*this = tmp;
	return *this;
}

BigNumber BigNumber::operator|(BigNumber const& other) const
{
	char (*op)(char,char) = &op_sum;
	BigNumber tmp = applyBinOp(*this,other,op);
	if(this->currentBase==10)
	{
		tmp.convertToDecimal();
	}
	return tmp;
}

BigNumber& BigNumber::operator|=(BigNumber const& other)
{
	BigNumber tmp = this->operator|(other);
	*this = tmp;
	return *this;
}

BigNumber BigNumber::operator^(BigNumber const& other) const
{
	char (*op)(char,char) = &op_pow;
	BigNumber tmp = applyBinOp(*this,other,op);
	if(this->currentBase==10)
	{
		tmp.convertToDecimal();
	}
	return tmp;
}

BigNumber& BigNumber::operator^=(BigNumber const& other)
{
	BigNumber tmp = this->operator^(other);
	*this = tmp;
	return *this;
}

BigNumber BigNumber::operator~() const
{
	int len;
	char* bin = getBinary(*this,len);
	for(int i=0;i<len;i++)
	{
		if(bin[i]=='0')
			bin[i]='1';
		else
			bin[i]='0';
	}
	BigNumber res = fromBinary(bin,len);
	if(this->currentBase==10)
	{
		res.convertToDecimal();
	}
	return res;
}

//�������� shift �� �����, �� �� ��������� �������������
BigNumber BigNumber::operator<<(int _shift) const
{
	int len=0;
	char* str = getBinary(*this,len);
	shift(str,len,_shift);
	BigNumber tmp = fromBinary(str,len);
	if(this->currentBase==10)
	{
		tmp.convertToDecimal();
	}
	return tmp;
}
BigNumber& BigNumber::operator<<=(int _shift)
{
	BigNumber tmp = this->operator<<(_shift);
	*this = tmp;
	return *this;
}

BigNumber BigNumber::operator>>(int _shift) const
{
	int len=0;
	char* str = getBinary(*this,len);
	shift(str,len,-_shift);
	BigNumber tmp = fromBinary(str,len);
	if(this->currentBase==10)
	{
		tmp.convertToDecimal();
	}
	return tmp;
}

BigNumber& BigNumber::operator>>=(int _shift)
{
	BigNumber tmp = this->operator>(_shift);
	*this = tmp;
	return *this;
}

//����������, ������ ������� �� ������ ������� ������ ����� � ������ ������ �������
char& BigNumber::operator[](int index) 
{
	if(this->currentBase==10)
	{
		if(index>=0&&index<this->numberOfDigits)
		{
			return digits[numberOfDigits-1-index];
		}
	}
	if(this->currentBase==16&&index<this->numberOfDigits2&&index>=0)
	{
		return digits_bin[numberOfDigits2-1-index];
	}
	cerr<<"Index out of range!"<<endl;
	
}

//����� ������ ���� �� �����
BigNumber BigNumber::abs_floor(BigNumber const& num) const
{
	BigNumber res;
	char* digits = new char[num.numberOfDigits+1];
	for(int i=0;i<num.numberOfDigits;i++)
		digits[i] = num.digits[i];
	digits[num.numberOfDigits] = '\0';
	res.setDigits10(digits,num.numberOfDigits,true,true);

	res.sign = 1;
	res.currentBase = 10;
	return res;
}
//��������� ����� ����� �� �� ����� �� ����� �� ������� � ��������� ����������� �������
int BigNumber::countDigits(long number, int base) const
{
	if(number==0)
		return 1;
	if(number<0)
		number =-number;
	int count=0;
	while(number!=0)
	{
		number/=base;
		count++;
	}
	return count;
}


void BigNumber::buildFromString(const char* str, int _sign, int base)
{
	currentBase = base;
	sign = _sign;
	void (BigNumber::*setDigits)(char*,int,bool,bool);
	void (BigNumber::*cleanDigits)();
	void (BigNumber::*setFraction)(char*,int,bool);
	void (BigNumber::*cleanFraction)();
	void (BigNumber::*cleanIfInt)();
	if(base==16)
	{
		setDigits = &BigNumber::setDigits16;
		cleanDigits = &BigNumber::clean_digits;
		setFraction = &BigNumber::setFraction16;
		cleanFraction = &BigNumber::clean_frac;
		cleanIfInt = &BigNumber::clean_frac_bin;
		
	}
	if(base==10)
	{
		setDigits = &BigNumber::setDigits10;
		cleanDigits = &BigNumber::clean_digits_bin;
		setFraction = &BigNumber::setFraction10;
		cleanFraction = &BigNumber::clean_frac_bin;
		cleanIfInt = &BigNumber::clean_frac;
	}
	buildString(str,setDigits,cleanDigits,setFraction,cleanFraction,cleanIfInt);
}

void BigNumber::buildFromNumber(long number, int padding, int base)
{
	currentBase = base;
	if(number==0)
	{
		buildZero();
		return;
	}
	sign = 1;
	if(number<0)
	{
		number = -number;
		sign = -1;
	}
	buildNumber(number,base,padding);
}

//������� �� ��������, ���������� strcpy
void BigNumber::copyFrom(BigNumber const& other, int padding)
{
	if(other.digits!=NULL)
	{
		digits = new char[other.numberOfDigits+padding+1];
		for(int i=0;i<padding;i++)
			digits[i] = '0';
		strcpy(digits + padding,other.digits);
		numberOfDigits = other.numberOfDigits + padding;
	}
	if(other.digits_bin!=NULL)
	{
		digits_bin = new char[other.numberOfDigits2+padding+1];
		for(int i=0;i<padding;i++)
			digits_bin[i] = '0';
		strcpy(digits_bin + padding,other.digits_bin);
		numberOfDigits2 = other.numberOfDigits2 + padding;
	}
	if(other.fraction!=NULL)
	{
		fraction = new char[other.numberOfFracDigits+1];
		strcpy(fraction,other.fraction);
		numberOfFracDigits = other.numberOfFracDigits;
	}
	if(other.fraction_bin!=NULL)
	{
		fraction_bin = new char[other.numberOfFracDigits2+1];
		strcpy(fraction_bin,other.fraction_bin);
		numberOfFracDigits2 = other.numberOfFracDigits2;
	}
	currentBase = other.currentBase;
	sign = other.sign;
}

//������� ������� �� ���������� �� ���������� ����� �� �����
void BigNumber::clean_digits()
{
	if(this->digits!=NULL)
		delete [] this->digits;
	this->digits = NULL;
	this->numberOfDigits = 0;
}

void BigNumber::clean_digits_bin()
{
	if(this->digits_bin!=NULL)
		delete [] this->digits_bin;
	this->digits_bin = NULL;
	this->numberOfDigits2 = 0;
}

void BigNumber::clean_frac()
{
	if(this->fraction!=NULL)
		delete [] this->fraction;
	this->fraction = NULL;
	this->numberOfFracDigits = 0;
}

void BigNumber::clean_frac_bin()
{
	if(this->fraction_bin!=NULL)
		delete [] this->fraction_bin;
	this->fraction_bin = NULL;
	this->numberOfFracDigits2 = 0;
}

void BigNumber::clean(bool full)
{
	if(full)
	{
		clean_digits();
		clean_digits_bin();
		clean_frac();
		clean_frac_bin();
		return;
	}
	if(digits!=NULL)
		delete [] digits;
	if(digits_bin!=NULL)
		delete [] digits_bin;
	if(fraction!=NULL)
		delete [] fraction;
	if(fraction_bin!=NULL)
		delete [] fraction_bin;
}

//������, ����� �� ������ �� �������� ���������� �� ������������� � �����
//����������� ��������: ���� ��� ���� ���� ����, �.�. �� �� ������� �� ��������,
//� ���� ������������� � ���� � ���� ������ �������, �.�. �� �� ������� �� � �������
void BigNumber::setDigits10(char* digits, int number, bool isInt, bool onlyBase)
{
	clean_digits();
	this->digits = digits;
	this->numberOfDigits = number;
	if(onlyBase)
	{
		clean_digits_bin();
	}
	if(isInt)
	{
		clean_frac();
		clean_frac_bin();
	}
}

void BigNumber::setDigits16(char* digits, int number, bool isInt, bool onlyBase)
{
	clean_digits_bin();
	this->digits_bin = digits;
	this->numberOfDigits2 = number;
	if(onlyBase)
	{
		clean_digits();
	}
	if(isInt)
	{
		clean_frac();
		clean_frac_bin();
	}
}

void BigNumber::setFraction10(char* frac, int number, bool onlyBase)
{
	clean_frac();
	this->fraction = frac;
	this->numberOfFracDigits = number;
	if(onlyBase)
	{
		clean_frac_bin();
	}

}

void BigNumber::setFraction16(char* digits, int number, bool isOnly)
{
	clean_frac_bin();
	this->fraction_bin = digits;
	this->numberOfFracDigits2 = number;
	if(isOnly)
	{
		clean_frac();
	}
}

void BigNumber::buildZero()
{
	char* zero = new char[2];
	zero[0] = '0';
	zero[1] = '\0';
	sign = 0;

	setDigits10(zero,1);
	clean_digits_bin();
	clean_frac();
	clean_frac_bin();
	currentBase = 10;
}

//����������� ����� �� �������
void BigNumber::buildNumber(long number, int base, int padding,bool reversed)
{
	char * storage = new char[countDigits(number,base) + padding + 1];
	for(int i=0;i<padding;i++)
		storage[i] = '0';
	int index = padding;
	long cpy = number;
	while(number!=0)
	{
		storage[index++] = get_char(number%base);
		number/=base;
	}
	storage[index] = '\0';
	if(reversed)
	{
		char* tmp = new char[index+1];
		for(int i=0;i<index;i++)
		{
			tmp[index - i -1] = storage[i];
		}
		tmp[index] = '\0';
		if(base==10)
		{
			setFraction10(tmp,index,true);
		}
		if(base==16)
		{
			setFraction16(tmp,index,true);
		}
		delete [] storage;
		return;
	}
	
	if(base==10)
	{
		setDigits10(storage,index,false,true);
	}
	else
	{
		setDigits16(storage,index,false,true);
	}
}

void BigNumber::buildString(const char* str,void (BigNumber::*setDigits)(char*,int,bool,bool), void (BigNumber::*cleanDigits)(),
							void (BigNumber::*setFraction)(char*,int,bool),void (BigNumber::*cleanFraction)(),
							void (BigNumber::*cleaIfInt)())
{
	//��������� �� ����������� ����� � ����������� ����� ����� �� ����
	const char* div = strchr(str,'.');
	int dif = strlen(str);
	if(div!=NULL)
	{
		dif = div - str;
	}
	
	//������������ ������ ���� � ������� ���
	char* storage_digits = new char[dif+1];
	for(int i=dif-1;i>=0;i--)
	{
		storage_digits[dif-1-i] = str[i];
	}
	storage_digits[dif] = '\0';
	(this->*setDigits)(storage_digits,dif,false,false);

	//��� ��� ������ ����, � ������������
	if(div!=NULL)
	{
		int dif2 = strlen(div+1);
		char* storage_fraction = new char[dif2+1];
		strncpy(storage_fraction,div+1,dif2+1);
		(this->*setFraction)(storage_fraction,dif2,false);
	}
	else
	{
		(this->*cleaIfInt)();
	}

	(this->*cleanDigits)();
	(this->*cleanFraction)();
}

//�� �������� �� ����� ���������� ��������� ��������� � ������ - ����� �� �����, 
//������ �������, � ���������� init e ��� ����� ������ �� �������� ����
int BigNumber::addNumbers(char*& target, const char* first, int fLen, 
						  const char* second, int sLen, int base, int init) const
{
	target = new char[max(fLen,sLen)+2];
	int index = 0;
	int carries = init;
	while(index<fLen&&index<sLen)
	{
		int candidate = get_value(first[index]) + get_value(second[index]) + carries;
		if(candidate>=base)
		{
			carries = candidate/base;
			candidate%=base;
		}
		else
		{
			carries = 0;
		}
		target[index++] = get_char(candidate);
	}
	while(index<fLen)
	{
		int candidate = get_value(first[index]) + carries;
		if(candidate>=base)
		{
			carries = candidate/base;
			candidate%=base;
		}
		else
		{
			carries = 0;
		}
		target[index++] = get_char(candidate);
	}
	while(index<sLen)
	{
		int candidate = get_value(second[index]) + carries;
		if(candidate>=base)
		{
			carries = candidate/base;
			candidate%=base;
		}
		else
		{
			carries = 0;
		}
		target[index++] = get_char(candidate);
	}
	if(carries>0)
		target[index++] = get_char(carries);
	target[index] = '\0';
	return index;
}

//��������� - ������������� ��������� ����������� ����, �� �� ����� �� ��-������� �� ����� ��-������
//��������� ���������� ��������� � �������
int BigNumber::subtractNumbers(char*& target, const char* first, int fLen,
							   const char* second, int sLen, int base, int init) const
{
	//�������� �� ����������� ����, �� ��-�������� ������� �� � �� ������
	target = new char[fLen+1];
	int index = 0,
		carries = init;
	while(index<sLen)
	{
		int fNumber = get_value(first[index]) - carries,
			sNumber = get_value(second[index]);
		if(fNumber < sNumber)
		{
			carries = 1;
			fNumber += base;
		}
		else
		{
			carries = 0;
		}
		target[index++] = get_char(fNumber - sNumber);
	}
	while(index<fLen)
	{
		int number = get_value(first[index]) - carries;
		if(number<0)
		{
			carries = 1;
			number +=base;
		}
		else
		{
			carries = 0;
		}
		target[index++] = get_char(number);
	}
	--index;
	//��� �� �������� ���� � �������� �� �������, �� �����
	while(index>=0&&target[index]=='0')
		--index;
	if(index<0)
	{
		index+=2;
		
	}
	else
	{
		++index;
	}
	target[index] = '\0';
	return index;
}

//������ ����� ����������, � ������, �� �������� ������� �������������� �� ������� ��� �� ���������
int BigNumber::addFraction(char*& target, const char* first, int fLen,
						   const char* second, int sLen, int base, int& carries) const
{
	target = new char[max(fLen,sLen)+1];
	target[max(fLen,sLen)] = '\0';
	if(fLen<sLen)
	{
		for(int i=fLen;i<sLen;i++)
			target[i] = second[i];
	}
	else if(sLen<fLen)
	{
		for(int i=sLen;i<fLen;i++)
			target[i] = first[i];
	}
	carries = 0;
	for(int index = min(fLen,sLen)-1;index>=0;index--)
	{
		int candidate = get_value(first[index]) + get_value(second[index]) + carries;
		if(candidate>=base)
		{
			carries = candidate/base;
			candidate%=base;
		}
		else
		{
			carries = 0;
		}
		target[index] = get_char(candidate);
	}
	return max(fLen,sLen);
}

//��������� �� ����� � ������
int BigNumber::removeFraction(char*& target, const char* first, int fLen,
							  const char* second, int sLen, int base, int& carries) const
{
	target = new char[max(fLen,sLen)+1];
	target[max(fLen,sLen)] = '\0';
	carries = 0;
	if(fLen<sLen)
	{
		for(int index=sLen-1;index>=fLen;index--)
		{
			int fNumber = 0,
			sNumber = get_value(second[index]);
			if(fNumber - carries < sNumber)
			{
				carries = 1;
				fNumber += base;
			}
			else
			{
				carries = 0;
			}
			target[index] = get_char(fNumber - sNumber);
		}
	}
	else if(sLen<fLen)
	{
		for(int i=sLen;i<fLen;i++)
			target[i] = first[i];
	}
	int index = min(fLen,sLen)-1;
	while(index>=0)
	{
		int fNumber = get_value(first[index]) - carries,
			sNumber = get_value(second[index]);
		if(fNumber < sNumber)
		{
			carries = 1;
			fNumber += base;
		}
		else
		{
			carries = 0;
		}
		target[index--] = get_char(fNumber - sNumber);
	}
	return max(fLen,sLen);
}

//���������, ����� �������� �� ����, ����� ���� ����� � ������ ��������������, ��� ���� ����� ���� �����
//����� ������� ���������� �� ����� ��� ������� �����
int BigNumber::compare(BigNumber const& other) const
{
	if(this->sign!=other.sign)
	{
		return this->sign - other.sign;
	}
	if(this->sign==0&&other.sign==0)
	{
		return 0;
	}
	if(this->currentBase==other.currentBase)
	{
		if(sign==1)
			return compareMods(other);
		if(sign==-1)
			return -compareMods(other);
	}
	BigNumber first, second;
	if(this->currentBase!=other.currentBase)
	{
		if(this->currentBase==10)
		{
			first = *this;
			second = other.getDecimal();
		}
		else
		{
			first = this->getDecimal();
			second = other;
		}
	}
	if(sign==1)
		return first.compareMods(second);
	if(sign==-1)
		return second.compareMods(first);
	
}

//������� �� ��������� �� ������ ��� ������� ������ �������
//�������� �������������� ��������� � ��������� �mp, ���������� ����� ������
int BigNumber::compareMods(BigNumber const& other) const
{
	if(this->currentBase==10)
	{
		if(numberOfDigits!=other.numberOfDigits)
			return numberOfDigits - other.numberOfDigits;
		if(strcmp(this->digits,other.digits))
			return cmp(this->digits,other.digits,this->numberOfDigits);
		if(this->isInteger()&&other.isInteger())
			return 0;
		if(this->isInteger()&&!other.isInteger())
			return -1;
		if(!this->isInteger()&&other.isInteger())
			return 1;
		return strcmp(this->fraction,other.fraction);
	}
	else
	{
		if(numberOfDigits2!=other.numberOfDigits2)
			return numberOfDigits2 - other.numberOfDigits2;
		if(strcmp(this->digits_bin,other.digits_bin))
			return cmp(this->digits_bin,other.digits_bin,this->numberOfDigits2);
		if(this->isInteger()&&other.isInteger())
			return 0;
		if(this->isInteger()&&!other.isInteger())
			return -1;
		if(!this->isInteger()&&other.isInteger())
			return 1;
		return strcmp(this->fraction_bin,other.fraction_bin);
	}
}

//private ����������� �� �������� ���������� �� ����� �� ������� ������ �� �����
BigNumber::BigNumber(const char* str, int len, int padding, int base)
	:digits(NULL),digits_bin(NULL),
	fraction_bin(NULL),fraction(NULL),
	numberOfDigits(0),numberOfDigits2(0),
	numberOfFracDigits(0),numberOfFracDigits2(0)
{
	if(base==10)
	{
		char* t_digits = new char[len+padding+1];
		t_digits[len + padding] = '\0';
		for(int i=0;i<padding;i++)
			t_digits[i] = '0';
		for(int i=0;i<len;i++)
			t_digits[padding+i] = str[i];
		setDigits10(t_digits,padding+len);
		clean_digits_bin();
	}
	else
	{
		char* t_digits_bin = new char[len+padding+1];
		t_digits_bin[len] = '\0';
		for(int i=0;i<padding;i++)
			t_digits_bin[padding] = '0';
		for(int i=0;i<len;i++)
			t_digits_bin[padding+i] = str[i];
		setDigits16(t_digits_bin,len+padding);
		clean_digits();
	}
	sign = 1;
	clean_frac();
	clean_frac_bin();
	currentBase = base;
}

//������� �������, ���������� ��������� � ����������� ����� - ��������� � ����� � ������
BigNumber BigNumber::multiplyBaseCaseHelper(BigNumber const& first, BigNumber const& second,
											char* BigNumber::*digits, int BigNumber::*numberOfDigits,
											char* BigNumber::*digits_null, int BigNumber::*numberOfDigits_null,
											int base) const
{
	BigNumber res;
	res.*digits = new char[second.*numberOfDigits + 2];
	int carries = 0, index=0;
	int pivot = get_value((first.*digits)[0]);
	while(index<second.*numberOfDigits)
	{
		int val = pivot*get_value((second.*digits)[index]) + carries;
		if(val>=base)
		{
			carries = val/base;
			val%=base;
		}
		else
		{
			carries = 0;
		}
		(res.*digits)[index++] = get_char(val);
	}
	if(carries>0)
	{
		(res.*digits)[index++] = get_char(carries);
	}
	(res.*digits)[index] = '\0';
	res.*numberOfDigits = index;
	res.*digits_null = NULL;
	res.*numberOfDigits_null = 0;
	res.sign = 1;
	res.currentBase = base;
	return res;
}

//���������� ������� �� ������� �� ������������, ����� �������� ������ �� ��������� �����������
BigNumber BigNumber::multiplyNumbers(BigNumber const& first, BigNumber const& second, int base) const
{
	int BigNumber::*numberOfDigits;
	char* BigNumber::*digits;
	if(base==10)
	{
		if(first.numberOfDigits==1)
		{
			return multiplyBaseCaseHelper(first,second,&BigNumber::digits,
				&BigNumber::numberOfDigits, &BigNumber::digits_bin,
				&BigNumber::numberOfDigits2,10);
		}
		if(second.numberOfDigits==1)
		{
			return multiplyBaseCaseHelper(second,first,
				&BigNumber::digits, &BigNumber::numberOfDigits,
				&BigNumber::digits_bin, &BigNumber::numberOfDigits2,
				10);
		}
		digits = &BigNumber::digits;
		numberOfDigits = &BigNumber::numberOfDigits;
	}
	else
	{
		if(first.numberOfDigits2==1)
		{
			return multiplyBaseCaseHelper(first,second,&BigNumber::digits_bin,&BigNumber::numberOfDigits2,
				&BigNumber::digits, &BigNumber::numberOfDigits, 16);
		}
		if(second.numberOfDigits2==1)
		{
			return multiplyBaseCaseHelper(second,first,&BigNumber::digits_bin,&BigNumber::numberOfDigits2,
				&BigNumber::digits, &BigNumber::numberOfDigits, 16);
		}
		digits = &BigNumber::digits_bin;
		numberOfDigits = &BigNumber::numberOfDigits2;
	}
	int m = first.*numberOfDigits,
		n = second.*numberOfDigits;
	int k = m/2, l = n/2;
	BigNumber a(first.*digits + k, m-k,0,base),
		b(first.*digits,k,0,base),
		c(second.*digits + l,n-l,0,base),
		d(second.*digits,l,0,base);
	BigNumber ac = multiplyNumbers(a,c,base);
	BigNumber bc = multiplyNumbers(b,c,base);
	BigNumber ad = multiplyNumbers(a,d,base);
	BigNumber bd = multiplyNumbers(b,d,base);
	return BigNumber(ac,k+l) + BigNumber(ad,k) + BigNumber(bc,l) + bd;
}

//������� �� ������, �������������� ����� ������
//���� ��������� �� ������ � remainder, � ����� ��������� ��������� �������� �� ���������, � ����� ������ ������
BigNumber BigNumber::divideNumbers(BigNumber const& first, BigNumber const& second, BigNumber& remainder, int base) const
{
	char* BigNumber::*digits;
	int BigNumber::*numberOfDigits;
	if(first.compareMods(second)<0)
	{
		BigNumber res;
		res.buildZero();
		remainder = first;
		return res;
	}
	if(base==10)
	{
		digits = &BigNumber::digits;
		numberOfDigits = &BigNumber::numberOfDigits;
	}
	else
	{
		digits = &BigNumber::digits_bin;
		numberOfDigits = &BigNumber::numberOfDigits2;
	}
	BigNumber res;
	char* temp = new char[first.*numberOfDigits - second.*numberOfDigits + 1];
	BigNumber divident = first;
	int index = 0;
	int current = divident.*numberOfDigits - second.*numberOfDigits;
	while(divident>=second||current>=0)
	{
		BigNumber divisor(second,current);
		int count = 0;
		BigNumber tmp;
		while(tmp<=divident)
		{
			count++;
			tmp +=divisor;
		}
		tmp-=divisor;
		count--;
		if(!(count==0&&index==0))
		{
			temp[index++] = get_char(count);
			divident -= tmp;
		}
		--current;
	}
	res.*digits = new char[index + 1];
	for(int i=0;i<index;i++)
		(res.*digits)[i] = temp[index-1-i];
	(res.*digits)[index] = '\0';
	res.*numberOfDigits = index;
	res.sign=1;

	remainder = divident;
	return res;
}

//������� ������� �� ����������� �� 16 ��� �������
void fillIn(char& s1, char& s2, char& s3, char& s4, char input)
{
	char tmp[4];
	int num = get_value(input);
	for(int i=0;i<4;i++)
	{
		tmp[i] = get_char(num%2);
		num/=2;
	}
	s1 = tmp[0];
	s2 = tmp[1];
	s3 = tmp[2];
	s4 = tmp[3];
}

//�������� �������� ����, �� ��������� ����������� �� �������� �� ������������ �� ����� �����
//� 16 � ������, ���������� �� ��� � ������� ���
char* BigNumber::getBinary(BigNumber const& num, int& len) const
{
	BigNumber hex = num.getHex();
	char* bin = new char[4*hex.numberOfDigits2+1];
	int j=0;
	for(int i=0;i<hex.numberOfDigits2;i++)
	{
		fillIn(bin[j],bin[j+1],bin[j+2],bin[j+3],hex.digits_bin[i]);
		j+=4;
	}
	bin[j] = '\0';
	len = j;
	return bin;
}

//������� ������� �� ����, ��� 16-���� ����� �������� �� 4 �������
char fillOut(const char* str)
{
	int val = get_value(str[0]) + get_value(str[1])*2 + 
		get_value(str[2])*4 + get_value(str[3])*8;
	return get_char(val);
}

//������� �� ������ �� 4 ������� � ���� ���� �� ����� �������� � �������� ��� ����������� 
//����� � 16-���� ������ �������
BigNumber BigNumber::fromBinary(const char* num, int len) const
{
	int k = len/4;
	char* digits = new char[k+2];
	int j = 0;
	for(int i=0;i<k;i++)
	{
		digits[i] = fillOut(num+j);
		j+=4;
	}
	if(j<len)
	{
		char tmp[4];
		for(int i = 0;i<len-j;i++)
			tmp[i] = num[j+i];
		for(int i=len-j;i<4;i++)
			tmp[i] = '0';
		digits[k] = fillOut(tmp);
		digits[k+1] = '\0';
		len = k+1;
	}
	else
	{
		digits[k] = '\0';
		len = k;
	}
	BigNumber res;
	res.currentBase = 16;
	res.setDigits16(digits,len,true,true);
	res.sign = 1;
	delete [] num;
	return res;
}



//����� ��������� ������������, ������� ���������� � �������� ����� �� ������������� �����
BigNumber BigNumber::applyBinOp(BigNumber const& f, BigNumber const& s, char (*op)(char,char)) const
{
	int fLen, sLen;
	const char* first = getBinary(f, fLen);
	const char* second = getBinary(s, sLen);
	int resLen = max(fLen,sLen);
	char* third = new char[resLen+1];
	int i;
	for(i=0;i<fLen&&i<sLen;i++)
	{
		third[i] = op(first[i],second[i]);
	}
	//��� ������� � ��������� ������� � ����
	while(i<fLen)
	{
		third[i] = op(first[i],0);
	}
	while(i<sLen)
	{
		third[i] = op(0,second[i]);
	}
	third[resLen] = '\0';
	delete [] first;
	delete [] second;
	return fromBinary(third,resLen);
}

//��������� ������� ����� �� �������� �������� �� ������ �� �����
char* BigNumber::buildArrHelper(char*& res, const char* fraction, const char* digits,
					int& size, int numberOfDigits,
					int numberOfFracDigits, int& precision) const
{
	res = new char[numberOfDigits + numberOfFracDigits+1];
	size = 0;
	for(;size<numberOfFracDigits;size++)
	{
		res[size] = fraction[numberOfFracDigits - 1 - size];
	}
	strncpy(res + size, digits,numberOfDigits);
	size += numberOfDigits;
	res[size] = '\0';
	precision = numberOfFracDigits;
	return res;
}

//����������� �����, ������� ������� �� ������ � �������� ����
char* BigNumber::buildArr(BigNumber const& number, int& size, int& precision) const
{
	char* res;
	if(number.currentBase==10)
	{
		return buildArrHelper(res,number.fraction,number.digits,
			size, number.numberOfDigits, number.numberOfFracDigits, precision);
	}
	return buildArrHelper(res,number.fraction_bin,number.digits_bin,
		size, number.numberOfDigits2, number.numberOfFracDigits2, precision);
}

//�� ��������� ��� ������ � ������� ������� �� ����������� �������
//���������� �������
void BigNumber::buildNumberFromStrHelper(const char* source, void (BigNumber::*setDigits)(char*,int,bool,bool),
							void (BigNumber::*setFraction)(char*,int,bool),int size, int precision)
{
	char* fraction = new char[precision+1];
	for(int i=0;i<precision;i++)
	{
		fraction[i] = source[precision-1-i];
	}
	fraction[precision] = '\0';
	int tmp = precision;
	while(fraction[precision-1]=='0')
		--precision;
	fraction[precision] = '\0';
	(this->*setFraction)(fraction,precision,true);

	if(size==tmp)
	{
		char* dig = new char[2];
		dig[0] = '0';
		dig[1] = '\0';
		(this->*setDigits)(dig,2,false,true);
		return;
	}
	char* m_digits = new char[size - tmp + 1];
	for(int i=0;i<size - tmp;i++)
	{
		m_digits[i] = source[tmp+i];
	}
	m_digits[size-tmp] = '\0';
	(this->*setDigits)(m_digits,size-tmp,false,true);
}

BigNumber BigNumber::buildNumberFromStr(BigNumber const& decimal, int size, int precision) const
{
	BigNumber res;

	if(decimal.digits!=NULL)
	{
		res.buildNumberFromStrHelper(decimal.digits, &BigNumber::setDigits10, &BigNumber::setFraction10, size, precision);
		res.currentBase = 10;
	}
	if(decimal.digits_bin!=NULL)
	{
		res.buildNumberFromStrHelper(decimal.digits_bin, &BigNumber::setDigits16, &BigNumber::setFraction16, size, precision);
		res.currentBase = 16;
	}
	res.sign = decimal.sign;
	return res;
}

