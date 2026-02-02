#pragma once

#include <algorithm>        // Para std::copy, std::move, etc. (en implementaciones)
#include <utility>          // Para std::forward, std::move, etc.
#include <functional>       // Para std::hash

template<typename T1, typename T2>
class Pair{
public:
    // ----- Aliases -----
    using first_type  = T1;
    using second_type = T2;

    using first_reference = T1&;
    using const_first_reference = const T1&;

    using second_reference = T2&;
    using const_second_reference = const T2&;

    // ----- Funciones especiales -----
    Pair();
    Pair(const Pair& other) = delete;
    Pair(Pair&& other) noexcept;
    Pair& operator=(const Pair& other) = delete;
    Pair& operator=(Pair&& other) noexcept;
    ~Pair();

    explicit Pair(const_first_reference first, const_second_reference second);
    explicit Pair(T1&& first, T2&& second);
    
    // ----- Acceso de elementos -----
    first_reference First();
    const_first_reference First() const;
    
    second_reference Second();
    const_second_reference Second() const;

    // ----- Modificacion -----
    void Set_First(const_first_reference value);
    void Set_First(T1&& value);
    
    template<typename... Args>
    void Set_First(Args&&... args);

    void Set_Second(const_second_reference value);
    void Set_Second(T2&& value);
    
    template<typename... Args>
    void Set_Second(Args&&... args);

    // ----- Comparadores -----
    bool operator==(const Pair& other) const;
    bool operator!=(const Pair& other) const;

protected:
    // ----- Atributos -----
    first_type first_ = first_type();
    second_type second_ = second_type();
};

// ----- Funciones especiales -----
template<typename T1, typename T2>
Pair<T1,T2>::Pair() : 
first_(first_type()), second_(second_type()) {}

template<typename T1, typename T2>
Pair<T1,T2>::Pair(Pair&& other) noexcept :
first_(std::move(other.first_)),  second_(std::move(other.second_)) {
    other.first_ = first_type();
    other.second_ = second_type();
}

template<typename T1, typename T2>
Pair<T1,T2>& Pair<T1,T2>::operator=(Pair&& other) noexcept {
    if(this != &other){
        first_ = other.first_;
        second_ = other.second_;
        
        other.first_ = first_type();
        other.second_ = second_type();
    }
    return *this;
}

template<typename T1, typename T2>
Pair<T1,T2>::~Pair() {
    first_.~first_type();
    second_.~second_type();
}

template<typename T1, typename T2>
Pair<T1,T2>::Pair(const_first_reference first, const_second_reference second) :
first_(first), second_(second) {}

template<typename T1, typename T2>
Pair<T1,T2>::Pair(T1&& first, T2&& second) :
first_(std::move(first)), second_(std::move(second)) {}

template<typename T1, typename T2>
Pair<T1,T2>::first_reference Pair<T1,T2>::First() {
    return first_;
}

// ----- Acceso de elementos -----
template<typename T1, typename T2>
Pair<T1,T2>::const_first_reference Pair<T1,T2>::First() const {
    return first_;
}

template<typename T1, typename T2>
Pair<T1,T2>::second_reference Pair<T1,T2>::Second() {
    return second_;
}

template<typename T1, typename T2>
Pair<T1,T2>::const_second_reference Pair<T1,T2>::Second() const {
    return second_;
}

// ----- Modificacion -----
template<typename T1, typename T2>
void Pair<T1,T2>::Set_First(const_first_reference value) {
    first_ = value;
}

template<typename T1, typename T2>
void Pair<T1,T2>::Set_First(T1&& value) {
    first_ = std::move(value);
}

template<typename T1, typename T2>
template<typename... Args>
void  Pair<T1,T2>::Set_First(Args&&... args) {
    first_ = first_type(std::forward<Args>(args)...);
}

template<typename T1, typename T2>
void Pair<T1,T2>::Set_Second(const_second_reference value) {
    second_ = value;
}

template<typename T1, typename T2>
void Pair<T1,T2>::Set_Second(T2&& value) {
    second_ = std::move(value);
}

template<typename T1, typename T2>
template<typename... Args>
void  Pair<T1,T2>::Set_Second(Args&&... args) {
    second_ = second_type(std::forward<Args>(args)...);
}

// ----- Comparadores -----
template<typename T1, typename T2>
bool Pair<T1,T2>::operator==(const Pair& other) const {
    if (this == &other) return true;
    return first_ == other.first_ && second_ == other.second_;
}

template<typename T1, typename T2>
bool Pair<T1,T2>::operator!=(const Pair& other) const {
    return !(*this == other);
}

// ################# Especializacion de Hash #################
namespace std {
    template<typename T1, typename T2>
    struct hash<Pair<T1, T2>> {
        size_t operator()(const Pair<T1, T2>& pair) const {
            size_t h1 = hash<T1>{}(pair.first());
            size_t h2 = hash<T2>{}(pair.second());
            return h1 ^ (h2 << 1) ^ (h2 >> 31);
        }
    };
}
