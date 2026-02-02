#pragma once

#include <cstddef>          // Para std::size_t, std::ptrdiff_t
#include <initializer_list> // Para std::initializer_list
#include <span>             // Para std::span (C++20)
#include <stdexcept>        // Para std::out_of_range (en at())
#include <algorithm>        // Para std::copy, std::move, etc. (en implementaciones)
#include <utility>          // Para std::forward, std::move, etc.
#include <iterator>         // Para std::distance, categorías de iteradores
#include <concepts>

#include "data_structures/Double_Linked_List.hpp"

template<typename T>
class Linked_Queue{
public:
    // ----- Aliases -----
    using value_type = T;

    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference = T&;
    using const_reference = const T&;

    // ----- Funciones especiales -----
    Linked_Queue();
    Linked_Queue(const Linked_Queue& other) = delete;
    Linked_Queue(Linked_Queue&& other) noexcept;
    Linked_Queue& operator=(const Linked_Queue& other) = delete;
    Linked_Queue& operator=(Linked_Queue&& other) noexcept;
    ~Linked_Queue();

    Linked_Queue(size_type size, const_reference value);
    Linked_Queue(std::initializer_list<T> init);
    explicit Linked_Queue(std::span<T> s);

    template<std::input_iterator It>
    Linked_Queue(It first, It last);

    // ----- Acceso de elementos -----
    reference front();
    reference back();
    const_reference front() const;
    const_reference back() const;

    // ----- Capacidad -----
    bool empty() const noexcept;
    size_type size() const noexcept;

    // ----- Modificacion -----
    void clear();

    void enqueue(const_reference value);
    void enqueue(T&& value);

    template<typename... Args>
    reference emplace_enqueue(Args&&... args);

    void dequeue();
    value_type extract();   

 // ----- Comparadores -----
    bool operator==(const Linked_Queue& other) const;
    bool operator!=(const Linked_Queue& other) const;

    // ----- Helpers -----
    void swap(Linked_Queue& other) noexcept;

private:
    // ----- Atributos -----
    Double_Linked_List<T> list_ = Double_Linked_List<T>();
};


template<typename T>
Linked_Queue<T>::Linked_Queue() :
list_(Double_Linked_List<T>()) {}

template<typename T>
Linked_Queue<T>::Linked_Queue(Linked_Queue&& other) noexcept : 
list_(std::move(other.list_)) {}

template<typename T>
Linked_Queue<T>& Linked_Queue<T>::operator=(Linked_Queue&& other) noexcept {
    if(this != &other){
        list_.clear();
        list_ = std::move(other.list_);
    }
    return *this;
}

template<typename T>
Linked_Queue<T>::~Linked_Queue() {
    clear();
}

template<typename T>
Linked_Queue<T>::Linked_Queue(size_type size, const_reference value) :
list_(size,value) {}

template<typename T>
Linked_Queue<T>::Linked_Queue(std::initializer_list<T> init) :
list_(init) {}

template<typename T>
Linked_Queue<T>::Linked_Queue(std::span<T> s) :
list_(s) {}

template<typename T>
template<std::input_iterator It>
Linked_Queue<T>::Linked_Queue(It first, It last) : 
list_(first, last) {}

// ----- Acceso de elementos -----
template<typename T>
Linked_Queue<T>::reference Linked_Queue<T>::front() {
    return list_.front();
}

template<typename T>
Linked_Queue<T>::reference Linked_Queue<T>::back() {
    return list_.back();
}

template<typename T>
Linked_Queue<T>::const_reference Linked_Queue<T>::front() const{
    return list_.front();
}

template<typename T>
Linked_Queue<T>::const_reference Linked_Queue<T>::back() const {
    return list_.back();
}

// ----- Capacidad -----
template<typename T>
bool Linked_Queue<T>::empty() const noexcept {
    return list_.empty();
}

template<typename T>
Linked_Queue<T>::size_type Linked_Queue<T>::size() const noexcept {
    return list_.size();
}

// ----- Modificacion -----
template<typename T>
void Linked_Queue<T>::clear() {
    list_.clear();
}

template<typename T>
void Linked_Queue<T>::enqueue(const_reference value) {
    list_.push_back(value);
}

template<typename T>
void Linked_Queue<T>::enqueue(T&& value) {
    list_.push_back(std::move(value));
}

template<typename T>
template<typename... Args>
Linked_Queue<T>::reference Linked_Queue<T>::emplace_enqueue(Args&&... args) {
    return list_.emplace_back(std::forward<Args>(args)...);
}

template<typename T>
void Linked_Queue<T>::dequeue() {
    list_.pop_front();
}

template<typename T>
Linked_Queue<T>::value_type Linked_Queue<T>::extract() {
    value_type temp = std::move(list_.front());
    list_.pop_front();

    return temp;
}

 // ----- Comparadores -----

template<typename T>
bool Linked_Queue<T>::operator==(const Linked_Queue& other) const {
    return list_ == other.list_;
}

template<typename T>
bool Linked_Queue<T>::operator!=(const Linked_Queue& other) const {
    return !(*this == other);
}

// ----- Helpers -----
template<typename T>
void Linked_Queue<T>::swap(Linked_Queue& other) noexcept {
    Double_Linked_List<T> temp_list(std::move(list_));
    list_ = std::move(other.list_);
    other.list_ = std::move(temp_list);
}