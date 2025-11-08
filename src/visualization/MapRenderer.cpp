#include "visualization/MapRenderer.hpp"
#include <iostream>

MapRenderer::MapRenderer(ChunkManager& chunkManager)
    : _chunkManager(chunkManager), _tileDrawer() {  // TileDrawer sin biomeSystem inicial
    
    // Intentar obtener el sistema de biomas del ChunkManager
    auto worldGenerator = _chunkManager.getWorldGenerator();
    if (worldGenerator) {
        auto biomeSystem = worldGenerator->getBiomeSystem();
        _tileDrawer.setBiomeSystem(biomeSystem);
    }
    
    std::cout << "MapRenderer inicializado\n";
}

void MapRenderer::render(sf::RenderTarget& target, const CameraController& camera) {
    sf::FloatRect visibleBounds = camera.getVisibleWorldBounds();
    
    // Calcular rango de chunks visibles de forma más precisa
    int chunkSize = static_cast<int>(_chunkManager.getChunkSize());
    int startChunkX = static_cast<int>(std::floor(visibleBounds.left / chunkSize));
    int startChunkY = static_cast<int>(std::floor(visibleBounds.top / chunkSize));
    int endChunkX = static_cast<int>(std::ceil((visibleBounds.left + visibleBounds.width) / chunkSize));
    int endChunkY = static_cast<int>(std::ceil((visibleBounds.top + visibleBounds.height) / chunkSize));
    
    // Limitar el rango para evitar bucles excesivos
    startChunkX = std::max(startChunkX, -100);
    startChunkY = std::max(startChunkY, -100);
    endChunkX = std::min(endChunkX, 100);
    endChunkY = std::min(endChunkY, 100);
    
    // Renderizar chunks visibles
    for (int chunkX = startChunkX; chunkX <= endChunkX; ++chunkX) {
        for (int chunkY = startChunkY; chunkY <= endChunkY; ++chunkY) {
            ChunkCoord coord(chunkX, chunkY);
            
            if (_chunkManager.HasChunk(coord)) {
                auto chunk = _chunkManager.GetChunk(coord);
                if (chunk) {
                    renderChunk(target, chunk);
                }
            }
        }
    }
}

void MapRenderer::renderChunk(sf::RenderTarget& target, const std::shared_ptr<Chunk>& chunk) {
    float tileSize = 1.0f;
    
    ChunkCoord coord(chunk->getChunkX(), chunk->getChunkY());
    auto cacheIt = _chunkCache.find(coord);
    
    if (cacheIt == _chunkCache.end()) {
        // Crear cache para este chunk
        sf::VertexArray chunkVertices = _tileDrawer.createChunkVertices(chunk, tileSize);
        _chunkCache[coord] = chunkVertices;
        cacheIt = _chunkCache.find(coord);
    }
    
    if (cacheIt != _chunkCache.end()) {
        target.draw(cacheIt->second);
    }
}

void MapRenderer::renderTile(sf::RenderTarget& target, const Tile& tile, 
                            float worldX, float worldY, float tileSize) {
    sf::RectangleShape tileShape(sf::Vector2f(tileSize, tileSize));
    tileShape.setPosition(worldX, worldY);
    tileShape.setFillColor(_tileDrawer.getTileColor(tile));
    tileShape.setOutlineThickness(0.1f);
    tileShape.setOutlineColor(sf::Color(0, 0, 0, 50));
    
    target.draw(tileShape);
}

void MapRenderer::clearCache() {
    _chunkCache.clear();
    std::cout << "Cache de MapRenderer limpiado\n";
}

void MapRenderer::updateCacheForChunk(const ChunkCoord& coord) {
    if (_chunkManager.HasChunk(coord)) {
        auto chunk = _chunkManager.GetChunk(coord);
        float tileSize = 1.0f;
        sf::VertexArray chunkVertices = _tileDrawer.createChunkVertices(chunk, tileSize);
        _chunkCache[coord] = chunkVertices;
    }
}