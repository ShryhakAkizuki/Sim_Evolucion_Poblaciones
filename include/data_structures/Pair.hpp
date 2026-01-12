#pragma once
#include <initializer_list>
#include <stdexcept>
#include <functional>                           // Para std::hash

template<typename T1, typename T2>
class Pair{
protected:
    // ----- Atributos -----
    T1 _first = {};
    T2 _second = {};

public:
    // ----- Constructores -----
    Pair() = default;

    // Constructor normal
    Pair(const T1& first, const T2& second) 
        : _first(first), _second(second) {}
    
    // Constructor con movimiento del segundo
    Pair(const T1& first, T2&& second) 
        : _first(first), _second(std::move(second)) {}
    
    // Constructor con movimiento de ambos
    Pair(T1&& first, T2&& second) 
        : _first(std::move(first)), _second(std::move(second)) {}
    
    // Constructor template para perfect forwarding
    template<typename U1, typename U2>
    Pair(U1&& first, U2&& second) 
        : _first(std::forward<U1>(first)), 
          _second(std::forward<U2>(second)) {}

    Pair(const Pair& other) = default;    
    Pair(Pair&& other) noexcept = default;

    Pair(std::initializer_list<typename std::common_type<T1>::type> init) {
        if(init.size() != 2) {
            throw std::invalid_argument("La lista debe contener exactamente dos elementos");
        }
        
        auto it = init.begin();
        _first = *it;
        _second = *(++it);
    }

    // ----- Destructor -----
    ~Pair() = default;

    // ----- Operadores -----
    Pair& operator=(const Pair&) = default;
    Pair& operator=(Pair&&) noexcept = default;
    
    bool operator==(const Pair& other) const{
        return _first == other._first && _second == other._second;
    }

    bool operator!=(const Pair& other) const{
        return !(*this == other);
    }

    // ----- Métodos -----
    // Asignacion y retorno

    T1& first() { return _first; }
    const T1& first() const { return _first; }

    T2& second() { return _second; }
    const T2& second() const { return _second; }
};

// ESPECIALIZACIÓN DE HASH
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