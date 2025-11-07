#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include "data_structures/DynamicArray.hpp"
#include "map/Tile.hpp"
#include "data_structures/Pair.hpp"

#include <iostream>

enum class State{
    INITIALIZATED,
    UNLOADED,
    LOADED,
    ACTIVE
};

class Chunk{
private:
    // ----- Atributos -----
    DynamicArray<DynamicArray<Tile>> _tiles;
    int _chunkX = 0;
    int _chunkY = 0;

    uint32_t _chunk_size = 16;
    
    std::weak_ptr<Chunk> _North;
    std::weak_ptr<Chunk> _South;
    std::weak_ptr<Chunk> _East;
    std::weak_ptr<Chunk> _West;

    State _state = State::INITIALIZATED;

    class RowProxy;
    class ConstRowProxy;

public:
    // ----- Constructores -----
    Chunk() = default;

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


    Chunk(const Chunk& other) : 
    _chunkX(other._chunkX), _chunkY(other._chunkY), 
    _chunk_size(other._chunk_size), _tiles(other._tiles),
    _state(other._state) {}

    Chunk(Chunk&& other) noexcept :
    _chunkX(other._chunkX), _chunkY(other._chunkY), 
    _chunk_size(other._chunk_size), _tiles(std::move(other._tiles)),
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
            other._state = State::UNLOADED;
        }
        return *this;
    }

    // ----- Métodos -----

    // Asignacion y retorno
    RowProxy operator[](int y) {return RowProxy(_tiles[y]); } 
    ConstRowProxy operator[](int y) const {return ConstRowProxy(_tiles[y]); } 

    Tile& at(int x, int y){
        if(x < 0 || x >= _chunk_size || y < 0 || y >= _chunk_size){
            throw std::out_of_range("Coordinates out of bounds");
        }
        return _tiles[y][x];
    }

    const Tile& at(int x, int y) const {
        if(x < 0 || x >= _chunk_size || y < 0 || y >= _chunk_size){
            throw std::out_of_range("Coordinates out of bounds");
        }
        return _tiles[y][x];
    }

    int getChunkX() const { return _chunkX; }
    int getChunkY() const { return _chunkY; }
    uint32_t getChunkSize() const { return _chunk_size; }

    // Vecindad
    void Set_North(std::shared_ptr<Chunk> neighbor) { _North = neighbor; }
    void Set_South(std::shared_ptr<Chunk> neighbor) { _South = neighbor; }
    void Set_East(std::shared_ptr<Chunk> neighbor)  { _East = neighbor; }
    void Set_West(std::shared_ptr<Chunk> neighbor)  { _West = neighbor; }

    std::shared_ptr<Chunk> get_North() const { return _North.lock(); }  
    std::shared_ptr<Chunk> get_South() const { return _South.lock(); }  
    std::shared_ptr<Chunk> get_East() const  { return _East.lock(); }  
    std::shared_ptr<Chunk> get_West() const  { return _West.lock(); }  

    bool has_North() const { return !_North.expired(); }
    bool has_South() const { return !_South.expired(); }
    bool has_East() const  { return !_East.expired(); }
    bool has_West() const  { return !_West.expired(); }

    std::shared_ptr<Chunk> getNeighborChunk(int dx, int dy) const {
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
    bool isUnloaded() const { return _state == State::UNLOADED; }
    bool isInitializated() const { return _state == State::INITIALIZATED; }

    void activate() { _state = State::ACTIVE; }
    void deactivate() { _state = State::LOADED; }
    void unload() { _state = State::UNLOADED; }


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