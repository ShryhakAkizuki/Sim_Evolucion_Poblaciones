#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <cmath>  

#include "data_structures/Pair.hpp"

class ChunkCoord: public Pair<int,int>{
public:
    // ----- Constructores -----
    
    ChunkCoord() : Pair(0,0) {}
    ChunkCoord(int x, int y) : Pair(x,y) {}
    
    // ----- Operadores -----

    int x() const { return first(); }
    int y() const { return second(); }

    int& x() { return first(); }
    int& y() { return second(); }

    // Operadores útiles
    ChunkCoord& operator+=(const ChunkCoord& other) {
        x() += other.x();
        y() += other.y();
        return *this;
    }
    
    ChunkCoord operator+(const ChunkCoord& other) const {
        return ChunkCoord(x() + other.x(), y() + other.y());
    }
    
    bool operator<(const ChunkCoord& other) const {
        return (x() < other.x()) || (x() == other.x() && y() < other.y());
    }
   
    // ----- Operaciones específicas de coordenadas -----
    ChunkCoord getNeighbor(int dx, int dy) const {
        return ChunkCoord(x()+dx, y()+dy);
    }

    ChunkCoord get_North() const { return ChunkCoord(x(), y() + 1); }
    ChunkCoord get_South() const { return ChunkCoord(x(), y() - 1); }
    ChunkCoord get_East() const  { return ChunkCoord(x() + 1, y()); }
    ChunkCoord get_West() const  { return ChunkCoord(x() - 1, y()); }

    // ----- Distancia entre chunks -----
    int manhattanDistance(const ChunkCoord& other) const {
        return std::abs(x() - other.x()) + std::abs(y() - other.y());
    }
    
    float euclideanDistance(const ChunkCoord& other) const {
        int dx = x() - other.x();
        int dy = y() - other.y();
        return std::sqrt(dx*dx + dy*dy);
    }
};

// ESPECIALIZACIÓN DE HASH
namespace std {
    template<>
    struct hash<ChunkCoord> {
        size_t operator()(const ChunkCoord& coord) const {
            return hash<Pair<int, int>>{}(coord);
        }
    };
}