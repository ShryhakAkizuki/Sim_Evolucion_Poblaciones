#pragma once
#include <string>

#include "map/manager/ChunkCord.hpp"
#include "map/manager/Chunk.hpp"
#include "map/manager/Tile.hpp"

#include "data_structures/Unordered_map.hpp"

class ChunkManager{
private:
    // ----- Atributos -----
    Unordered_map<ChunkCoord, std::unique_ptr<Chunk>> _chunks;
    uint32_t _chunk_size = 16;
    uint64_t _seed;

public:
    // ----- Constructores -----
    explicit ChunkManager(uint32_t chunk_size = 16,  
                          uint64_t worldSeed = 12345);

    ChunkManager(const ChunkManager& other) = delete;
    ChunkManager(ChunkManager&& other) noexcept;

    // ----- Destructor -----
    ~ChunkManager() = default;

    // ----- Operadores -----
    ChunkManager& operator=(const ChunkManager& other) = delete;        // Operador de asignación por copia
    ChunkManager& operator=(ChunkManager&& other) noexcept;    // Operador de asignación por movimiento

    // ----- Iteradores -----
    class iterator;
    class const_iterator;
    
    iterator begin();
    iterator end();
    
    const_iterator begin() const;
    const_iterator end() const;
    
    const_iterator cbegin() const;
    const_iterator cend() const;
    
    // ----- Métodos -----

    // Conversiones
    ChunkCoord WorldToChunkPos(int worldX, int worldY) const;
    
    // Acceso y retorno - coord
    void SetChunk(ChunkCoord coord, std::unique_ptr<Chunk>&& chunk);
    Chunk* GetChunk(ChunkCoord coord);
    const Chunk* GetChunk(ChunkCoord coord) const;

    // Acceso y retorno - world x,y
    void SetChunk(int worldX, int worldY, std::unique_ptr<Chunk>&& chunk);
    
    Chunk* GetChunk(int worldX, int worldY);
    const Chunk* GetChunk(int worldX, int worldY) const;

    Tile& GetTile(int worldX, int worldY, Chunk* chunk);
    Tile& GetTile(int worldX, int worldY);

    const Tile& GetTile(int worldX, int worldY, const Chunk* chunk) const;
    const Tile& GetTile(int worldX, int worldY) const;

    void SetTile(int worldX, int worldY, const Tile& Value){ GetTile(worldX, worldY) = Value; }
    void SetTile(int worldX, int worldY, Tile&& Value){ GetTile(worldX, worldY) = std::move(Value); }

    const uint32_t& getChunkSize() const { return _chunk_size; }

    // Eliminacion/descarga
    void eraseChunk(const ChunkCoord& coord);
  
    // Utilidades
    bool HasChunk(ChunkCoord coord) const { return _chunks.find_ptr(coord) != nullptr; }
    bool HasChunk(int worldX, int worldY) const;

    size_t GetLoadedChunkCount() const { return _chunks.size(); }    

// Iteradores

class iterator {
private:
    Unordered_map<ChunkCoord, std::unique_ptr<Chunk>>::iterator _map_iter;

public:
    iterator(Unordered_map<ChunkCoord, std::unique_ptr<Chunk>>::iterator it)
    : _map_iter(it) {}
    
    Chunk* operator*(){
        return _map_iter->second().get();
    }

    Chunk* operator->()  {
        return _map_iter->second().get();
    }
        
    ChunkCoord coord() {
        return _map_iter->first();  
    }

    iterator& operator++() {
        ++_map_iter;
        return *this;
    }

    iterator operator++(int) {
        iterator temp = *this;
        ++(*this);
        return temp;
    }
    bool operator==(const iterator& other) const {
        return _map_iter == other._map_iter;
    }
    
    bool operator!=(const iterator& other) const {
        return _map_iter != other._map_iter;
    }

};

class const_iterator {
private:
    Unordered_map<ChunkCoord, std::unique_ptr<Chunk>>::const_iterator _map_iter;

public:
    const_iterator(Unordered_map<ChunkCoord, std::unique_ptr<Chunk>>::const_iterator it)
    : _map_iter(it) {}
    
    const Chunk* operator*() const{
        return _map_iter->second().get();
    }

    const Chunk* operator->() const{
        return _map_iter->second().get();
    }
        
    ChunkCoord coord() const {
        return _map_iter->first();  
    }

    const_iterator& operator++() {
        ++_map_iter;
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator temp = *this;
        ++(*this);
        return temp;
    }
    bool operator==(const const_iterator& other) const {
        return _map_iter == other._map_iter;
    }
    
    bool operator!=(const const_iterator& other) const {
        return _map_iter != other._map_iter;
    }

};

private:

    // Disk - Chunks
    std::unique_ptr<Chunk> LoadChunkFromDisk(const ChunkCoord& coord);
    void SaveChunkToDisk(Chunk* chunk);

    // Dynamic Link - Chunks
    void LinkChunkNeighbors(Chunk* chunk);
    void UnlinkChunkNeighbors(const ChunkCoord& coord, Chunk* chunk);
    
    // PATH - Chunks
    std::string GetExecutableDirectory() const;
    std::string GetChunkDirectory() const;

};