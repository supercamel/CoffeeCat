#ifndef COFFEECAT_ENUM_H
#define COFFEECAT_ENUM_H


struct __Enum
{
	__Enum() { }
	__Enum(int v) { value = v; }

	explicit operator int&() { return value; }

	int operator*(const int q) { return value*q; }
	int operator/(const int q) { return value/q; }
	int operator+(const int q) { return value+q; }
	int operator-(const int q) { return value-q; }
	int operator%(const int q) { return value%q; }

	bool operator==(const int q) { return value==q; }
	bool operator!=(const int q) { return value!=q; }
	bool operator<(const int q) { return value<q; }
	bool operator>(const int q) { return value>q; }
	bool operator<=(const int q) { return value<=q; }
	bool operator>=(const int q) { return value>=q; }


	virtual void inc(const int i = 1) { value += i; }
	virtual void dec(const int i = 1) { value -= i; }

	int value = 0;
};


#endif
