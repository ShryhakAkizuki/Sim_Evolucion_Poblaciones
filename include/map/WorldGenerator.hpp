#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <random>
#include <cmath>

#include "map/Chunk.hpp"
#include "biome/BiomeSystem.hpp"
#include "data_structures/DynamicArray.hpp"
#include "data_structures/Pair.hpp"
#include "utils/PerlinNoise.hpp"

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
    LakeConfig(float scl, float thresh, float w) 
        : scale(scl), threshold(thresh){}
};

class WorldGenerator {
private:
    // ----- Atributos -----
    uint64_t _worldSeed;
    std::mt19937_64 _rng;
    
    DynamicArray<BiomeSeed> _biomeSeeds;
    std::shared_ptr<BiomeSystem> _biomeSystem;

    LakeConfig _lakeConfig;
    std::shared_ptr<PerlinNoise> _globalNoise;

public:
    // ----- Constructores -----
    WorldGenerator();
    explicit WorldGenerator(uint64_t worldSeed);
    WorldGenerator(uint64_t worldSeed, std::shared_ptr<BiomeSystem> biomeSystem);
    WorldGenerator(const WorldGenerator& other);
    WorldGenerator(WorldGenerator&& other) noexcept;
    
    // ----- Destructor -----
    ~WorldGenerator() = default;

    // ----- Operadores -----
    WorldGenerator& operator=(const WorldGenerator& other);
    WorldGenerator& operator=(WorldGenerator&& other) noexcept;

    // ----- Métodos -----
    // Generacion
    std::shared_ptr<Chunk> generateChunk(int chunkX, int chunkY, uint32_t chunkSize);
    void setBiomeSystem(std::shared_ptr<BiomeSystem> biomeSystem);
    void actualizarEstadosBiomas(float horaDelDia);

    // Lagos
    void setLakeConfig(const LakeConfig& config) { _lakeConfig = config; }
    const LakeConfig& getLakeConfig() const { return _lakeConfig; }

    uint64_t getWorldSeed() const { return _worldSeed; }
    std::shared_ptr<BiomeSystem> getBiomeSystem() const { return _biomeSystem; }

private:
    // ----- Métodos de generación -----
    void initializeBiomeSeeds();
    void assignBiomesToChunk(Chunk& chunk);
    
    void generateLakes(Chunk& chunk);
    
    // ----- Helpers -----
    int findClosestBiome(float worldX, float worldY) const;
    float calculateBiomeInfluence(const BiomeSeed& seed, float x, float y) const;
    uint64_t getChunkSeed(int chunkX, int chunkY) const;
    bool worldToChunkLocal(const Chunk& chunk, float worldX, float worldY, int& localX, int& localY) const;
    float getChunkWorldX(const Chunk& chunk) const;
    float getChunkWorldY(const Chunk& chunk) const;

    // ----- Inicialización -----
    void initializeDefaultBiomes();
};