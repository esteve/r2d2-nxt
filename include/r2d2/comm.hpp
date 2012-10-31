#ifndef R2D2_COMM_HPP
#define R2D2_COMM_HPP

#include <cstdint>

class Comm {
public:
    virtual void devWrite(uint8_t *, int) = 0;
    virtual void devRead(uint8_t *, int) = 0;
    virtual bool open() = 0;
};
#endif
