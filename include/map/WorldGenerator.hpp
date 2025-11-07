#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <random>

#include "map/Chunk.hpp"
#include "biome/BiomeSystem.hpp"
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

class WorldGenerator {
private:
    // ----- Atributos -----
    uint64_t _worldSeed;
    std::mt19937_64 _rng;
    
    DynamicArray<BiomeSeed> _biomeSeeds;
    std::shared_ptr<BiomeSystem> _biomeSystem;
    float _riverFrequency = 0.05f;

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
    // Acceso y retorno
    std::shared_ptr<Chunk> generateChunk(int chunkX, int chunkY, uint32_t chunkSize);

    void setRiverFrequency(float frequency) { _riverFrequency = frequency; }
    void setBiomeSystem(std::shared_ptr<BiomeSystem> biomeSystem);

    uint64_t getWorldSeed() const { return _worldSeed; }
    std::shared_ptr<BiomeSystem> getBiomeSystem() const { return _biomeSystem; }

    // Utilidades 
    void actualizarEstadosBiomas(float horaDelDia);

private:
    // ----- Métodos de generación -----
    void initializeBiomeSeeds();
    void assignBiomesToChunk(Chunk& chunk);
    void generateRivers(Chunk& chunk);
    void generateLakes(Chunk& chunk);
    
    // ----- Helpers -----
    int findClosestBiome(float worldX, float worldY) const;
    float calculateBiomeInfluence(const BiomeSeed& seed, float x, float y) const;
    float getNoise2D(float x, float y, float scale, int octaves) const;
    uint64_t getChunkSeed(int chunkX, int chunkY) const;
    
    // ----- Inicialización -----
    void initializeDefaultBiomes();

};