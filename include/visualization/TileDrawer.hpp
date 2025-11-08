#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "map/Tile.hpp"
#include "map/Chunk.hpp"
#include "biome/BiomeSystem.hpp"

class TileDrawer {
private:
    std::shared_ptr<BiomeSystem> _biomeSystem;
    std::unordered_map<int, sf::Color> _biomeColors;
    sf::Color _waterColor;
    sf::Color _defaultColor;
    
    void initializeBiomeColors();

public:
    
    TileDrawer(std::shared_ptr<BiomeSystem> biomeSystem = nullptr);
    
    sf::Color getTileColor(const Tile& tile) const;
    sf::Color getBiomeColor(int biomeId) const;
    
    // Métodos para crear formas visuales
    sf::RectangleShape createTileShape(const Tile& tile, float size) const;
    sf::VertexArray createChunkVertices(const std::shared_ptr<Chunk>& chunk, 
                                       float tileSize) const;
    
    // Setters para personalización
    void setBiomeSystem(std::shared_ptr<BiomeSystem> biomeSystem);
    void setWaterColor(const sf::Color& color);
  
};