#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <iostream>

#include "data_structures/DynamicArray.hpp"
#include "data_structures/Pair.hpp"
#include "map/manager/ChunkCord.hpp"

#include "map/manager/Tile.hpp"


enum class State{
    INITIALIZATED,
    LOADED,
    ACTIVE,
    DISTANT
};

class Chunk{
private:
    // ----- Atributos -----
    DynamicArray<DynamicArray<Tile>> _tiles;
    int _chunkX = 0;
    int _chunkY = 0;

    uint32_t _chunk_size = 16;
    
    Chunk* _North;
    Chunk* _South;
    Chunk* _East;
    Chunk* _West;

    State _state = State::INITIALIZATED;

    class RowProxy;
    class ConstRowProxy;

public:
    // ----- Constructores -----
    Chunk() = default;

    // x - y pair
    explicit Chunk(int x, int y) : _chunkX(x), _chunkY(y), _chunk_size(16), _state(State::INITIALIZATED) {
        initializeTiles();  
    }
    
    explicit Chunk(int x, int y, uint32_t chunk_size) : _chunkX(x), _chunkY(y), _chunk_size(chunk_size), _state(State::INITIALIZATED) {
        initializeTiles();  
    }

    explicit Chunk(int x, int y, const Tile& Tile_Data) : _chunkX(x), _chunkY(y),  _chunk_size(16), _state(State::LOADED) {
        initializeTiles(Tile_Data);  
    }

    explicit Chunk(int x, int y, uint32_t chunk_size, const Tile& Tile_Data) : _chunkX(x), _chunkY(y),  _chunk_size(chunk_size), _state(State::LOADED) {
        initializeTiles(Tile_Data);  
    }

    // coord - struct
    explicit Chunk(ChunkCoord coord) : _chunkX(coord.x()), _chunkY(coord.y()), _chunk_size(16), _state(State::INITIALIZATED) {
        initializeTiles();  
    }
    
    explicit Chunk(ChunkCoord coord, uint32_t chunk_size) : _chunkX(coord.x()), _chunkY(coord.y()), _chunk_size(chunk_size), _state(State::INITIALIZATED) {
        initializeTiles();  
    }

    explicit Chunk(ChunkCoord coord, const Tile& Tile_Data) : _chunkX(coord.x()), _chunkY(coord.y()),  _chunk_size(16), _state(State::LOADED) {
        initializeTiles(Tile_Data);  
    }

    explicit Chunk(ChunkCoord coord, uint32_t chunk_size, const Tile& Tile_Data) : _chunkX(coord.x()), _chunkY(coord.y()),  _chunk_size(chunk_size), _state(State::LOADED) {
        initializeTiles(Tile_Data);  
    }

    // Move and copy
    Chunk(const Chunk& other) :  
    _tiles(other._tiles),
    _chunkX(other._chunkX), _chunkY(other._chunkY),
    _chunk_size(other._chunk_size),
    _state(other._state) {}

    Chunk(Chunk&& other) noexcept :
    _tiles(std::move(other._tiles)),
    _chunkX(other._chunkX), _chunkY(other._chunkY), 
    _chunk_size(other._chunk_size),
    _state(other._state) {
        other._chunkX = 0;
        other._chunkY = 0;
        other._chunk_size = 0;
        other._state = State::INITIALIZATED;
    }

    // ----- Destructor -----
    ~Chunk() = default;

    // ----- Operadores -----
    Chunk& operator=(const Chunk& other) {        // Operador de asignación por copia
        if(this != &other) {
            _chunkX = other._chunkX;
            _chunkY = other._chunkY;
            _chunk_size = other._chunk_size;
            _tiles = other._tiles;
            _state = other._state;
        }
        return *this;
    }

    Chunk& operator=(Chunk&& other) noexcept {    // Operador de asignación por movimiento
        if(this != &other) {
            _chunkX = other._chunkX;
            _chunkY = other._chunkY;
            _chunk_size = other._chunk_size;
            _tiles = std::move(other._tiles);
            _state = other._state;

            other._chunkX = 0;
            other._chunkY = 0;
            other._chunk_size = 0;
            other._state = State::INITIALIZATED;
        }
        return *this;
    }

    // ----- Métodos -----

    // Asignacion y retorno
    RowProxy operator[](int y) {return RowProxy(_tiles[y]); } 
    ConstRowProxy operator[](int y) const {return ConstRowProxy(_tiles[y]); } 

    Tile& at(int x, int y){
        if(x < 0 || static_cast<uint32_t>(x) >= _chunk_size || y < 0 || static_cast<uint32_t>(y) >= _chunk_size){
            throw std::out_of_range("Coordinates out of bounds");
        }
        return _tiles[y][x];
    }

    const Tile& at(int x, int y) const {
        if(x < 0 || static_cast<uint32_t>(x) >= _chunk_size || y < 0 || static_cast<uint32_t>(y) >= _chunk_size){
            throw std::out_of_range("Coordinates out of bounds");
        }
        return _tiles[y][x];
    }

    int getChunkX() const { return _chunkX; }
    int getChunkY() const { return _chunkY; }
    ChunkCoord getChunkCoord() const {return ChunkCoord(_chunkX,_chunkY); }
    uint32_t getChunkSize() const { return _chunk_size; }

    // Vecindad
    void Set_North(Chunk* neighbor) { _North = neighbor; }
    void Set_South(Chunk* neighbor) { _South = neighbor; }
    void Set_East(Chunk* neighbor)  { _East = neighbor; }
    void Set_West(Chunk* neighbor)  { _West = neighbor; }

    Chunk* get_North() const { return _North; }  
    Chunk* get_South() const { return _South; }  
    Chunk* get_East() const  { return _East; }  
    Chunk* get_West() const  { return _West; } 

    bool has_North() const { return _North!=nullptr; }
    bool has_South() const { return _South!=nullptr; }
    bool has_East() const  { return _East!=nullptr; }
    bool has_West() const  { return _West!=nullptr; }

    Chunk* getNeighborChunk(int dx, int dy) const {
        if(dx == 1 && dy == 0) return get_East();
        if(dx == -1 && dy == 0) return get_West();
        if(dx == 0 && dy == 1) return get_North();
        if(dx == 0 && dy == -1) return get_South();
        return nullptr;
    }

    // Conversiones
    Pair<int, int> localToWorld(int localX, int localY) const {
        int worldX = _chunkX * static_cast<int>(_chunk_size) + localX;
        int worldY = _chunkY * static_cast<int>(_chunk_size) + localY;
        return {worldX, worldY};
    }

    Pair<int, int> worldToLocal(int worldX, int worldY) const {
        int localX = worldX - _chunkX * static_cast<int>(_chunk_size);
        int localY = worldY - _chunkY * static_cast<int>(_chunk_size);
        return {localX, localY};
    }

    // Metodos de estado
    
    State getState() const { return _state; }
    void setState(State new_state) { _state = new_state; }
    
    bool isActive() const { return _state == State::ACTIVE; }
    bool isLoaded() const { return _state == State::LOADED || _state == State::ACTIVE; }
    bool isInitializated() const { return _state == State::INITIALIZATED; }
    bool isDistant() const {return _state == State::DISTANT; }

    void activate() { _state = State::ACTIVE; }
    void deactivate() { _state = State::LOADED; }
    void distant() { _state = State::DISTANT; }


private:
    // ----- Inicializador por defecto de Tiles -----

    void initializeTiles(const Tile& fillTile = Tile()) {

        _tiles = DynamicArray<DynamicArray<Tile>>(_chunk_size);
        
        for(uint32_t i = 0; i < _chunk_size; ++i) {
            DynamicArray<Tile> row(_chunk_size);
            for(uint32_t j = 0; j < _chunk_size; ++j) {
                row[j] = fillTile;
            }
            _tiles[i]=std::move(row);
        }
    }

    // ----- Clase comodin - Operador [] auxiliar -----

    class RowProxy{
    private:
        DynamicArray<Tile>& _row;
    
    public:
        RowProxy(DynamicArray<Tile>& row) : _row(row) {}

        Tile& operator[](int x) {return _row[x]; }
    };

    class ConstRowProxy{
    private:
        const DynamicArray<Tile>& _row;
    
    public:
        ConstRowProxy(const DynamicArray<Tile>& row) : _row(row) {}

        const Tile& operator[](int x) const {return _row[x]; }
    };

};