#include <cstdint>
#include <cstdlib>
#include <bit>
#include <memory>
#include <initializer_list>
#include <stdexcept>

template <typename T>
class dynArray {
    private:
        size_t   size;
        size_t   width;
        size_t   capacity;

        uint8_t* data;

        void reallocate_(size_t bytes) {
            bytes = std::bit_ceil(bytes);
            void* newData = realloc(data, bytes);

            if (!newData) throw std::bad_alloc();

            capacity = bytes;
            data = (uint8_t*)newData;
        }

    public:
        dynArray(T v, size_t count) {
            size = count;
            width = sizeof(T);

            size_t bytes = width * count;
            capacity = std::bit_ceil(bytes);

            data = (uint8_t*)malloc(capacity);
            T* arr = (T*)data;

            for (size_t i = 0; i < count; i++) std::construct_at(&arr[i], v);
        }

        dynArray() {
            size = 0;
            width = sizeof(T);
            capacity = 0;
            data = nullptr;
        }

        ~dynArray() {
            T* arr = (T*)data;

            for (size_t i = 0; i < size; i++) std::destroy_at(&arr[i]);

            free(data);
        }

        T& operator[](size_t index) {
            if (index >= size) throw std::out_of_range("index out of range");
            
            T* arr = (T*)data;
            return arr[index];
        }

        dynArray(std::initializer_list<T> values) {
            size = values.size();
            width = sizeof(T);

            size_t bytes = size * width;
            capacity = std::bit_ceil(bytes);
            data = (uint8_t*)malloc(capacity);

            T* arr = (T*)data;

            size_t i = 0;
            for (const T& value : values) {
                std::construct_at(&arr[i], value);
                i++;
            }
        }

        dynArray& operator=(const T* arr) {
            for (size_t i = 0; i < size; i++) std::destroy_at((T*)data + i);

            T* dest = (T*)data;

            for (size_t i = 0; i < size; i++) std::construct_at(&dest[i], arr[i]);

            return *this;
        }

        void reserve(size_t count) {
            if (count * width <= capacity) return;

            reallocate_(count * width);
        }

        void push_back(const T& value) {
            if (size == capacity / width) reserve(size * 2 + 1);

            T* arr = (T*)data;
            std::construct_at(&arr[size], value);

            size++;
        }

        template <typename... Args>
        T& emplace_back(Args&&... args) {
            if (size == capacity / width) reserve(size * 2 + 1);

            T* arr = (T*)data;

            std::construct_at(&arr[size], std::forward<Args>(args)...);

            return arr[size++];
        }

        void resize(size_t count) {
            if (count < size) {
                T* arr = (T*)data;

                for (size_t i = count; i < size; i++) std::destroy_at(&arr[i]);

                size = count;
            } else if (count > size) {
                reserve(count);

                T* arr = (T*)data;

                for (size_t i = size; i < count; i++) std::construct_at(&arr[i]);

                size = count;
            }
        }
};