#include "visualization/TileDrawer.hpp"
#include <iostream>

TileDrawer::TileDrawer(std::shared_ptr<BiomeSystem> biomeSystem)
    : _biomeSystem(biomeSystem) {
    
    _waterColor = sf::Color(0, 120, 255, 200);
    _defaultColor = sf::Color(100, 100, 100);  // Gris por defecto
    
    std::cout << "TileDrawer inicializado " 
              << (_biomeSystem ? "con" : "sin") 
              << " sistema de biomas\n";
}

sf::Color TileDrawer::getTileColor(const Tile& tile) const {
    sf::Color baseColor = getBiomeColor(tile.getBiomeId());
    
    // Si el tile tiene agua, mezclar con color azul
    if (tile.hasWater()) {
        baseColor.r = static_cast<sf::Uint8>((baseColor.r + _waterColor.r) / 2);
        baseColor.g = static_cast<sf::Uint8>((baseColor.g + _waterColor.g) / 2);
        baseColor.b = static_cast<sf::Uint8>((baseColor.b + _waterColor.b) / 2);
        baseColor.a = static_cast<sf::Uint8>((baseColor.a + _waterColor.a) / 2);
    }
    
    return baseColor;
}

sf::Color TileDrawer::getBiomeColor(int biomeId) const {
    if (!_biomeSystem) {
        std::cout << "WARNING: TileDrawer no tiene BiomeSystem, usando color por defecto\n";
        return _defaultColor;
    }
    
    auto biome = _biomeSystem->getBioma(biomeId);
    if (biome) {
        // Convertir nuestra estructura Color a sf::Color
        Color biomeColor = biome->getColor();
        return sf::Color(biomeColor.r, biomeColor.g, biomeColor.b);
    }
    
    std::cout << "WARNING: Bioma ID " << biomeId << " no encontrado, usando color por defecto\n";
    return _defaultColor;
}

// Los demás métodos se mantienen iguales...
sf::RectangleShape TileDrawer::createTileShape(const Tile& tile, float size) const {
    sf::RectangleShape shape(sf::Vector2f(size, size));
    shape.setFillColor(getTileColor(tile));
    shape.setOutlineThickness(0.05f);
    shape.setOutlineColor(sf::Color(0, 0, 0, 100));
    return shape;
}

sf::VertexArray TileDrawer::createChunkVertices(const std::shared_ptr<Chunk>& chunk, 
                                               float tileSize) const {
    sf::VertexArray vertices(sf::Quads, chunk->getChunkSize() * chunk->getChunkSize() * 4);
    
    uint32_t chunkSize = chunk->getChunkSize();
    
    for (uint32_t localY = 0; localY < chunkSize; ++localY) {
        for (uint32_t localX = 0; localX < chunkSize; ++localX) {
            const Tile& tile = chunk->at(localX, localY);
            sf::Color color = getTileColor(tile);
            
            // Convertir a coordenadas mundiales
            auto worldPos = chunk->localToWorld(localX, localY);
            float worldX = static_cast<float>(worldPos.first());
            float worldY = static_cast<float>(worldPos.second());
            
            // Calcular índice base para este tile
            int index = (localY * chunkSize + localX) * 4;
            
            // Definir los 4 vértices del quad
            vertices[index].position = sf::Vector2f(worldX, worldY);
            vertices[index+1].position = sf::Vector2f(worldX + tileSize, worldY);
            vertices[index+2].position = sf::Vector2f(worldX + tileSize, worldY + tileSize);
            vertices[index+3].position = sf::Vector2f(worldX, worldY + tileSize);
            
            // Asignar color a cada vértice
            for (int i = 0; i < 4; ++i) {
                vertices[index + i].color = color;
            }
        }
    }
    
    return vertices;
}

void TileDrawer::setBiomeSystem(std::shared_ptr<BiomeSystem> biomeSystem) {
    _biomeSystem = biomeSystem;
    std::cout << "TileDrawer: Sistema de biomas actualizado\n";
}

void TileDrawer::setWaterColor(const sf::Color& color) {
    _waterColor = color;
}
