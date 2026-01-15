#include <cmath>
#include <algorithm>
#include <numbers>
#include <iostream>

#include "map/generator/WorldGenerator.hpp"

// ----- Constructores -----
WorldGenerator::WorldGenerator(const DynamicArray<int>& biomeIds, 
                             uint64_t worldSeed, 
                             float biomeRadiusInMeters, 
                             float metersPerTile) 
    : _worldSeed(worldSeed), 
      _rng(_worldSeed),
      _biomeIds(biomeIds) {
    
    // Validación básica
    if (_biomeIds.empty()) {
        throw std::runtime_error("WorldGenerator: At least one biome ID is required");
    }
    
    _biomeRadius = biomeRadiusInMeters / metersPerTile;
    updateCellSize();
    _globalNoise = PerlinNoise(_worldSeed);
}

WorldGenerator::WorldGenerator(const DynamicArray<int>& biomeIds, 
                             const LakeConfig& lake_config, 
                             uint64_t worldSeed,
                             float biomeRadiusInMeters, 
                             float metersPerTile)
    : _worldSeed(worldSeed), 
      _rng(_worldSeed),
      _lakeConfig(lake_config),
      _biomeIds(biomeIds) {
    
    if (_biomeIds.empty()) {
        throw std::runtime_error("WorldGenerator: At least one biome ID is required");
    }
    
    _biomeRadius = biomeRadiusInMeters / metersPerTile;
    updateCellSize();
    _globalNoise = PerlinNoise(_worldSeed);
}

WorldGenerator::WorldGenerator(WorldGenerator&& other) noexcept
    : _worldSeed(other._worldSeed),
      _rng(std::move(other._rng)),
      _biomeRadius(other._biomeRadius),
      _cellSize(other._cellSize),
      _seedGrid(std::move(other._seedGrid)),
      _lakeConfig(std::move(other._lakeConfig)),
      _globalNoise(std::move(other._globalNoise)),
      _biomeIds(std::move(other._biomeIds)) {}

// ----- Operadores -----
WorldGenerator& WorldGenerator::operator=(WorldGenerator&& other) noexcept {
    if (this != &other) {
        _worldSeed = other._worldSeed;
        _rng = std::move(other._rng);
        _biomeRadius = other._biomeRadius;
        _cellSize = other._cellSize;
        _seedGrid = std::move(other._seedGrid);
        _lakeConfig = std::move(other._lakeConfig);
        _globalNoise = std::move(other._globalNoise);
        _biomeIds = std::move(other._biomeIds);
    }
    return *this;
}

// ----- Métodos públicos -----
std::unique_ptr<Chunk> WorldGenerator::generateChunk(ChunkCoord coord, uint32_t chunkSize) {
    auto chunk = std::make_unique<Chunk>(coord, chunkSize);
    
    // Generar semillas para esta región (con margen)
    generateSeedsForRegion(coord, chunkSize);
    
    // Asignar biomas
    assignBiomesToChunk(*chunk);
    
    // Generar lagos
    generateLakes(*chunk);
    
    chunk->setState(State::LOADED);
    
    cleanupOldSeeds();

    return chunk;
}

std::unique_ptr<Chunk> WorldGenerator::generateChunk(int chunkX, int chunkY, uint32_t chunkSize) {
    ChunkCoord coord(chunkX,chunkY);
    return generateChunk(coord,chunkSize);
}

// Setters
void WorldGenerator::setWorldSeed(uint64_t worldSeed) {
    _worldSeed = worldSeed;
    _rng.seed(_worldSeed);
    _globalNoise = PerlinNoise(_worldSeed);
    _seedGrid.clear();  // Reiniciar grid
}

void WorldGenerator::setBiomeRadius(float radiusInMeters, float metersPerTile) {
    _biomeRadius = radiusInMeters / metersPerTile;
    updateCellSize();
    _seedGrid.clear();  // Reiniciar grid
}

// ----- Metodos Poisson Disk -----
// Generacion de semillas
void WorldGenerator::generateSeedsForRegion(const ChunkCoord& coord, uint32_t chunkSize) {
    // Convertir coordenadas de chunk a tiles del mundo
    float minTileX = static_cast<float>(coord.x() * chunkSize);
    float minTileY = static_cast<float>(coord.y() * chunkSize);
    float maxTileX = minTileX + static_cast<float>(chunkSize);
    float maxTileY = minTileY + static_cast<float>(chunkSize);
    
    // Expandir región para incluir semillas vecinas
    float expand = _biomeRadius * 2.0f;
    
    // Calcular celdas que cubren la región expandida
    Pair<int, int> startCell = worldToCellCoords(minTileX - expand, minTileY - expand);
    Pair<int, int> endCell = worldToCellCoords(maxTileX + expand, maxTileY + expand);
    
    // Generar semillas para cada celda necesaria
    for (int cellY = startCell.second(); cellY <= endCell.second(); ++cellY) {
        for (int cellX = startCell.first(); cellX <= endCell.first(); ++cellX) {
            int64_t cellIndex = calculateCellIndex(cellX, cellY);
            
            // Solo generar si no existe
            if (!_seedGrid.contains(cellIndex)) {
                generateSeedsForCell(cellX, cellY);
            }
        }
    }
}

void WorldGenerator::generateSeedsForCell(int cellX, int cellY) {
    auto cellRng = createCellRNG(cellX, cellY);
    
    std::uniform_real_distribution<float> posDist(0.0f, _cellSize);
    std::uniform_real_distribution<float> strengthDist(0.5f, 2.0f);
    
    // CORRECCIÓN: Manejar correctamente coordenadas negativas
    float cellWorldX = static_cast<float>(cellX) * _cellSize;
    float cellWorldY = static_cast<float>(cellY) * _cellSize;
    
    // Centro de la celda en coordenadas mundiales (puede ser negativo)
    float cellCenterX = cellWorldX + (_cellSize / 2.0f);
    float cellCenterY = cellWorldY + (_cellSize / 2.0f);
    
    DynamicArray<BiomeSeed> seeds;
    
    std::uniform_int_distribution<int> countDist(1, 3);
    int attempts = countDist(cellRng);
    
    for (int i = 0; i < attempts; ++i) {
        float offsetX = posDist(cellRng) - (_cellSize / 2.0f);
        float offsetY = posDist(cellRng) - (_cellSize / 2.0f);
        
        // Posición absoluta en el mundo (puede ser negativa)
        float seedX = cellCenterX + offsetX;
        float seedY = cellCenterY + offsetY;
        
        if (isValidSeedPosition(seedX, seedY, cellX, cellY)) {
            std::uniform_int_distribution<size_t> biomeDist(0, _biomeIds.size() - 1);
            int biomeId = _biomeIds[biomeDist(cellRng)];
            float strength = strengthDist(cellRng);
            
            seeds.push_back(BiomeSeed(biomeId, seedX, seedY, strength));
        }
    }
    
    if (!seeds.empty()) {
        _seedGrid[calculateCellIndex(cellX, cellY)] = std::move(seeds);
    }
}


bool WorldGenerator::isValidSeedPosition(float tileX, float tileY, int cellX, int cellY) const {
    float minDistSq = _biomeRadius * _biomeRadius;
    
    // Verificar celdas vecinas 3x3
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int neighborCellX = cellX + dx;
            int neighborCellY = cellY + dy;
            
            int64_t neighborIndex = calculateCellIndex(neighborCellX, neighborCellY);
            auto it = _seedGrid.find_ptr(neighborIndex);
            
            if (it != nullptr) {
                for (const BiomeSeed& seed : *it) {
                    float dx2 = tileX - seed.x;
                    float dy2 = tileY - seed.y;
                    float distSq = dx2 * dx2 + dy2 * dy2;
                    
                    if (distSq < minDistSq) {
                        return false;  // Colisión
                    }
                }
            }
        }
    }
    
    return true;
}

// Asignacion
void WorldGenerator::assignBiomesToChunk(Chunk& chunk) {
    // Obtener semillas relevantes para este chunk
    auto seeds = collectSeedsForChunk(chunk);
    
    // Procesar cada tile del chunk
    for (uint32_t y = 0; y < chunk.getChunkSize(); ++y) {
        for (uint32_t x = 0; x < chunk.getChunkSize(); ++x) {
            auto worldPos = chunk.localToWorld(x, y);
            float tileX = static_cast<float>(worldPos.first());
            float tileY = static_cast<float>(worldPos.second());
            
            // Encontrar bioma dominante
            int biomeId = selectDominantBiome(tileX, tileY, seeds);
            
            // Asignar (siempre hay al menos un bioma)
            chunk.at(x, y).setBiomeId(biomeId);
        }
    }
}

DynamicArray<BiomeSeed> WorldGenerator::collectSeedsForChunk(const Chunk& chunk) const {
    DynamicArray<BiomeSeed> result;
    
    // CORRECCIÓN: Usar int64_t para evitar problemas de signo
    int64_t chunkX = chunk.getChunkCoord().x();
    int64_t chunkY = chunk.getChunkCoord().y();
    uint32_t chunkSize = chunk.getChunkSize();
    
    float minTileX = static_cast<float>(chunkX * chunkSize);
    float minTileY = static_cast<float>(chunkY * chunkSize);
    float maxTileX = minTileX + static_cast<float>(chunkSize);
    float maxTileY = minTileY + static_cast<float>(chunkSize);
    
    Pair<int, int> startCell = worldToCellCoords(minTileX - _biomeRadius, minTileY - _biomeRadius);
    Pair<int, int> endCell = worldToCellCoords(maxTileX + _biomeRadius, maxTileY + _biomeRadius);
    
    for (int cellY = startCell.second(); cellY <= endCell.second(); ++cellY) {
        for (int cellX = startCell.first(); cellX <= endCell.first(); ++cellX) {
            int64_t cellIndex = calculateCellIndex(cellX, cellY);
            auto it = _seedGrid.find_ptr(cellIndex);
            
            if (it != nullptr) {
                for (const BiomeSeed& seed : *it) {
                    // CORRECCIÓN: Comparación correcta con coordenadas que pueden ser negativas
                    if (seed.x >= (minTileX - _biomeRadius) && seed.x <= (maxTileX + _biomeRadius) &&
                        seed.y >= (minTileY - _biomeRadius) && seed.y <= (maxTileY + _biomeRadius)) {
                        result.push_back(seed);
                    }
                }
            }
        }
    }
    
    if (result.empty()) {
        float centerX = (minTileX + maxTileX) / 2.0f;
        float centerY = (minTileY + maxTileY) / 2.0f;
        result.push_back(BiomeSeed(_biomeIds[0], centerX, centerY, 1.0f));
    }
    
    return result;
}

int WorldGenerator::selectDominantBiome(float tileX, float tileY, 
                                      const DynamicArray<BiomeSeed>& seeds) const {
    int bestBiomeId = _biomeIds[0];  // Default al primer bioma
    float bestInfluence = -1.0f;
    
    for (const BiomeSeed& seed : seeds) {
        float influence = calculateBiomeInfluence(seed, tileX, tileY);
        if (influence > bestInfluence) {
            bestInfluence = influence;
            bestBiomeId = seed.biomeId;
        }
    }
    
    return bestBiomeId;
}

float WorldGenerator::calculateBiomeInfluence(const BiomeSeed& seed, 
                                            float tileX, float tileY) const {
    float dx = tileX - seed.x;
    float dy = tileY - seed.y;
    float distanceSq = dx * dx + dy * dy;
    
    // Influencia inversamente proporcional a la distancia
    return seed.strength / (1.0f + distanceSq * 0.0001f);
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

// ----- Métodos Helper -----
void WorldGenerator::updateCellSize() {
    _cellSize = _biomeRadius / std::sqrt(2.0f);
}

int64_t WorldGenerator::calculateCellIndex(int cellX, int cellY) const {
    uint64_t ux = static_cast<uint64_t>((cellX << 1) ^ (cellX >> 31));
    uint64_t uy = static_cast<uint64_t>((cellY << 1) ^ (cellY >> 31));
    
    return static_cast<int64_t>((ux << 32) | uy);
}

Pair<int, int> WorldGenerator::worldToCellCoords(float worldX, float worldY) const {
    return Pair<int, int>(
        static_cast<int>(std::floor(worldX / _cellSize)),
        static_cast<int>(std::floor(worldY / _cellSize))
    );
}

std::mt19937_64 WorldGenerator::createCellRNG(int cellX, int cellY) const {
    // Usar un método seguro para manejar negativos
    uint64_t encodedX = static_cast<uint64_t>((cellX << 1) ^ (cellX >> 63));
    uint64_t encodedY = static_cast<uint64_t>((cellY << 1) ^ (cellY >> 63));
    
    uint64_t cellSeed = _worldSeed ^ 
                       (encodedX << 32) ^ 
                       (encodedY & 0xFFFFFFFF);
    return std::mt19937_64(cellSeed);
}

void WorldGenerator::cleanupOldSeeds() {
    // Limpiar periódicamente (cada 100 chunks, por ejemplo)
    static int cleanupCounter = 0;
    cleanupCounter++;
    
    if (cleanupCounter > 100 && _seedGrid.size() > 1000) {
        // Mantener solo las 500 celdas más recientes
        if (_seedGrid.size() > 500) {
            auto it = _seedGrid.begin();

            for(int i = 0; i < 500; ++i){
                ++it;
                if(it == _seedGrid.end()){
                    break;
                }
            }
            
            while(it != _seedGrid.end()){
                it = _seedGrid.erase(it);
            }
        }
        cleanupCounter = 0;
    }
}