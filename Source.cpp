#include <iostream>
#include "BigNumber.h"
using namespace std;
int BigNumber::precision;
void compare()
{
	BigNumber a(123.23,2),
		b(4323.235,3);
	cout<<(a<b)<<endl;

	BigNumber c(10000),d(0);
	cout<<c/d<<endl;

	c = 40067;
	d = 2;
	cout<<c/d<<endl;

	c = 400607;
	cout<<c/d<<endl;

	c = 10000;
	d/=2;
	cout<<c/d<<endl;

	c = 4500;
	d = 45;
	cout<<c/d<<endl;

	cin>>a;
	cout<<a*b<<endl;

	c = 2;
	d = BigNumber(2.5,1);
	cout<<c+d<<endl;
}

void test_Binary()
{
	BigNumber a(134),
		b(25);
	cout<<(a&b)<<endl;
	cout<<(a|b)<<endl;
	cout<<(a^b)<<endl;
	cout<<(~a)<<endl;
	cout<<(a>>2)<<endl;
	cout<<(a>>4)<<endl;
	cout<<(b<<3)<<endl;

}

void test_Conversions()
{
	BigNumber a(1245449);
	cout<<a.getHex()<<endl;

	BigNumber b(244324,0,16);
	cout<<b.getDecimal()<<endl;
	cout<<a[3]<<endl;
}

void test_Hex()
{
	BigNumber a(13534,0,16),
		b(-983,0,16);
	
	cout<<a+b<<endl;
	cout<<a-b<<endl;
	cout<<b-a<<endl;

	cout<<a*b<<endl;
	cout<<a/b<<endl;
	cout<<b/a<<endl;
	cout<<a%b<<endl;
	cout<<b%a<<endl;

}

void test_Arithmetics()
{
	BigNumber a(12354),
		b(2454656);

	cout<<a+b<<endl;
	cout<<a-b<<endl;
	cout<<b-a<<endl;

	BigNumber c(138234),
		d(-2344334);
	cout<<c+d<<endl;
	cout<<c-d<<endl;
	cout<<d-c<<endl;

	BigNumber f(132434.2323,4),
		g(23232.244,3);
	cout<<f+g<<endl;
	cout<<f-g<<endl;
	cout<<g-f<<endl;

	f = BigNumber(2.4531,4);
	g = BigNumber(-3.789,3);
	cout<<f+g<<endl;
	cout<<f-g<<endl;
	cout<<g-f<<endl;

	cout<<a*b<<endl;
	cout<<a/b<<endl;
	cout<<b/a<<endl;
	cout<<b%a<<endl;

	cout<<c*d<<endl;
	cout<<d/c<<endl;
	cout<<d%c<<endl;

	cout<<f*g<<endl;
	cout<<f/g<<endl;
	cout<<g/f<<endl;

	BigNumber five(5),
		two(2.5,1);
	cout<<five*two<<endl;
	cout<<five/two<<endl;
}

void test_Constructors()
{
	BigNumber a;
	cout<<a<<' ';

	BigNumber b(-124),
		c(2454l,3),
		d(-116l,1,16);

	cout<<b<<' '<<c<<' '<<d<<endl;
	
	BigNumber f(2.146,3),
		g(3.16794,5);

	BigNumber h(g);

	BigNumber b1(b,12);
	cout<<b1<<endl;
	f = -d;

	cout<<f<<' '<<h<<endl;

	cin>>d;
	cout<<d;

}

void testBigNumber()
{
	BigNumber n((long)12,1,10);
	cout<<n<<endl;

	BigNumber p(2.35468,3);
	cout<<p<<endl;

	cin>>p;
	cout<<p<<endl;

	BigNumber m((long)123),
		q((long)24646574);
	BigNumber hex(170);
	cout<<hex.getHex()<<endl;
	hex.convertToBase();
	BigNumber hex2((long)0x1A,0,16);
	cout<<hex2<<endl;
	hex2.convertToDecimal();
	cout<<hex2<<endl;
	hex.convertToDecimal();
	cout<<hex<<endl;
	cout<<(-q)/m<<endl;
	cout<<(-q)%m<<endl;
	
	BigNumber a(3.25,2),
		b(2.6,1);
	cout<<a/b<<endl;
	cout<<m+q<<endl;
	cout<<q-m<<endl;
	cout<<a+b<<endl;
	cout<<b-a<<endl;
	cout<<a-b<<endl;
	cout<<a-a<<endl;
	cout<<m*q<<endl;
	q*=m;
	cout<<q<<endl;
	cout<<q*q<<endl;
	cout<<a*b<<endl;
	cout<<(a-a)*b<<endl;

	BigNumber twtw(144),
		tw(12);
	cout<<twtw[1]<<endl;
	cout<<twtw%6<<endl;
	twtw/=tw;
	cout<<twtw<<endl;

	BigNumber six(6);
	cout<<(twtw&six)<<endl;
	cout<<(twtw|six)<<endl;
	cout<<(twtw^six)<<endl;
	cout<<(~twtw)<<endl;
	cout<<(twtw<<2)<<endl;
	cout<<(twtw>>2)<<endl;
	cout<<(twtw>>4)<<endl;
}

int main()
{
	BigNumber a;
	a.setPrecision(5);
	test_Constructors();
	cout<<"\n\n\n";
	testBigNumber();
	cout<<"\n\n\n";
	test_Arithmetics();
	cout<<"\n\n\n";
	test_Hex();
	cout<<"\n\n\n";
	test_Conversions();
	cout<<"\n\n\n";
	test_Binary();
	cout<<"\n\n\n";
	compare();
	return 0;
}