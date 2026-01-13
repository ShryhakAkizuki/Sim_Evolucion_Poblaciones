#pragma once

#include "map/manager/ChunkManager.hpp"
#include "map/generator/WorldGenerator.hpp"

#include "map/manager/Tile.hpp"
#include "map/manager/Chunk.hpp"
#include "map/manager/ChunkCord.hpp"

#include "data_structures/Double_Linked_List.hpp"

class WorldSystem {
private:
    ChunkManager _Manager;
    WorldGenerator _Generator;

    int _simulation_distance = 8;
    int _keep_loaded_distance = 12;

    Double_Linked_List<ChunkCoord> _Activity_Centers;
    Double_Linked_List<Chunk*> _Distant_Chunks;
    
public:
    // ----- Constructores -----

    explicit WorldSystem(const BiomeSystem& biomeSystem, 
                         uint64_t worldSeed = 12345, 
                        uint32_t chunkSize = 16, 
                         int simulation_distance = 8, 
                         int keep_loaded_distance = 12);
                         
    explicit WorldSystem(const BiomeSystem& biomeSystem,
                         uint64_t worldSeed = 12345, 
                        uint32_t chunkSize = 16, 
                         LakeConfig lake_config = LakeConfig(),
                         int simulation_distance = 8, 
                         int keep_loaded_distance = 12);

    WorldSystem(const WorldSystem& other) = delete;
    WorldSystem(WorldSystem&& other) noexcept;

    // ----- Destructor -----
    ~WorldSystem() = default;

    // ----- Operadores -----
    WorldSystem& operator=(const WorldSystem& other) = delete;
    WorldSystem& operator=(WorldSystem&& other) noexcept;

    // ----- Métodos Base -----
    Tile& GetTile(int WorldX, int WorldY);
    const Tile& GetTile(int WorldX, int WorldY) const;

    const Chunk* GetChunk(ChunkCoord coord) const;

    void SetTile(int WorldX, int WorldY, const Tile& Value);
    void SetTile(int WorldX, int WorldY, Tile&& Value);

    void UnloadChunk(const ChunkCoord& coord);
    void UnloadChunk(int WorldX, int WorldY);

    const DynamicArray<DynamicArray<Tile>>& LoadChunk(ChunkCoord Coord);
    const DynamicArray<DynamicArray<Tile>>& LoadChunk(int WorldX, int WorldY);

    const DynamicArray<DynamicArray<Tile>>* LoadChunk_ptr(ChunkCoord Coord);
    const DynamicArray<DynamicArray<Tile>>* LoadChunk_ptr(int WorldX, int WorldY);

    const uint32_t& GetChunkSize() const;

    // ------ Carga y descarga masiva ------
    DynamicArray<const DynamicArray<DynamicArray<Tile>>*> loadAllChunksInVector(const DynamicArray<ChunkCoord>& Chunk_Array);
    void UnloadAllChunksInVector(const DynamicArray<ChunkCoord>&Chunk_Array);

    // ------ Gestion de Chunks y estados ------
    void DynamicChunkStates();
    // void CalculateCentersActivity(); 

    // ------ Carga y descarga automatica ------
    void UnloadFarChunks();
    void LoadActivityChunks();

// private:
    // ------ Gestion de centros de actividad ------
    void Set_Center(ChunkCoord coord);
    void Set_Erase_Center(ChunkCoord coord);

};