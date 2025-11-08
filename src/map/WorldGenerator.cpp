#include "map/WorldGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <numbers>

// ----- Constructores -----
WorldGenerator::WorldGenerator() 
    : _worldSeed(12345), _rng(_worldSeed) {
    
    _biomeSystem = std::make_shared<BiomeSystem>(static_cast<uint32_t>(_worldSeed));
    _globalNoise = std::make_shared<PerlinNoise>(_worldSeed);
    initializeDefaultBiomes();
    initializeBiomeSeeds();
}

WorldGenerator::WorldGenerator(uint64_t worldSeed) 
    : _worldSeed(worldSeed), _rng(_worldSeed) {
    
    _biomeSystem = std::make_shared<BiomeSystem>(static_cast<uint32_t>(_worldSeed));
    _globalNoise = std::make_shared<PerlinNoise>(_worldSeed);
    initializeDefaultBiomes();
    initializeBiomeSeeds();
}

WorldGenerator::WorldGenerator(uint64_t worldSeed, std::shared_ptr<BiomeSystem> biomeSystem)
    : _worldSeed(worldSeed), _rng(_worldSeed), _biomeSystem(std::move(biomeSystem)) {
    
    if (!_biomeSystem) {
        _biomeSystem = std::make_shared<BiomeSystem>(static_cast<uint32_t>(_worldSeed));
        initializeDefaultBiomes();
    }
    _globalNoise = std::make_shared<PerlinNoise>(_worldSeed);
    initializeBiomeSeeds();
}

WorldGenerator::WorldGenerator(uint64_t worldSeed,  const LakeConfig& lake_config)
    : _worldSeed(worldSeed), _rng(_worldSeed), _lakeConfig(lake_config) {
    
    _biomeSystem = std::make_shared<BiomeSystem>(static_cast<uint32_t>(_worldSeed));
    _globalNoise = std::make_shared<PerlinNoise>(_worldSeed);
    initializeBiomeSeeds();
}

WorldGenerator::WorldGenerator(uint64_t worldSeed, std::shared_ptr<BiomeSystem> biomeSystem, const LakeConfig& lake_config)
    : _worldSeed(worldSeed), _rng(_worldSeed), _biomeSystem(std::move(biomeSystem)), _lakeConfig(lake_config) {
    
    if (!_biomeSystem) {
        _biomeSystem = std::make_shared<BiomeSystem>(static_cast<uint32_t>(_worldSeed));
        initializeDefaultBiomes();
    }
    _globalNoise = std::make_shared<PerlinNoise>(_worldSeed);
    initializeBiomeSeeds();
}

WorldGenerator::WorldGenerator(const WorldGenerator& other)
    : _worldSeed(other._worldSeed),
      _rng(other._worldSeed),
      _biomeSystem(std::make_shared<BiomeSystem>(*other._biomeSystem)),
      _lakeConfig(other._lakeConfig) {  // Añadir _riverConfig
    
    _biomeSeeds = other._biomeSeeds;
    _globalNoise = std::make_shared<PerlinNoise>(_worldSeed);
}

WorldGenerator::WorldGenerator(WorldGenerator&& other) noexcept
    : _worldSeed(std::exchange(other._worldSeed, 12345)),
      _rng(std::move(other._rng)),
      _biomeSystem(std::move(other._biomeSystem)),
      _biomeSeeds(std::move(other._biomeSeeds)),
      _lakeConfig(std::move(other._lakeConfig)),
      _globalNoise(std::move(other._globalNoise)){}

// ----- Operadores -----
WorldGenerator& WorldGenerator::operator=(const WorldGenerator& other) {
    if (this != &other) {
        _worldSeed = other._worldSeed;
        _rng = std::mt19937_64(_worldSeed);
        _biomeSystem = std::make_shared<BiomeSystem>(*other._biomeSystem);
        _biomeSeeds = other._biomeSeeds;
        _lakeConfig = other._lakeConfig;
        _globalNoise = std::make_shared<PerlinNoise>(_worldSeed);
    }
    return *this;
}

WorldGenerator& WorldGenerator::operator=(WorldGenerator&& other) noexcept {
    if (this != &other) {
        _worldSeed = std::exchange(other._worldSeed, 12345);
        _rng = std::move(other._rng);
        _biomeSystem = std::move(other._biomeSystem);
        _biomeSeeds = std::move(other._biomeSeeds);
        _lakeConfig = std::move(other._lakeConfig);
        _globalNoise = std::move(other._globalNoise);
    }
    return *this;
}

// ----- Métodos públicos -----
std::shared_ptr<Chunk> WorldGenerator::generateChunk(int chunkX, int chunkY, uint32_t chunkSize) {
    auto chunk = std::make_shared<Chunk>(chunkX, chunkY, chunkSize);
    
    assignBiomesToChunk(*chunk);
    generateLakes(*chunk);

    chunk->setState(State::LOADED);
    return chunk;
}

void WorldGenerator::setBiomeSystem(std::shared_ptr<BiomeSystem> biomeSystem) {
    _biomeSystem = std::move(biomeSystem);
    initializeBiomeSeeds();
}

void WorldGenerator::actualizarEstadosBiomas(float horaDelDia) {
    if (_biomeSystem) {
        _biomeSystem->actualizarTodosBiomas(horaDelDia);
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
            float lakeNoise = _globalNoise->noise(
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

// ----- Inicialización de Biomas -----
void WorldGenerator::initializeDefaultBiomes() {
    if (!_biomeSystem || _biomeSystem->getCantidadBiomas() > 0) {
        return;
    }
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        0, "Llanura", Color{124, 252, 0},
        22.0f, 45.0f, 70.0f, 1.0f, 0.2f, 0.2f,
        6.0f, 15.0f, 12.0f, 0.15f, 0.08f, 0.07f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        1, "Bosque", Color{34, 139, 34},
        18.0f, 65.0f, 50.0f, 1.3f, 0.15f, 0.1f,
        4.0f, 12.0f, 10.0f, 0.25f, 0.06f, 0.04f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        2, "Colinas", Color{139, 115, 85},
        16.0f, 55.0f, 65.0f, 1.4f, 0.1f, 0.15f,
        5.0f, 10.0f, 8.0f, 0.3f, 0.05f, 0.06f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        3, "Pantano", Color{48, 128, 20},
        20.0f, 85.0f, 45.0f, 1.6f, 0.25f, 0.05f,
        3.0f, 8.0f, 6.0f, 0.4f, 0.12f, 0.02f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        4, "Desierto", Color{255, 230, 150},
        35.0f, 15.0f, 85.0f, 1.5f, 0.05f, 0.3f,
        8.0f, 5.0f, 10.0f, 0.25f, 0.03f, 0.12f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        5, "Sabana", Color{210, 180, 140},
        28.0f, 35.0f, 75.0f, 1.2f, 0.12f, 0.18f,
        7.0f, 10.0f, 9.0f, 0.2f, 0.07f, 0.08f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        6, "Tundra", Color{200, 220, 240},
        5.0f, 60.0f, 40.0f, 1.3f, 0.08f, 0.12f,
        4.0f, 12.0f, 7.0f, 0.25f, 0.04f, 0.05f,
        static_cast<uint32_t>(_worldSeed)
    ));
}

void WorldGenerator::initializeBiomeSeeds(float Area) {
    if (!_biomeSystem) return;
    
    _biomeSeeds.clear();
    std::uniform_real_distribution<float> posDist(-Area/2, Area/2);
    std::uniform_real_distribution<float> strengthDist(0.5f, 2.0f);
    std::uniform_int_distribution<int> biomeDist(0, _biomeSystem->getCantidadBiomas() - 1);
    
    for (int i = 0; i < Area/10; ++i) {
        _biomeSeeds.push_back(BiomeSeed(
            biomeDist(_rng),
            posDist(_rng),
            posDist(_rng),
            strengthDist(_rng)
        ));
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

uint64_t WorldGenerator::getChunkSeed(int chunkX, int chunkY) const {
    return _worldSeed ^ (static_cast<uint64_t>(chunkX) << 32) ^ static_cast<uint64_t>(chunkY);
}

float WorldGenerator::getChunkWorldX(const Chunk& chunk) const {
    auto worldPos = chunk.localToWorld(0, 0);
    return static_cast<float>(worldPos.first());
}

float WorldGenerator::getChunkWorldY(const Chunk& chunk) const {
    auto worldPos = chunk.localToWorld(0, 0);
    return static_cast<float>(worldPos.second());
}

bool WorldGenerator::worldToChunkLocal(const Chunk& chunk, float worldX, float worldY, int& localX, int& localY) const {
    auto chunkWorldPos = chunk.localToWorld(0, 0);
    int chunkWorldX = chunkWorldPos.first();
    int chunkWorldY = chunkWorldPos.second();
    uint32_t chunkSize = chunk.getChunkSize();
    
    localX = static_cast<int>(worldX) - chunkWorldX;
    localY = static_cast<int>(worldY) - chunkWorldY;
    
    return (localX >= 0 && localX < static_cast<int>(chunkSize) && 
            localY >= 0 && localY < static_cast<int>(chunkSize));
}

