#pragma once

#include <cstddef>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>

template <typename T>
class Vector {
public:
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Vector() noexcept : data_(nullptr), size_(0), capacity_(0) {
    }

    explicit Vector(size_t count) : data_(new T[count]), size_(count), capacity_(count) {
        std::fill_n(data_, size_, T());
    }

    Vector(size_t count, const T& value) : data_(new T[count]), size_(count), capacity_(count) {
        std::fill_n(data_, size_, value);
    }

    Vector(std::initializer_list<T> init) : data_(new T[init.size()]), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), data_);
    }

    Vector(const Vector& other) : data_(new T[other.capacity_]), size_(other.size_), capacity_(other.capacity_) {
        std::copy(other.data_, other.data_ + size_, data_);
    }

    Vector(Vector&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector temp(other);
            swap(temp);
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            Vector temp(std::move(other));
            swap(temp);
        }
        return *this;
    }

    Vector& operator=(std::initializer_list<T> init) {
        Vector temp(init);
        swap(temp);
        return *this;
    }

    ~Vector() {
        clear();
        delete[] data_;
    }

    T& at(size_t pos) {
        if (pos >= size_) {
            throw std::out_of_range("Vector::at");
        }
        return data_[pos];
    }

    const T& at(size_t pos) const {
        if (pos >= size_) {
            throw std::out_of_range("Vector::at");
        }
        return data_[pos];
    }

    T& operator[](size_t pos) noexcept {
        return data_[pos];
    }
    const T& operator[](size_t pos) const noexcept {
        return data_[pos];
    }

    T& front() noexcept {
        return data_[0];
    }
    const T& front() const noexcept {
        return data_[0];
    }

    T& back() noexcept {
        return data_[size_ - 1];
    }
    const T& back() const noexcept {
        return data_[size_ - 1];
    }

    T* data() noexcept {
        return data_;
    }
    const T* data() const noexcept {
        return data_;
    }

    iterator begin() noexcept {
        return data_;
    }
    const_iterator begin() const noexcept {
        return data_;
    }
    const_iterator cbegin() const noexcept {
        return data_;
    }

    iterator end() noexcept {
        return data_ + size_;
    }
    const_iterator end() const noexcept {
        return data_ + size_;
    }
    const_iterator cend() const noexcept {
        return data_ + size_;
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    bool empty() const noexcept {
        return size_ == 0;
    }
    size_t size() const noexcept {
        return size_;
    }
    size_t capacity() const noexcept {
        return capacity_;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            reallocate(new_capacity);
        }
    }

    void shrink_to_fit() {
        if (capacity_ > size_) {
            reallocate(size_);
        }
    }

    void clear() noexcept {
        for (size_t i = 0; i != size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            reserve(capacity_ ? capacity_ * 2 : 1);
        }
        new (data_ + size_) T(value);
        ++size_;
    }

    void push_back(T&& value) {
        if (size_ == capacity_) {
            reserve(capacity_ ? capacity_ * 2 : 1);
        }
        new (data_ + size_) T(std::move(value));
        ++size_;
    }

    template<typename... Args>
    T& emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            reserve(capacity_ ? capacity_ * 2 : 1);
        }
        T* ptr = data_ + size_;
        new (ptr) T(std::forward<Args>(args)...);
        ++size_;
        return *ptr;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }

    void resize(size_t count) {
        if (count > size_) {
            reserve(count);
            for (size_t i = size_; i != count; ++i) {
                new (data_ + i) T();
            }
        } else if (count < size_) {
            for (size_t i = count; i != size_; ++i) {
                data_[i].~T();
            }
        }
        size_ = count;
    }

    void resize(size_t count, const T& value) {
        if (count > size_) {
            reserve(count);
            for (size_t i = size_; i != count; ++i) {
                new (data_ + i) T(value);
            }
        } else if (count < size_) {
            for (size_t i = count; i != size_; ++i) {
                data_[i].~T();
            }
        }
        size_ = count;
    }

    void swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    T* data_;
    size_t size_;
    size_t capacity_;

    void reallocate(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i != size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
            data_[i].~T();
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
};
