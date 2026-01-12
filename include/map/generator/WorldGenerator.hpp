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
    
    DynamicArray<BiomeSeed> _biomeSeeds;

    LakeConfig _lakeConfig;
    PerlinNoise _globalNoise;

public:
    // ----- Constructores -----
    explicit WorldGenerator(const BiomeSystem& Biomas, uint64_t worldSeed = 12345);
    explicit WorldGenerator(const BiomeSystem& Biomas, const LakeConfig& lake_config, uint64_t worldSeed = 12345);

    WorldGenerator(const WorldGenerator& other);
    WorldGenerator(WorldGenerator&& other) noexcept;
    
    // ----- Destructor -----
    ~WorldGenerator() = default;

    // ----- Operadores -----
    WorldGenerator& operator=(const WorldGenerator& other);
    WorldGenerator& operator=(WorldGenerator&& other) noexcept;

    // ----- Métodos -----
    // Generacion
    std::unique_ptr<Chunk> generateChunk(int chunkX, int chunkY, uint32_t chunkSize);
    std::unique_ptr<Chunk> generateChunk(ChunkCoord coord, uint32_t chunkSize);
    void initializeBiomeSeeds(const BiomeSystem& Biomas, float Area = 1000);

    // Configuracion
    void setLakeConfig(const LakeConfig& config) { _lakeConfig = config; }
    const LakeConfig& getLakeConfig() const { return _lakeConfig; }

    uint64_t getWorldSeed() const { return _worldSeed; }
    void setWorldSeed(uint64_t worldSeed) { _worldSeed = worldSeed; }

private:
    // ----- Métodos de generación -----
    void assignBiomesToChunk(Chunk& chunk);
    void generateLakes(Chunk& chunk);
    
    // ----- Helpers -----
    int findClosestBiome(float worldX, float worldY) const;
    float calculateBiomeInfluence(const BiomeSeed& seed, float x, float y) const;
};