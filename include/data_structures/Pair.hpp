#pragma once
#include <memory>
#include <cstdint>
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

    explicit Pair(const T1& first, const T2& second) :
    _first(first), _second(second) {}

    Pair(const Pair& other) : 
    _first(other._first), _second(other._second) {}

    Pair(Pair&& other) noexcept = default;

    // ----- Destructor -----
    ~Pair() = default;

    // ----- Operadores -----
    Pair& operator=(const Pair& other){
        if(this != &other){
            _first = other._first;
            _second = other._second;
        }
        return *this;
    }

    Pair& operator=(Pair&& other) noexcept {
        if(this != &other){
            _first = std::move(other._first);
            _second = std::move(other._second);
        }
        return *this;
    }

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

// ESPECIALIZACIÓN DE HASH - después de la clase
namespace std {
    template<typename T1, typename T2>
    struct hash<Pair<T1, T2>> {
        size_t operator()(const Pair<T1, T2>& pair) const {
            size_t h1 = hash<T1>{}(pair.first());
            size_t h2 = hash<T2>{}(pair.second());
            return h1 ^ (h2 << 16) ^ (h2 >> 16);
        }
    };
}