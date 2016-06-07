#ifndef COFFEECAT_PRINT_H
#define COFFEECAT_PRINT_H

#include <ninjaskit/ninjaskit.h>

constexpr uint32 max_line_length = 128;


class Printer : public etk::Stream<Printer>
{
public:
    void put(char c)
    {
        Serial1.put(c);
    }
    

    
};

extern Printer io;

inline void print(auto t)
{
    io.print(t);
}



#endif

