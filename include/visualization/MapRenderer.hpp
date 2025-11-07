#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "map/ChunkManager.hpp"
#include "visualization/TileDrawer.hpp"
#include "visualization/CameraController.hpp"

class MapRenderer {
private:
    ChunkManager& _chunkManager;
    TileDrawer _tileDrawer;
    
    // Cache de sprites para optimización
    std::unordered_map<ChunkCoord, sf::VertexArray> _chunkCache;
    
    void renderChunk(sf::RenderTarget& target, const std::shared_ptr<Chunk>& chunk, 
                    const CameraController& camera);
    void renderTile(sf::RenderTarget& target, const Tile& tile, 
                   float worldX, float worldY, float tileSize);
    
public:
    MapRenderer(ChunkManager& chunkManager);
    
    void render(sf::RenderTarget& target, const CameraController& camera);
    void clearCache();
    void updateCacheForChunk(const ChunkCoord& coord);
};