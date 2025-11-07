#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "map/Tile.hpp"
#include "map/Chunk.hpp"

class TileDrawer {
private:
    std::unordered_map<int, sf::Color> _biomeColors;
    sf::Color _waterColor;
    sf::Color _defaultColor;
    
    void initializeBiomeColors();

public:
    TileDrawer();
    
    sf::Color getTileColor(const Tile& tile) const;
    sf::Color getBiomeColor(int biomeId) const;
    
    // Métodos para crear formas visuales
    sf::RectangleShape createTileShape(const Tile& tile, float size) const;
    sf::VertexArray createChunkVertices(const std::shared_ptr<Chunk>& chunk, 
                                       float tileSize) const;
    
    // Setters para personalización
    void setBiomeColor(int biomeId, const sf::Color& color);
    void setWaterColor(const sf::Color& color);
};