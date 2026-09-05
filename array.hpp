#include <cstdint>

template <typename T>
class dynArray {
    public:
        size_t   elementCount;
        size_t   elementWidth = sizeof(T);

        uint8_t* arrPointer;

};