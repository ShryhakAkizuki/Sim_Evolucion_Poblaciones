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
    
    std::unordered_map<ChunkCoord, sf::VertexArray> _chunkCache;
    
public:
    MapRenderer(ChunkManager& chunkManager);
    
    void render(sf::RenderTarget& target, const CameraController& camera);
    void clearCache();
    void updateCacheForChunk(const ChunkCoord& coord);
    
    // Nuevo método para actualizar el sistema de biomas
    void setBiomeSystem(std::shared_ptr<BiomeSystem> biomeSystem) {
        _tileDrawer.setBiomeSystem(biomeSystem);
        clearCache(); // Limpiar cache porque los colores pueden haber cambiado
    }
    
    TileDrawer& getTileDrawer() { return _tileDrawer; }
    const TileDrawer& getTileDrawer() const { return _tileDrawer; }

private:
    void renderChunk(sf::RenderTarget& target, const std::shared_ptr<Chunk>& chunk);
    void renderTile(sf::RenderTarget& target, const Tile& tile, 
                   float worldX, float worldY, float tileSize);
};