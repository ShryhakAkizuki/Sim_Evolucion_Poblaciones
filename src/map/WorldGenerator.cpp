#include "map/WorldGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <numbers>

// ----- Constructores -----
// Constructor por defecto
WorldGenerator::WorldGenerator() 
    : _worldSeed(12345), _rng(_worldSeed) {
    
    _biomeSystem = std::make_shared<BiomeSystem>(static_cast<uint32_t>(_worldSeed));
    initializeDefaultBiomes();
    initializeBiomeSeeds();
}

// Constructor con semilla
WorldGenerator::WorldGenerator(uint64_t worldSeed) 
    : _worldSeed(worldSeed), _rng(_worldSeed) {
    
    _biomeSystem = std::make_shared<BiomeSystem>(static_cast<uint32_t>(_worldSeed));
    initializeDefaultBiomes();
    initializeBiomeSeeds();
}

// Constructor con BiomeSystem existente
WorldGenerator::WorldGenerator(uint64_t worldSeed, std::shared_ptr<BiomeSystem> biomeSystem)
    : _worldSeed(worldSeed), _rng(_worldSeed), _biomeSystem(std::move(biomeSystem)) {
    
    if (!_biomeSystem) {
        _biomeSystem = std::make_shared<BiomeSystem>(static_cast<uint32_t>(_worldSeed));
        initializeDefaultBiomes();
    }
    initializeBiomeSeeds();
}

// Constructor de copia
WorldGenerator::WorldGenerator(const WorldGenerator& other)
    : _worldSeed(other._worldSeed),
      _rng(other._worldSeed), // Reinicializar RNG con misma semilla
      _biomeSystem(std::make_shared<BiomeSystem>(*other._biomeSystem)),
      _riverFrequency(other._riverFrequency){
    
    // Copiar semillas de biomas
    _biomeSeeds = other._biomeSeeds;
}

// Constructor de movimiento
WorldGenerator::WorldGenerator(WorldGenerator&& other) noexcept
    : _worldSeed(std::exchange(other._worldSeed, 12345)),
      _rng(std::move(other._rng)),
      _biomeSystem(std::move(other._biomeSystem)),
      _biomeSeeds(std::move(other._biomeSeeds)),
      _riverFrequency(std::exchange(other._riverFrequency, 0.05f)) {}

// ----- Operadores -----
// Asignacion por copia
WorldGenerator& WorldGenerator::operator=(const WorldGenerator& other) {
    if (this != &other) {
        _worldSeed = other._worldSeed;
        _rng = std::mt19937_64(_worldSeed); // Reinicializar
        _biomeSystem = std::make_shared<BiomeSystem>(*other._biomeSystem);
        _biomeSeeds = other._biomeSeeds;
        _riverFrequency = other._riverFrequency;
    }
    return *this;
}

// Asignacion por movimiento
WorldGenerator& WorldGenerator::operator=(WorldGenerator&& other) noexcept {
    if (this != &other) {
        _worldSeed = std::exchange(other._worldSeed, 12345);
        _rng = std::move(other._rng);
        _biomeSystem = std::move(other._biomeSystem);
        _biomeSeeds = std::move(other._biomeSeeds);
        _riverFrequency = std::exchange(other._riverFrequency, 0.05f);
    }
    return *this;
}

// ----- Métodos públicos -----
std::shared_ptr<Chunk> WorldGenerator::generateChunk(int chunkX, int chunkY, uint32_t chunkSize) {
    auto chunk = std::make_shared<Chunk>(chunkX, chunkY, chunkSize);
    
    // Pipeline de generación
    assignBiomesToChunk(*chunk);
    generateRivers(*chunk);
    generateLakes(*chunk);
    
    chunk->setState(State::LOADED);
    return chunk;
}

void WorldGenerator::setBiomeSystem(std::shared_ptr<BiomeSystem> biomeSystem) {
    _biomeSystem = std::move(biomeSystem);
    initializeBiomeSeeds(); // Regenerar semillas con nuevo sistema
}

void WorldGenerator::actualizarEstadosBiomas(float horaDelDia) {
    if (_biomeSystem) {
        _biomeSystem->actualizarTodosBiomas(horaDelDia);
    }
}

// ----- Métodos privados -----

void WorldGenerator::initializeDefaultBiomes() {
    if (!_biomeSystem || _biomeSystem->getCantidadBiomas() > 0) {
        return; // Ya tiene biomas o no hay sistema
    }
    
    // Registrar biomas por defecto
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        0, "Océano", Color{0, 0, 128},
        15.0f, 90.0f, 50.0f, 0.5f, 0.1f, 0.8f,
        2.0f, 5.0f, 20.0f, 0.1f, 0.05f, 0.1f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        1, "Playa", Color{238, 214, 175},
        25.0f, 60.0f, 80.0f, 0.8f, 0.2f, 0.5f,
        5.0f, 10.0f, 15.0f, 0.2f, 0.1f, 0.08f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        2, "Bosque", Color{34, 139, 34},
        20.0f, 70.0f, 60.0f, 1.2f, 0.3f, 0.2f,
        5.0f, 10.0f, 15.0f, 0.3f, 0.1f, 0.05f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        3, "Pradera", Color{124, 252, 0},
        22.0f, 50.0f, 70.0f, 1.0f, 0.15f, 0.3f,
        6.0f, 15.0f, 12.0f, 0.15f, 0.08f, 0.07f,
        static_cast<uint32_t>(_worldSeed)
    ));
    
    _biomeSystem->registrarBioma(std::make_shared<Bioma>(
        4, "Desierto", Color{255, 255, 0},
        35.0f, 10.0f, 90.0f, 1.5f, 0.05f, 0.9f,
        8.0f, 5.0f, 10.0f, 0.25f, 0.03f, 0.12f,
        static_cast<uint32_t>(_worldSeed)
    ));
}

void WorldGenerator::initializeBiomeSeeds() {
    if (!_biomeSystem) return;
    
    _biomeSeeds.clear();
    std::uniform_real_distribution<float> posDist(-500.0f, 500.0f);
    std::uniform_real_distribution<float> strengthDist(0.5f, 2.0f);
    std::uniform_int_distribution<int> biomeDist(0, _biomeSystem->getCantidadBiomas() - 1);
    
    // Generar 100 semillas de Voronoi
    for (int i = 0; i < 100; ++i) {
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
            tile.setBiomeId(biomeId);
            
            // Océano automáticamente tiene agua
            if (biomeId == 0) {
                tile.setHasWater(true);
            }
        }
    }
}

void WorldGenerator::generateRivers(Chunk& chunk) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    uint64_t chunkSeed = getChunkSeed(chunk.getChunkX(), chunk.getChunkY());
    std::mt19937_64 localRNG(chunkSeed);
    
    for (uint32_t localY = 0; localY < chunk.getChunkSize(); ++localY) {
        for (uint32_t localX = 0; localX < chunk.getChunkSize(); ++localX) {
            Tile& tile = chunk.at(localX, localY);
            if (tile.hasWater()) continue; 
            
            auto worldPos = chunk.localToWorld(localX, localY);
            
            // Generar ríos usando noise
            float riverNoise = getNoise2D(worldPos.first(), worldPos.second(), 0.02f, 3);
            if (std::abs(riverNoise) < 0.08f && dist(localRNG) < _riverFrequency) {
                tile.setHasWater(true);
            }
        }
    }
}

void WorldGenerator::generateLakes(Chunk& chunk) {
    uint64_t chunkSeed = getChunkSeed(chunk.getChunkX(), chunk.getChunkY());
    std::mt19937_64 localRNG(chunkSeed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (uint32_t localY = 0; localY < chunk.getChunkSize(); ++localY) {
        for (uint32_t localX = 0; localX < chunk.getChunkSize(); ++localX) {
            Tile& tile = chunk.at(localX, localY);
            if (tile.hasWater()) continue;
            
            auto worldPos = chunk.localToWorld(localX, localY);
            
            // Lagos - áreas circulares de agua
            float lakeNoise = getNoise2D(worldPos.first() * 0.5f, worldPos.second() * 0.5f, 0.01f, 2);
            if (lakeNoise > 0.7f && dist(localRNG) < 0.02f) {
                tile.setHasWater(true);
                
                // Hacer el lago más grande (radio 2)
                for (int dy = -2; dy <= 2; ++dy) {
                    for (int dx = -2; dx <= 2; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        
                        int nx = localX + dx;
                        int ny = localY + dy;
                        if (nx >= 0 && nx < static_cast<int>(chunk.getChunkSize()) &&
                            ny >= 0 && ny < static_cast<int>(chunk.getChunkSize())) {
                            
                            float distSq = dx*dx + dy*dy;
                            if (distSq <= 4.0f && dist(localRNG) < 0.7f) {
                                chunk.at(nx, ny).setHasWater(true);
                            }
                        }
                    }
                }
            }
        }
    }
}

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
    
    // Influencia inversamente proporcional a la distancia
    return seed.strength / (1.0f + distanceSq * 0.0001f);
}

float WorldGenerator::getNoise2D(float x, float y, float scale, int octaves) const {
    float value = 0.0f;
    float amplitude = 1.0f;
    float maxAmplitude = 0.0f;
    float frequency = scale;
    
    for (int i = 0; i < octaves; i++) {
        // Noise simple usando funciones trigonométricas
        float noise = std::sin(x * frequency) * std::cos(y * frequency);
        value += noise * amplitude;
        maxAmplitude += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    
    return value / maxAmplitude;
}

uint64_t WorldGenerator::getChunkSeed(int chunkX, int chunkY) const {
    return _worldSeed ^ (static_cast<uint64_t>(chunkX) << 32) ^ static_cast<uint64_t>(chunkY);
}