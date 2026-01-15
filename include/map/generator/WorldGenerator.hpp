#pragma once
#include <memory>
#include <cstdint>
#include <random>

#include "map/manager/Chunk.hpp"
#include "map/manager/ChunkCord.hpp"

#include "biome/BiomeSystem.hpp"

#include "utils/PerlinNoise.hpp"

#include "data_structures/DynamicArray.hpp"
#include "data_structures/Pair.hpp"
#include "data_structures/Unordered_map.hpp"

struct BiomeSeed {
    int biomeId;
    float x, y;
    float strength;
    
    BiomeSeed() = default;

    BiomeSeed(int id, float posX, float posY, float str) 
        : biomeId(id), x(posX), y(posY), strength(str) {}
};

struct LakeConfig {
    float scale = 0.01f;         // Escala del ruido
    float threshold = -0.4f;     // Umbral para generar lagos
    
    LakeConfig() = default;
    LakeConfig(float scl, float thresh) 
        : scale(scl), threshold(thresh){}
};

class WorldGenerator {
private:
    // ----- Atributos -----
    uint64_t _worldSeed;
    std::mt19937_64 _rng;

    // Configuracion - Biomas (Poisson Disk)
    float _biomeRadius;                         
    float _cellSize;                            
    Unordered_map<int64_t, DynamicArray<BiomeSeed>> _seedGrid;

    // Configuracion - Lagos
    LakeConfig _lakeConfig;
    PerlinNoise _globalNoise;
    
    // Biomas disponibles
    DynamicArray<int> _biomeIds;
public:
    // ----- Constructores -----
    explicit WorldGenerator(const DynamicArray<int>& biomeIds, uint64_t worldSeed = 12345, float _biomeRadiusInMeters = 500, float metersPerTile = 2.0f);
    explicit WorldGenerator(const DynamicArray<int>& biomeIds, const LakeConfig& lake_config, uint64_t worldSeed = 12345, float _biomeRadiusInMeters = 500, float metersPerTile = 2.0f);

    WorldGenerator(const WorldGenerator& other) = delete;
    WorldGenerator(WorldGenerator&& other) noexcept;
    
    // ----- Destructor -----
    ~WorldGenerator() = default;

    // ----- Operadores -----
    WorldGenerator& operator=(const WorldGenerator& other) = delete;
    WorldGenerator& operator=(WorldGenerator&& other) noexcept;

    // ----- Métodos -----
    // Generacion
    std::unique_ptr<Chunk> generateChunk(int chunkX, int chunkY, uint32_t chunkSize);
    std::unique_ptr<Chunk> generateChunk(ChunkCoord coord, uint32_t chunkSize);

    // Configuracion
    void setLakeConfig(const LakeConfig& config) { _lakeConfig = config; }
    const LakeConfig& getLakeConfig() const { return _lakeConfig; }
    
    void setBiomeRadius(float radiusInMeters, float metersPerTile = 2.0f);
    float getBiomeRadius() const { return _biomeRadius; }

    uint64_t getWorldSeed() const { return _worldSeed; }
    void setWorldSeed(uint64_t worldSeed);

private:
    // ----- Metodos Poisson Disk - Biomas -----
    // Generacion de semillas
    void generateSeedsForRegion(const ChunkCoord& coord, uint32_t chunkSize);
    void generateSeedsForCell(int cellX, int cellY);
    bool isValidSeedPosition(float tileX, float tileY, int cellX, int cellY) const;

    // Asignacion
    void assignBiomesToChunk(Chunk& chunk);
    DynamicArray<BiomeSeed> collectSeedsForChunk(const Chunk& chunk) const;
    float calculateBiomeInfluence(const BiomeSeed& seed, float tileX, float tileY) const;
    int selectDominantBiome(float tileX, float tileY, const DynamicArray<BiomeSeed>& seeds) const;

    // ----- Rios -----    
    void generateLakes(Chunk& chunk);
    
    // ----- Helpers -----
    void updateCellSize();
    int64_t calculateCellIndex(int cellX, int cellY) const;
    Pair<int, int> worldToCellCoords(float worldX, float worldY) const;

    // ----- RNG -----
    std::mt19937_64 createCellRNG(int cellX, int cellY) const;

    // ----- Limpieza -----
    void cleanupOldSeeds();
};