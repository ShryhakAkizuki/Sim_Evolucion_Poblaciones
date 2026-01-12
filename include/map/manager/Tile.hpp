#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>

class Tile{
private:
    // ----- Atributos -----
    int _biomeId = -1;          // -1 significa no asignado
    bool _hasWater = false;
public:
    // ----- Constructores -----
    Tile() = default;
    
    Tile(int biomeId, bool hasWater) : 
    _biomeId(biomeId), _hasWater(hasWater) {}

    Tile(const Tile& other) :
    _biomeId(other._biomeId), _hasWater(other._hasWater) {}

    Tile(Tile&& other) noexcept :
    _biomeId(other._biomeId), _hasWater(other._hasWater) {
        other._biomeId = -1;
        other._hasWater = false;
    }

    // ----- Destructor -----
    ~Tile() = default;

    // ----- Operadores -----
    Tile& operator=(const Tile& other){
        if(this != &other){
            _biomeId = other._biomeId;
            _hasWater = other._hasWater;
        }
        return *this;
    }

    Tile& operator=(Tile&& other) noexcept {
        if(this != &other){
            _biomeId = other._biomeId;
            _hasWater = other._hasWater;

            other._biomeId = -1;
            other._hasWater = false;
        }
        return *this;
    }

    // ----- Métodos -----

    int getBiomeId() const { return _biomeId; }
    bool hasWater() const { return _hasWater; }

    void setBiomeId(int biomeId) { _biomeId = biomeId; }
    void setHasWater(bool hasWater) { _hasWater = hasWater; }

};