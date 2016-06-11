#ifndef COFFEECAT_PRINT_H
#define COFFEECAT_PRINT_H

#include <etk/etk.h>
#include <iostream>

constexpr uint32 max_line_length = 128;


class Printer : public etk::Stream<Printer>
{
public:
    void put(char c)
    {
        std::cout << c << std::flush;
    }
    
	etk::StaticString<max_line_length> get_line()
	{
		char buf[max_line_length];
		std::cin.getline (buf, max_line_length);
		return buf;
	}
    
};

extern Printer io;

inline void print(auto t)
{
    io.print(t);
}


#endif

