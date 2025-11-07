#include "visualization/TileDrawer.hpp"
#include <iostream>

TileDrawer::TileDrawer() {
    initializeBiomeColors();
    _waterColor = sf::Color(0, 100, 200, 180); // Azul semitransparente para agua
    _defaultColor = sf::Color(100, 100, 100);  // Gris por defecto
    
    std::cout << "TileDrawer inicializado con " << _biomeColors.size() << " colores de bioma\n";
}

void TileDrawer::initializeBiomeColors() {
    // Colores basados en los biomas por defecto de WorldGenerator
    _biomeColors[0] = sf::Color(0, 0, 150);     // Océano - Azul oscuro
    _biomeColors[1] = sf::Color(240, 230, 140); // Playa - Amarillo arena
    _biomeColors[2] = sf::Color(34, 139, 34);   // Bosque - Verde bosque
    _biomeColors[3] = sf::Color(124, 252, 0);   // Pradera - Verde pradera
    _biomeColors[4] = sf::Color(255, 255, 0);   // Desierto - Amarillo
    _biomeColors[-1] = sf::Color(150, 150, 150); // No asignado - Gris
}

sf::Color TileDrawer::getTileColor(const Tile& tile) const {
    sf::Color baseColor = getBiomeColor(tile.getBiomeId());
    
    // Si el tile tiene agua, mezclar con color azul
    if (tile.hasWater()) {
        // Mezcla lineal entre el color base y el color de agua
        baseColor.r = static_cast<sf::Uint8>((baseColor.r + _waterColor.r) / 2);
        baseColor.g = static_cast<sf::Uint8>((baseColor.g + _waterColor.g) / 2);
        baseColor.b = static_cast<sf::Uint8>((baseColor.b + _waterColor.b) / 2);
        baseColor.a = static_cast<sf::Uint8>((baseColor.a + _waterColor.a) / 2);
    }
    
    return baseColor;
}

sf::Color TileDrawer::getBiomeColor(int biomeId) const {
    auto it = _biomeColors.find(biomeId);
    if (it != _biomeColors.end()) {
        return it->second;
    }
    return _defaultColor;
}

sf::RectangleShape TileDrawer::createTileShape(const Tile& tile, float size) const {
    sf::RectangleShape shape(sf::Vector2f(size, size));
    shape.setFillColor(getTileColor(tile));
    shape.setOutlineThickness(0.05f);
    shape.setOutlineColor(sf::Color(0, 0, 0, 100)); // Borde negro semitransparente
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
            
            // Calcular índice base para este tile (4 vértices por tile)
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

void TileDrawer::setBiomeColor(int biomeId, const sf::Color& color) {
    _biomeColors[biomeId] = color;
}

void TileDrawer::setWaterColor(const sf::Color& color) {
    _waterColor = color;
}