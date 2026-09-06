#include <cstdint>
#include <cstdlib>
#include <bit>
#include <memory>
#include <initializer_list>
#include <stdexcept>

template <typename T>
class dynArray {
    private:
        size_t   size_;
        size_t   width_;
        size_t   capacity_;

        uint8_t* data_;

        void reallocate_(size_t bytes) {
            bytes = std::bit_ceil(bytes);
            void* newData_ = realloc(data_, bytes);

            if (!newData_) throw std::bad_alloc();

            capacity_ = bytes;
            data_ = (uint8_t*)newData_;
        }

    public:
        dynArray(T v, size_t count) {
            size_ = count;
            width_ = sizeof(T);

            size_t bytes = width_ * count;
            capacity_ = std::bit_ceil(bytes);

            data_ = (uint8_t*)malloc(capacity_);
            T* arr = (T*)data_;

            for (size_t i = 0; i < count; i++) std::construct_at(&arr[i], v);
        }

        dynArray() {
            size_ = 0;
            width_ = sizeof(T);
            capacity_ = 0;
            data_ = nullptr;
        }

        ~dynArray() {
            T* arr = (T*)data_;

            for (size_t i = 0; i < size_; i++) std::destroy_at(&arr[i]);

            free(data_);
        }

        T& operator[](size_t index) {
            if (index >= size_) throw std::out_of_range("index out of range");
            
            T* arr = (T*)data_;
            return arr[index];
        }

        dynArray(std::initializer_list<T> values) {
            size_ = values.size();
            width_ = sizeof(T);

            size_t bytes = size_ * width_;
            capacity_ = std::bit_ceil(bytes);
            data_ = (uint8_t*)malloc(capacity_);

            T* arr = (T*)data_;

            size_t i = 0;
            for (const T& value : values) {
                std::construct_at(&arr[i], value);
                i++;
            }
        }

        dynArray& operator=(const T* arr) {
            for (size_t i = 0; i < size_; i++) std::destroy_at((T*)data_ + i);

            T* dest = (T*)data_;
            for (size_t i = 0; i < size_; i++) std::construct_at(&dest[i], arr[i]);

            return *this;
        }

        void reserve(size_t count) {
            if (count * width_ <= capacity_) return;

            reallocate_(count * width_);
        }

        void push_back(const T& value) {
            if (size_ == capacity_ / width_) reserve(size_* 2 + 1);

            T* arr = (T*)data_;
            std::construct_at(&arr[size_], value);

            size_++;
        }

        template <typename... Args>
        T& emplace_back(Args&&... args) {
            if (size_ == capacity_ / width_) reserve(size_ * 2 + 1);

            T* arr = (T*)data_;
            std::construct_at(&arr[size_], std::forward<Args>(args)...);

            return arr[size_++];
        }

        void resize(size_t count) {
            if (count < size_) {
                T* arr = (T*)data_;

                for (size_t i = count; i < size_; i++) std::destroy_at(&arr[i]);

                size_ = count;
            } else if (count > size_) {
                reserve(count);

                T* arr = (T*)data_;
                for (size_t i = size_; i < count; i++) std::construct_at(&arr[i]);

                size_ = count;
            }
        }

        size_t size() {
            return size_;
        }
        
        size_t capacity() {
            return capacity_;
        }
};