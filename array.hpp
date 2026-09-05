#include <cstdint>
#include <cstdlib>
#include <bit>
#include <memory>

template <typename T>
class dynArray {
    public:
        size_t   size;
        size_t   width;

        uint8_t* data;


    dynArray(T v, size_t count) {
        size = count;
        width = sizeof(v);

        size_t bytes = width * count;
        size_t capacity = std::bit_ceil(bytes);


        data = (uint8_t*)malloc(capacity);
        T* arr = (T*)data;
        for (size_t i = 0; i < count; i++) {
            std::construct_at(&arr[i], v);
        }
    }
};