#include <cmath>
#include <algorithm>
#include <numbers>

#include "map/generator/WorldGenerator.hpp"

// ----- Constructores -----
WorldGenerator::WorldGenerator(const BiomeSystem& Biomas, uint64_t worldSeed) 
    : _worldSeed(worldSeed), _rng(_worldSeed) {
    
    _globalNoise = PerlinNoise(_worldSeed);
    initializeBiomeSeeds(Biomas);
}

WorldGenerator::WorldGenerator(const BiomeSystem& Biomas, const LakeConfig& lake_config, uint64_t worldSeed)
    : _worldSeed(worldSeed), _rng(_worldSeed), _lakeConfig(lake_config) {
    
    _globalNoise = PerlinNoise(_worldSeed);
    initializeBiomeSeeds(Biomas);
}

WorldGenerator::WorldGenerator(const WorldGenerator& other)
    : _worldSeed(other._worldSeed),
      _rng(other._worldSeed),
      _lakeConfig(other._lakeConfig) {
    
    _biomeSeeds = other._biomeSeeds;
    _globalNoise = other._globalNoise;
}

WorldGenerator::WorldGenerator(WorldGenerator&& other) noexcept
    : _worldSeed(other._worldSeed),
      _rng(std::move(other._rng)),
      _biomeSeeds(std::move(other._biomeSeeds)),
      _lakeConfig(std::move(other._lakeConfig)),
      _globalNoise(std::move(other._globalNoise)){
        other._worldSeed = 12345;
      }

// ----- Operadores -----
WorldGenerator& WorldGenerator::operator=(const WorldGenerator& other) {
    if (this != &other) {
        _worldSeed = other._worldSeed;
        _rng = std::mt19937_64(_worldSeed);
        _biomeSeeds = other._biomeSeeds;
        _lakeConfig = other._lakeConfig;
        _globalNoise = other._globalNoise;
    }
    return *this;
}

WorldGenerator& WorldGenerator::operator=(WorldGenerator&& other) noexcept {
    if (this != &other) {
        _worldSeed = other._worldSeed;
        other._worldSeed = 12345;

        _rng = std::move(other._rng);
        _biomeSeeds = std::move(other._biomeSeeds);
        _lakeConfig = std::move(other._lakeConfig);
        _globalNoise = std::move(other._globalNoise);
    }
    return *this;
}

// ----- Métodos públicos -----
std::unique_ptr<Chunk> WorldGenerator::generateChunk(ChunkCoord coord, uint32_t chunkSize) {
    auto chunk = std::make_unique<Chunk>(coord, chunkSize);
    
    assignBiomesToChunk(*chunk);
    generateLakes(*chunk);

    chunk->setState(State::LOADED);
    return chunk;
}


std::unique_ptr<Chunk> WorldGenerator::generateChunk(int chunkX, int chunkY, uint32_t chunkSize) {
    ChunkCoord coord(chunkX,chunkY);
    return generateChunk(coord,chunkSize);
}

void WorldGenerator::initializeBiomeSeeds(const BiomeSystem& Biomas, float Area) {
    DynamicArray<int> IDs = Biomas.getTodosBiomasID();

    if(IDs.empty()) return;

    _biomeSeeds.clear();
    std::uniform_real_distribution<float> posDist(-Area/2, Area/2);
    std::uniform_real_distribution<float> strengthDist(0.5f, 2.0f);
    std::uniform_int_distribution<int> biomeDist(0, IDs.size()-1);
    
    for (int i = 0; i < Area/10; ++i) {
        _biomeSeeds.push_back(BiomeSeed(
            biomeDist(_rng),
            posDist(_rng),
            posDist(_rng),
            strengthDist(_rng)
        ));
    }
}


// ----- Métodos de Generación de Lagos  -----
void WorldGenerator::generateLakes(Chunk& chunk) {
    
    for (uint32_t y = 0; y < chunk.getChunkSize(); ++y) {
        for (uint32_t x = 0; x < chunk.getChunkSize(); ++x) {
            Pair<int, int> worldPos = chunk.localToWorld(x, y);
            float worldX = static_cast<float>(worldPos.first());
            float worldY = static_cast<float>(worldPos.second());
            
            // RUIDO PERLIN SIMPLE - solo esto
            float lakeNoise = _globalNoise.noise(
                worldX * _lakeConfig.scale, 
                worldY * _lakeConfig.scale, 
                0.0f
            );
            
            if (lakeNoise < _lakeConfig.threshold) {
                Tile& tile = chunk.at(x, y);
                tile.setHasWater(true);
                
            }
        }
    }
}



void WorldGenerator::assignBiomesToChunk(Chunk& chunk) {
    for (uint32_t localY = 0; localY < chunk.getChunkSize(); ++localY) {
        for (uint32_t localX = 0; localX < chunk.getChunkSize(); ++localX) {
            auto worldPos = chunk.localToWorld(localX, localY);
            int biomeId = findClosestBiome(worldPos.first(), worldPos.second());
            
            Tile& tile = chunk.at(localX, localY);
            // Asignar bioma a TODAS las celdas, incluyendo las que luego tendrán agua
            tile.setBiomeId(biomeId);
        }
    }
}

// ----- Métodos Helper -----
int WorldGenerator::findClosestBiome(float worldX, float worldY) const {
    int bestBiomeId = 0;
    float bestScore = -std::numeric_limits<float>::max();
    
    for (const auto& seed : _biomeSeeds) {
        float score = calculateBiomeInfluence(seed, worldX, worldY);
        if (score > bestScore) {
            bestScore = score;
            bestBiomeId = seed.biomeId;
        }
    }
    
    return bestBiomeId;
}

float WorldGenerator::calculateBiomeInfluence(const BiomeSeed& seed, float x, float y) const {
    float dx = x - seed.x;
    float dy = y - seed.y;
    float distanceSq = dx * dx + dy * dy;
    
    return seed.strength / (1.0f + distanceSq * 0.0001f);
}