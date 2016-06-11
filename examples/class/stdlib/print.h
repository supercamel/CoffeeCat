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
    

    
};

extern Printer io;

inline void print(auto t)
{
    io.print(t);
}


#endif

