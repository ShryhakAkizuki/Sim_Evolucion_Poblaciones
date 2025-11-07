#include "visualization/MapRenderer.hpp"
#include <iostream>

MapRenderer::MapRenderer(ChunkManager& chunkManager)
    : _chunkManager(chunkManager), _tileDrawer() {
    std::cout << "MapRenderer inicializado\n";
}

void MapRenderer::render(sf::RenderTarget& target, const CameraController& camera) {
    // Usar getVisibleWorldBounds() sin parámetros
    sf::FloatRect visibleBounds = camera.getVisibleWorldBounds();
    
    // Calcular rango de chunks visibles
    int chunkSize = static_cast<int>(_chunkManager.getChunkSize());
    int startChunkX = static_cast<int>(std::floor(visibleBounds.left / chunkSize));
    int startChunkY = static_cast<int>(std::floor(visibleBounds.top / chunkSize));
    int endChunkX = static_cast<int>(std::ceil((visibleBounds.left + visibleBounds.width) / chunkSize));
    int endChunkY = static_cast<int>(std::ceil((visibleBounds.top + visibleBounds.height) / chunkSize));
    
    int renderedChunks = 0;
    int renderedTiles = 0;
    
    // Renderizar chunks visibles
    for (int chunkX = startChunkX; chunkX <= endChunkX; ++chunkX) {
        for (int chunkY = startChunkY; chunkY <= endChunkY; ++chunkY) {
            ChunkCoord coord(chunkX, chunkY);
            
            if (_chunkManager.HasChunk(coord)) {
                auto chunk = _chunkManager.GetChunk(coord);
                if (chunk) {
                    renderChunk(target, chunk, camera);
                    renderedChunks++;
                    renderedTiles += chunk->getChunkSize() * chunk->getChunkSize();
                }
            }
        }
    }
    
    // Debug opcional
    // std::cout << "Rendered " << renderedChunks << " chunks, " << renderedTiles << " tiles\n";
}

void MapRenderer::renderChunk(sf::RenderTarget& target, const std::shared_ptr<Chunk>& chunk, 
                              const CameraController& camera) {
    float tileSize = 1.0f; // Cada tile es 1x1 unidad mundial
    
    // Verificar si el chunk está en cache
    ChunkCoord coord(chunk->getChunkX(), chunk->getChunkY());
    auto cacheIt = _chunkCache.find(coord);
    
    if (cacheIt == _chunkCache.end()) {
        // Crear cache para este chunk
        sf::VertexArray chunkVertices = _tileDrawer.createChunkVertices(chunk, tileSize);
        _chunkCache[coord] = chunkVertices;
        cacheIt = _chunkCache.find(coord);
    }
    
    // Dibujar el chunk desde cache
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