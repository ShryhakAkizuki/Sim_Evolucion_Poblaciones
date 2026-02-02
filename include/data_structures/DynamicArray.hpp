#pragma once

#include <cstddef>          // Para std::size_t, std::ptrdiff_t
#include <initializer_list> // Para std::initializer_list
#include <span>             // Para std::span (C++20)
#include <stdexcept>        // Para std::out_of_range (en at())
#include <algorithm>        // Para std::copy, std::move, etc. (en implementaciones)
#include <utility>          // Para std::forward, std::move, etc.
#include <iterator>         // Para std::distance, categorías de iteradores
#include <concepts>

// Tag - reservar memoria
struct Reserve_TAG {};
inline constexpr Reserve_TAG Reserve{};

template<typename T>
class DynamicArray {
public:
    // ----- Aliases -----
    using iterator = T*;
    using const_iterator = const T*;

    using value_type = T;

    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference = T&;
    using const_reference   = const T&;

    using pointer = T*;
    using const_pointer     = const T*;

    // ----- Funciones especiales -----
    DynamicArray();
    DynamicArray(const DynamicArray& other) = delete;
    DynamicArray(DynamicArray&& other) noexcept;
    DynamicArray& operator=(const DynamicArray& other) = delete;
    DynamicArray& operator=(DynamicArray&& other) noexcept;
    ~DynamicArray();

    DynamicArray(size_type capacity, const_reference value);
    explicit DynamicArray(Reserve_TAG, size_type capacity);
    DynamicArray(std::initializer_list<T> init);
    DynamicArray(std::span<T> s);

    template<std::input_iterator It>
    DynamicArray(It first, It last);

    // ----- Acceso de elementos -----
    reference operator[](size_type index);
    const_reference operator[](size_type index) const;
    reference at (size_type index);
    const_reference at (size_type index) const;

    reference front();
    reference back();
    const_reference front() const;
    const_reference back() const;

    pointer data();
    const_pointer data() const;

    // ----- Iteradores -----
    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    iterator insert(const_iterator pos, const_reference value);
    iterator insert(const_iterator pos, T&& value);

    template<std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last);

    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    iterator find(const_reference value);
    const_iterator find(const_reference value) const;

    // ----- Capacidad -----
    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type capacity() const noexcept;

    // ----- Modificacion -----
    void clear();
    void reserve(size_type capacity);
    void shrink_to_fit();
    
    void push_back(const_reference value);
    void push_back(T&& value);

    template<typename... Args>
    reference emplace_back(Args&&... args);

    void pop_back();

    void insert(size_type index, const_reference value);
    void insert(size_type index, T&& value);

    template<typename... Args>
    reference emplace(size_type index, Args&&... args);

    void erase(size_type index);

    // ----- Comparadores -----
    bool operator==(const DynamicArray& other) const;
    bool operator!=(const DynamicArray& other) const;

    // ----- Helpers -----
    void swap(DynamicArray& other) noexcept;

private:
    // ----- Atributos -----
    static constexpr size_type DEFAULT_CAPACITY = 16;
    static constexpr float GROWTH_FACTOR = 1.5f;

    pointer data_ = nullptr;
    
    size_type capacity_ = 0;
    size_type size_ = 0;
};

// ##### Metodos - Publicos #####

// ----- Funciones especiales -----
template<typename T>
DynamicArray<T>::DynamicArray():
data_(nullptr), capacity_(0), size_(0) {}

template<typename T>
DynamicArray<T>::DynamicArray(DynamicArray&& other) noexcept:
data_(other.data_), capacity_(other.capacity_), size_(other.size_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray&& other) noexcept {
    if(this != &other){

        for (size_type i = 0; i < size_; ++i) data_[i].~value_type();
        ::operator delete(data_);

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

template<typename T>
DynamicArray<T>::~DynamicArray() {
    // Destruir elementos construidos
    for (size_type i = 0; i < size_; ++i) {
        data_[i].~value_type();
    }
    // Liberar memoria
    ::operator delete(data_);
}

template<typename T>
DynamicArray<T>::DynamicArray(size_type capacity, const_reference value) :
capacity_(0), size_(0) {

    if (capacity == 0) {
        data_ = nullptr;
        return;
    }

    try {
        data_ = static_cast<pointer>(::operator new(capacity * sizeof(value_type)));

        for(size_ = 0; size_ < capacity; ++size_) new(&data_[size_]) value_type(value);
        capacity_ = capacity;

    } catch (...) {
        for(size_type i = 0; i<size_; ++i) data_[i].~value_type();
        
        ::operator delete(data_);
        data_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        throw;
    }
}

template<typename T>
DynamicArray<T>::DynamicArray(Reserve_TAG, size_type capacity) :
capacity_(capacity), size_(0) {

    if (capacity == 0) {
        data_ = nullptr;
        return;
    }

    try { 
        data_ = static_cast<pointer>(::operator new(capacity * sizeof(value_type)));
    } catch (...) {        
        ::operator delete(data_);
        data_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        throw;
    }
}

template<typename T>
DynamicArray<T>::DynamicArray(std::initializer_list<T> init) :
capacity_(0), size_(0) {
    
    if (init.size() == 0) {
        data_ = nullptr;
        return;
    }

    try {
        data_ = static_cast<pointer>(::operator new(init.size() * sizeof(value_type)));
        
        auto it = init.begin();
        for(size_ = 0; it != init.end(); ++it, ++size_) new(&data_[size_]) value_type(*it);
        capacity_ = init.size();

    } catch (...) {
        for(size_type i = 0; i<size_; ++i) data_[i].~value_type();
        
        ::operator delete(data_);
        data_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        throw;
    }
}

template<typename T>
DynamicArray<T>::DynamicArray(std::span<T> s) :
capacity_(0), size_(0) {

    if (s.empty()) {
        data_ = nullptr;
        return;
    }

    try {
        data_ = static_cast<pointer>(::operator new(s.size() * sizeof(value_type)));
        
        auto it = s.begin();
        for(size_ = 0; it != s.end(); ++it, ++size_) new(&data_[size_]) value_type(*it);
        capacity_ = s.size();

    } catch (...) {
        for(size_type i = 0; i<size_; ++i) data_[i].~value_type();
        
        ::operator delete(data_);
        data_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        throw;
    }
}

template<typename T>
template<std::input_iterator It>
DynamicArray<T>::DynamicArray(It first, It last) :
capacity_(0), size_(0) {
    if (first == last) {
        data_ = nullptr;
        return;
    }

    size_type count = 0;
    {
        It temp = first;
        while (temp != last) {
            ++count;
            ++temp;
        }
    }
    
    if (count == 0) return;


    try {
        data_ = static_cast<pointer>(::operator new(count * sizeof(value_type)));
        
        It current = first;
        for(size_ = 0; current != last; ++current, ++size_) new(&data_[size_]) value_type(*current);
        capacity_ = count;

    } catch (...) {
        for(size_type i = 0; i<size_; ++i) data_[i].~value_type();
        
        ::operator delete(data_);
        data_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        throw;
    }    
}

// ----- Acceso de elementos -----
template<typename T>
DynamicArray<T>::reference DynamicArray<T>::operator[](size_type index) {
    return data_[index];
}

template<typename T>
DynamicArray<T>::const_reference DynamicArray<T>::operator[](size_type index) const {
    return data_[index];
}

template<typename T>
DynamicArray<T>::reference DynamicArray<T>::at(size_type index) {
    if(index >= size_) throw std::out_of_range("DynamicArray::at: index out of range");
    return data_[index];
}

template<typename T>
DynamicArray<T>::const_reference DynamicArray<T>::at(size_type index) const {
    if(index >= size_) throw std::out_of_range("DynamicArray::at: index out of range");
    return data_[index];
}

template<typename T>
DynamicArray<T>::reference DynamicArray<T>::front(){
    if(size_ == 0) throw std::out_of_range("DynamicArray::front: Array without elements");
    return data_[0];
}

template<typename T>
DynamicArray<T>::reference DynamicArray<T>::back(){
    if(size_ == 0) throw std::out_of_range("DynamicArray::back: Array without elements");
    return data_[size_ - 1];
}

template<typename T>
DynamicArray<T>::const_reference DynamicArray<T>::front() const {
    if(size_ == 0) throw std::out_of_range("DynamicArray::front: Array without elements");
    return data_[0];
}

template<typename T>
DynamicArray<T>::const_reference DynamicArray<T>::back() const {
    if(size_ == 0) throw std::out_of_range("DynamicArray::back: Array without elements");
    return data_[size_ - 1];
}

template<typename T>
DynamicArray<T>::pointer DynamicArray<T>::data() {
    return data_;
}

template<typename T>
DynamicArray<T>::const_pointer DynamicArray<T>::data() const {
    return data_;
}

// ----- Iteradores -----
template<typename T>
DynamicArray<T>::iterator DynamicArray<T>::begin() {
    return data_;
}

template<typename T>
DynamicArray<T>::iterator DynamicArray<T>::end() {
    return data_ + size_;
}

template<typename T>
DynamicArray<T>::const_iterator DynamicArray<T>::begin() const {
    return data_;
}

template<typename T>
DynamicArray<T>::const_iterator DynamicArray<T>::end() const {
    return data_ + size_;
}

template<typename T>
DynamicArray<T>::const_iterator DynamicArray<T>::cbegin() const {
    return data_;
}

template<typename T>
DynamicArray<T>::const_iterator DynamicArray<T>::cend() const {
    return data_ + size_;
}

template<typename T>            
DynamicArray<T>::iterator DynamicArray<T>::insert(const_iterator pos, const_reference value) {
    const size_type insert_index = pos - data_;
    if (pos > data_ + size_ || pos < data_) throw std::out_of_range("DynamicArray::insert: position out of range");

    if (size_ + 1 > capacity_) {
        size_type new_capacity = (capacity_ == 0) ? DEFAULT_CAPACITY : 
                                 static_cast<size_type>(capacity_ * GROWTH_FACTOR);
        
        while(new_capacity < size_ + 1) new_capacity = static_cast<size_type>(new_capacity * GROWTH_FACTOR);
    
        pointer new_data = static_cast<pointer>(::operator new(new_capacity * sizeof(value_type)));
        size_type new_size = 0;

        try {
            for (;new_size < insert_index; ++new_size) new(&new_data[new_size]) value_type(std::move_if_noexcept(data_[new_size]));
            
            new(&new_data[new_size++]) value_type(value);
            
            for (size_type i = insert_index; i < size_; ++i) new(&new_data[new_size++]) value_type(std::move_if_noexcept(data_[i]));

        } catch (...) {
            for (size_type i = 0; i < new_size; ++i) new_data[i].~value_type();
            
            ::operator delete(new_data);
            throw;
        }
        
        for (size_type i = 0; i < size_; ++i) data_[i].~value_type();
        
        ::operator delete(data_);
        
        data_ = new_data;
        capacity_ = new_capacity;
        size_ = new_size; 
    } else {
        
        if (insert_index == size_) new(&data_[size_]) value_type(value);
        else {            
            new(&data_[size_]) value_type(std::move_if_noexcept(data_[size_ - 1]));
            
            for (size_type i = size_-1; i > insert_index; --i) data_[i] = std::move_if_noexcept(data_[i - 1]);
                        
            data_[insert_index] = value;
        }
        ++size_;
    }
    
    return data_ + insert_index;
}

template<typename T>            
DynamicArray<T>::iterator DynamicArray<T>::insert(const_iterator pos, T&& value) {
    const size_type insert_index = pos - data_;
    if (pos > data_ + size_ || pos < data_) throw std::out_of_range("DynamicArray::insert: position out of range");

    if (size_ + 1 > capacity_) {
        size_type new_capacity = (capacity_ == 0) ? DEFAULT_CAPACITY : 
                                 static_cast<size_type>(capacity_ * GROWTH_FACTOR);
        
        while(new_capacity < size_ + 1) new_capacity = static_cast<size_type>(new_capacity * GROWTH_FACTOR);
    
        pointer new_data = static_cast<pointer>(::operator new(new_capacity * sizeof(value_type)));
        size_type new_size = 0;

        try {
            for (;new_size < insert_index; ++new_size) new(&new_data[new_size]) value_type(std::move_if_noexcept(data_[new_size]));
            
            new(&new_data[new_size++]) value_type(std::move(value));
            
            for (size_type i = insert_index; i < size_; ++i) new(&new_data[new_size++]) value_type(std::move_if_noexcept(data_[i]));

        } catch (...) {
            for (size_type i = 0; i < new_size; ++i) new_data[i].~value_type();
            
            ::operator delete(new_data);
            throw;
        }
        
        for (size_type i = 0; i < size_; ++i) data_[i].~value_type();
        
        ::operator delete(data_);
        
        data_ = new_data;
        capacity_ = new_capacity;
        size_ = new_size; 
    } else {
        
        if (insert_index == size_) new(&data_[size_]) value_type(std::move(value));
        else {            
            new(&data_[size_]) value_type(std::move_if_noexcept(data_[size_ - 1]));
            
            for (size_type i = size_-1; i > insert_index; --i) data_[i] = std::move_if_noexcept(data_[i - 1]);
                        
            data_[insert_index] = std::move(value);
        }
        ++size_;
    }
    
    return data_ + insert_index;
}

template<typename T>   
template<std::input_iterator InputIt>
DynamicArray<T>::iterator DynamicArray<T>::insert(const_iterator pos, InputIt first, InputIt last) {
    const size_type insert_index = pos - data_;
    if (pos > data_ + size_ || pos < data_) throw std::out_of_range("DynamicArray::insert: position out of range");

    size_type count = 0;
    for(InputIt it = first; it != last; ++it) ++count;

    if (count == 0) return data_+insert_index;

    if (size_ + count > capacity_) {
    
        size_type new_capacity = (capacity_ == 0) ? DEFAULT_CAPACITY : 
                                 static_cast<size_type>(capacity_ * GROWTH_FACTOR);
        
        while(new_capacity < size_ + count) new_capacity = static_cast<size_type>(new_capacity * GROWTH_FACTOR);

        pointer new_data = static_cast<pointer>(::operator new(new_capacity * sizeof(value_type)));
        size_type new_size = 0;
        
        try {
            for (size_type i = 0; i < insert_index; ++i) new(&new_data[new_size++]) value_type(std::move_if_noexcept(data_[i]));
            
            for(InputIt it_counter = first; it_counter != last; ++it_counter) new(&new_data[new_size++]) value_type(*it_counter);

            for (size_type i = insert_index; i < size_; ++i) new(&new_data[new_size++]) value_type(std::move_if_noexcept(data_[i]));
        
        } catch (...) {
            for (size_type i = 0; i < new_size; ++i) new_data[i].~value_type();

            ::operator delete(new_data);
            throw;
        }
        
        for (size_type i = 0; i < size_; ++i) data_[i].~value_type();
    
        ::operator delete(data_);
        
        data_ = new_data;
        capacity_ = new_capacity;
        size_ = new_size;
    }else {
        if (insert_index == size_) for(InputIt it_counter = first; it_counter != last; ++it_counter) new(&data_[size_++]) value_type(*it_counter);
        else {    
            
            for (size_type i = size_ - 1; i >= insert_index; --i) {
                size_type nueva_pos = i + count;
            
                if (nueva_pos < size_) data_[nueva_pos] = std::move_if_noexcept(data_[i]);
                else new(&data_[nueva_pos]) value_type(std::move_if_noexcept(data_[i]));
            }
            
            InputIt it = first;
            for (size_type i = 0; i < count; ++i, ++it){
                if(insert_index + i < size_) data_[insert_index + i] = *it;
                else new(&data_[insert_index+i]) value_type(*it);
            }
    
            size_ += count;
        }
    }

    return data_ + insert_index;
}

template<typename T>   
template<typename... Args>
DynamicArray<T>::iterator DynamicArray<T>::emplace(const_iterator pos, Args&&... args){
    const size_type insert_index = pos - data_;
    if (pos > data_ + size_ || pos < data_) throw std::out_of_range("DynamicArray::emplace: position out of range");

    if (size_ + 1 > capacity_) {
        size_type new_capacity = (capacity_ == 0) ? DEFAULT_CAPACITY : 
                                 static_cast<size_type>(capacity_ * GROWTH_FACTOR);
        
        while(new_capacity < size_ + 1) new_capacity = static_cast<size_type>(new_capacity * GROWTH_FACTOR);
    
        pointer new_data = static_cast<pointer>(::operator new(new_capacity * sizeof(value_type)));
        size_type new_size = 0;

        try {
            for (;new_size < insert_index; ++new_size) new(&new_data[new_size]) value_type(std::move_if_noexcept(data_[new_size]));
            
            new(&new_data[new_size++]) value_type(std::forward<Args>(args)...);

            for (size_type i = insert_index; i < size_; ++i) new(&new_data[new_size++]) value_type(std::move_if_noexcept(data_[i]));

        } catch (...) {
            for (size_type i = 0; i < new_size; ++i) new_data[i].~value_type();
            
            ::operator delete(new_data);
            throw;
        }
        
        for (size_type i = 0; i < size_; ++i) data_[i].~value_type();
        
        ::operator delete(data_);
        
        data_ = new_data;
        capacity_ = new_capacity;
        size_ = new_size; 
    } else {
        
        if (insert_index == size_) new(&data_[size_]) value_type(std::forward<Args>(args)...);
        else {            
            new(&data_[size_]) value_type(std::move_if_noexcept(data_[size_ - 1]));
            
            for (size_type i = size_-1; i > insert_index; --i) data_[i] = std::move_if_noexcept(data_[i - 1]);
            
            data_[insert_index].~value_type();
            new(&data_[insert_index]) value_type(std::forward<Args>(args)...);
        }
        ++size_;
    }
    
    return data_ + insert_index;
}

template<typename T>            
DynamicArray<T>::iterator DynamicArray<T>::erase(const_iterator pos) {
    const size_type erase_index = pos - data_;
    if (pos > data_ + size_ || pos < data_) throw std::out_of_range("DynamicArray::erase: position out of range");

    for (size_type i = erase_index; i < size_ - 1; ++i) data_[i] = std::move_if_noexcept(data_[i + 1]);
    
    data_[--size_].~value_type();

    if(size_ > (1/4)*capacity_) shrink_to_fit();

    return data_ + erase_index;
}

template<typename T>            
DynamicArray<T>::iterator DynamicArray<T>::erase(const_iterator first, const_iterator last) {
    const size_type first_index = first - data_;
    const size_type last_index = last - data_;
    const size_type count = last_index - first_index;
    if (first > data_ + size_ || first < data_ || last > data_ + size_ || last < data_ ) throw std::out_of_range("DynamicArray::erase: position out of range");

   if (first == last) return const_cast<iterator>(first); 

    for (size_type i = first_index; i < size_ - count; ++i) data_[i] = std::move_if_noexcept(data_[i + count]);
    
    for (size_type i = size_ - count; i < size_; ++i) data_[i].~value_type();

    size_ -= count;
    
    if(size_ > (1/4)*capacity_) shrink_to_fit();

    return data_ + first_index;
}

template<typename T>            
DynamicArray<T>::iterator DynamicArray<T>::find(const_reference value) {
    for(iterator it = begin(); it != end(); ++it) if (*it == value) return it;
    return end();
}

template<typename T>            
DynamicArray<T>::const_iterator DynamicArray<T>::find(const_reference value) const {
    for(const_iterator it = cbegin(); it != cend(); ++it) if (*it == value) return it;
    return cend();
}

// ----- Capacidad -----

template<typename T>            
bool DynamicArray<T>::empty() const noexcept {
    return size_ == 0;
}

template<typename T>            
DynamicArray<T>::size_type DynamicArray<T>::size() const noexcept {
    return size_;
}

template<typename T>            
DynamicArray<T>::size_type DynamicArray<T>::capacity() const noexcept {
    return capacity_;
}

// ----- Modificacion -----
template<typename T>            
void DynamicArray<T>::clear() {
    if (size_ == 0) return;
    for(;size_ > 0; --size_) data_[size_-1].~value_type();
}

template<typename T>            
void DynamicArray<T>::reserve(size_type capacity) {
    if (capacity_ >= capacity) return;

    pointer new_data = static_cast<pointer>(::operator new(capacity * sizeof(value_type)));
    size_type new_size = 0;
    
    try {
        for (;new_size < size_; ++new_size) new(&new_data[new_size]) value_type(std::move_if_noexcept(data_[new_size]));
        
    } catch (...) {
        for (size_type i = 0; i < new_size; ++i) new_data[i].~value_type();
        
        ::operator delete(new_data);
        throw;
    }
    
    for (size_type i = 0; i < size_; ++i) data_[i].~value_type();
    
    ::operator delete(data_);
    
    data_ = new_data;
    capacity_ = capacity;
}

template<typename T>            
void DynamicArray<T>::shrink_to_fit() {
    if (capacity_ == size_ || capacity_ == 0) {
        return; 
    }

    if (size_ == 0) {
        ::operator delete(data_);
        data_ = nullptr;
        capacity_ = 0;
        return;
    }

    pointer new_data = static_cast<pointer>(::operator new(size_ * sizeof(value_type)));
    size_type new_size = 0;

    try {
        for (;new_size < size_; ++new_size) new(&new_data[new_size]) value_type(std::move_if_noexcept(data_[new_size]));
    
    } catch (...) {
        for (size_type i = 0; i < new_size; ++i) {
            new_data[i].~value_type();
        }
        ::operator delete(new_data);
        throw;  
    }
    
    for (size_type i = 0; i < size_; ++i) data_[i].~value_type();
    
    ::operator delete(data_);
    
    data_ = new_data;
    capacity_ = size_;  
}

template<typename T>            
void DynamicArray<T>::push_back(const_reference value){
    if (size_ >= capacity_) reserve( (capacity_ == 0) ? DEFAULT_CAPACITY:capacity_*GROWTH_FACTOR);
    
    new(&data_[size_++]) value_type(value);
}

template<typename T>            
void DynamicArray<T>::push_back(T&& value){
    if (size_ >= capacity_) reserve( (capacity_ == 0) ? DEFAULT_CAPACITY:capacity_*GROWTH_FACTOR);
    
    new(&data_[size_++]) value_type(std::move(value));
}

template<typename T>           
template<typename... Args>
DynamicArray<T>::reference DynamicArray<T>::emplace_back(Args&&... args){
    if (size_ >= capacity_) reserve( (capacity_ == 0) ? DEFAULT_CAPACITY:capacity_*GROWTH_FACTOR);
    
    new(&data_[size_++]) value_type(std::forward<Args>(args)...);

    return data_[size_ - 1];
}

template<typename T>            
void DynamicArray<T>::pop_back(){
    data_[--size_].~value_type();
    if(size_ > (1/4)*capacity_) shrink_to_fit();
}

template<typename T>            
void DynamicArray<T>::insert(size_type index, const_reference value){
    if (index > size_) throw std::out_of_range("DynamicArray::insert: position out of range");

    insert(data_+index, value);
}

template<typename T>            
void DynamicArray<T>::insert(size_type index, T&& value){
    if (index > size_) throw std::out_of_range("DynamicArray::insert: position out of range");

    insert(data_+index, std::move(value));
}

template<typename T>            
template<typename... Args>
DynamicArray<T>::reference DynamicArray<T>::emplace(size_type index, Args&&... args){
    if (index > size_) throw std::out_of_range("DynamicArray::emplace: position out of range");

    auto it_ref = emplace(data_+index, std::forward<Args>(args)...);
    
    return *it_ref;
}

template<typename T>            
void DynamicArray<T>::erase(size_type index){
    if (index > size_ ) throw std::out_of_range("DynamicArray::erase: position out of range");

    erase(data_ + index);
}

template<typename T>            
bool DynamicArray<T>::operator==(const DynamicArray& other) const {
    if (this == &other) return true;
    if (size_ != other.size_) return false;
    if (data_ == other.data_) return true;

    for (size_type i = 0; i < size_; ++i) if (!(data_[i] == other.data_[i])) return false;
        
    return true;
}

template<typename T>            
bool DynamicArray<T>::operator!=(const DynamicArray& other) const {
    return !(*this == other);
}

template<typename T>            
void DynamicArray<T>::swap(DynamicArray& other) noexcept {
    pointer temp_data = data_;
    data_ = other.data_;
    other.data_ = temp_data;
    
    size_type temp_size = size_;
    size_ = other.size_;
    other.size_ = temp_size;
    
    size_type temp_capacity = capacity_;
    capacity_ = other.capacity_;
    other.capacity_ = temp_capacity;
}
