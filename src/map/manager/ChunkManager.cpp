#include <memory>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <iostream>

#include <fstream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#include "data_structures/Pair.hpp"

#include "map/manager/ChunkManager.hpp"
#include "map/manager/ChunkFileFormat.hpp"

// ----- Constructores -----

ChunkManager::ChunkManager(uint32_t chunk_size,
                        uint64_t worldSeed) : 
_chunk_size(chunk_size),
_seed(worldSeed) {}

ChunkManager::ChunkManager(ChunkManager&& other) noexcept :              
_chunks(std::move(other._chunks)), 
_chunk_size(other._chunk_size),
_seed(other._seed) {
    other._chunk_size = 16;
    other._seed = 12345;
}

// ----- Operadores -----
ChunkManager& ChunkManager::operator=(ChunkManager&& other) noexcept {    // Operador de asignación por movimiento
    if(this != &other) {
        _chunks.clear();  
        
        _chunk_size = other._chunk_size;
        _chunks = std::move(other._chunks);  // Mover
        _seed = other._seed;

        other._chunk_size = 16;
        other._seed = 12345;
    }
    return *this;
}

// ----- Métodos -----

// Conversiones
ChunkCoord ChunkManager::WorldToChunkPos(int worldX, int worldY) const {
    int chunk_size_int = static_cast<int>(_chunk_size);

    int ChunkX = (worldX >= 0) ? worldX / chunk_size_int : (worldX - chunk_size_int + 1) / chunk_size_int;
    int ChunkY = (worldY >= 0) ? worldY / chunk_size_int : (worldY - chunk_size_int + 1) / chunk_size_int;
    return {ChunkX, ChunkY};
}

// Acceso y retorno - coord
void ChunkManager::SetChunk(ChunkCoord coord, std::unique_ptr<Chunk>&& chunk){   
    Chunk* raw = chunk.get();
    _chunks.emplace(coord,std::move(chunk));
    LinkChunkNeighbors(raw);
}

Chunk* ChunkManager::GetChunk(ChunkCoord coord) {
    std::unique_ptr<Chunk>* found_chunk = _chunks.find_ptr(coord);
    if (found_chunk != nullptr) return found_chunk->get();
        
    std::unique_ptr<Chunk> disk_chunk = LoadChunkFromDisk(coord);
    if (disk_chunk != nullptr) {
        Chunk* raw = disk_chunk.get();
        SetChunk(coord, std::move(disk_chunk));
        return raw;
    }

    return nullptr;
}

const Chunk* ChunkManager::GetChunk(ChunkCoord coord) const{
    const std::unique_ptr<Chunk>* found_chunk = _chunks.find_ptr(coord);
    if (found_chunk != nullptr) return found_chunk->get();
    else return nullptr;
}

// Acceso y retorno - world x,y
void ChunkManager::SetChunk(int worldX, int worldY, std::unique_ptr<Chunk>&& chunk){  
    ChunkCoord chunkPos = WorldToChunkPos(worldX, worldY);
    SetChunk(chunkPos, std::move(chunk));
}

Chunk* ChunkManager::GetChunk(int worldX, int worldY) {
    ChunkCoord chunkPos = WorldToChunkPos(worldX, worldY);
    return GetChunk(chunkPos);
}

const Chunk* ChunkManager::GetChunk(int worldX, int worldY) const{
    ChunkCoord chunkPos = WorldToChunkPos(worldX, worldY);
    return GetChunk(chunkPos);
}

Tile& ChunkManager::GetTile(int worldX, int worldY, Chunk* chunk) {
    Pair<int,int> LocalPos = chunk->worldToLocal(worldX, worldY);

    return chunk->at(static_cast<uint32_t>(LocalPos.first()), 
                    static_cast<uint32_t>(LocalPos.second()));
}

Tile& ChunkManager::GetTile(int worldX, int worldY) {
    ChunkCoord chunkPos = WorldToChunkPos(worldX, worldY);
    auto chunk = GetChunk(chunkPos);

    return GetTile(worldX, worldY, chunk);
}

const Tile& ChunkManager::GetTile(int worldX, int worldY, const Chunk* chunk) const {
    Pair<int,int> LocalPos = chunk->worldToLocal(worldX, worldY);

    return chunk->at(static_cast<uint32_t>(LocalPos.first()), 
                    static_cast<uint32_t>(LocalPos.second()));
}

const Tile& ChunkManager::GetTile(int worldX, int worldY) const {
    ChunkCoord chunkPos = WorldToChunkPos(worldX, worldY);
    auto chunk = GetChunk(chunkPos);

    return GetTile(worldX, worldY, chunk);
}

// Eliminacion/descarga
void ChunkManager::eraseChunk(const ChunkCoord& coord){
    Chunk* chunk = GetChunk(coord);
    if (chunk != nullptr) {
        SaveChunkToDisk(chunk);
        UnlinkChunkNeighbors(coord, chunk);
    }

    // Usa el método erase() correcto de Unordered_map
    _chunks.erase(coord);
}
  
// Utilidades
bool ChunkManager::HasChunk(int worldX, int worldY) const {
    ChunkCoord chunkPos = WorldToChunkPos(worldX, worldY);
    return _chunks.find_ptr(chunkPos) != nullptr;
}

// ---------- Metodos privados ----------

// Disk - Chunks
std::unique_ptr<Chunk> ChunkManager::LoadChunkFromDisk(const ChunkCoord& coord) {
    // Generar nombre de archivo
    std::string filename = GetChunkDirectory() + "/chunk_" + 
                        std::to_string(coord.x()) + "_" + 
                        std::to_string(coord.y()) + ".chnk";
    
    // Verificar si el archivo existe
    if (!std::filesystem::exists(filename)) {
        return nullptr;
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "ERROR: No se pudo abrir archivo para cargar: " << filename << "\n";
        return nullptr;
    }
    
    // Leer header
    ChunkFileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(ChunkFileHeader));
    
    // Verificar magic number
    if (header.magic[0] != 'C' || header.magic[1] != 'H' || 
        header.magic[2] != 'N' || header.magic[3] != 'K') {
        std::cout << "ERROR: Archivo corrupto o formato inválido: " << filename << "\n";
        return nullptr;
    }
    
    // Verificar versión
    if (header.version != 1) {
        std::cout << "ERROR: Versión de formato no soportada: " << header.version << "\n";
        return nullptr;
    }

    // Verificar coordenadas
    if (header.chunkX != coord.x() || header.chunkY != coord.y()) {
        std::cout << "ERROR: Coordenadas en archivo no coinciden: " 
                << "esperado (" << coord.x() << ", " << coord.y() << "), "
                << "encontrado (" << header.chunkX << ", " << header.chunkY << ")\n";
        return nullptr;
    }

    // Verificar que el tamaño del chunk coincide
    if (header.chunkSize != _chunk_size) {
        std::cout << "WARNING: Tamaño de chunk no coincide: " 
                << header.chunkSize << " vs " << _chunk_size << "\n";
        return nullptr;
    }

    // Verificar que la Seed del mundo sea correcta
    if (header.seed != _seed) {
        std::cout << "WARNING: La seed del mundo no coincide: " 
                << header.seed << " vs " << _seed << "\n";
        return nullptr;
    }

    // Crear chunk
    auto chunk = std::make_unique<Chunk>(coord, header.chunkSize);
    
    // Leer datos de tiles
    for (uint32_t y = 0; y < header.chunkSize; ++y) {
        for (uint32_t x = 0; x < header.chunkSize; ++x) {
            Tile tile;
            file.read(reinterpret_cast<char*>(&tile), sizeof(Tile));
            
            if (!file.good()) {
                std::cout << "ERROR: Fallo al leer tile en (" << x << ", " << y << ")\n";
                return nullptr;
            }
            
            chunk->at(x, y) = tile;
        }
    }
    chunk->setState(State::LOADED);

    file.close();
    
    return chunk;

}

void ChunkManager::SaveChunkToDisk(Chunk* chunk){
    if (!chunk) {
        std::cout << "ERROR: Intento de guardar chunk nulo\n";
        return;
    }
    
    std::string chunkDir = GetChunkDirectory();
    std::filesystem::create_directories(chunkDir);

    // Generar nombre de archivo
    std::string filename = chunkDir + "/chunk_" + 
        std::to_string(chunk->getChunkX()) + "_" + 
        std::to_string(chunk->getChunkY()) + ".chnk";

    std::ofstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        std::cout << "ERROR: No se pudo abrir archivo para guardar: " << filename << "\n";
        return;
    } 

    ChunkFileHeader header;
    header.chunkX = chunk->getChunkX();
    header.chunkY = chunk->getChunkY();
    header.chunkSize = chunk->getChunkSize();
    header.state = chunk->getState();
    header.seed = _seed;

    // Calcular tamaño de datos (asumiendo que cada Tile tiene tamaño fijo)
    header.tileDataSize = header.chunkSize * header.chunkSize * sizeof(Tile);
    
    // Escribir header
    file.write(reinterpret_cast<const char*>(&header), sizeof(ChunkFileHeader));
    
    // Escribir datos de tiles
    for (uint32_t y = 0; y < chunk->getChunkSize(); ++y) {
        for (uint32_t x = 0; x < chunk->getChunkSize(); ++x) {
            const Tile& tile = chunk->at(x, y);
            file.write(reinterpret_cast<const char*>(&tile), sizeof(Tile));
        }
    }
    
    if (!file.good()) {
        std::cout << "ERROR: Fallo al escribir chunk: " << filename << "\n";
        return;
    }
    
    file.close();
}

// Dynamic Link - Chunks

void ChunkManager::LinkChunkNeighbors(Chunk* chunk){
    if (!chunk) return;
        
    ChunkCoord coord(chunk->getChunkX(), chunk->getChunkY());
    
    // Norte
    ChunkCoord northCoord = coord.get_North();
    std::unique_ptr<Chunk>* northPtr = _chunks.find_ptr(northCoord);
    if (northPtr != nullptr) {
        auto north = northPtr->get();
        chunk->Set_North(north);
        north->Set_South(chunk);
    }

    // Sur
    ChunkCoord southCoord = coord.get_South();
    std::unique_ptr<Chunk>* southPtr = _chunks.find_ptr(southCoord);
    if (southPtr != nullptr) {
        auto south = southPtr->get();
        chunk->Set_South(south);
        south->Set_North(chunk);
    }

    // Este
    ChunkCoord eastCoord = coord.get_East();
    std::unique_ptr<Chunk>* eastPtr = _chunks.find_ptr(eastCoord);
    if (eastPtr != nullptr) {
        auto east = eastPtr->get();
        chunk->Set_East(east);
        east->Set_West(chunk);
    }

    // Oeste
    ChunkCoord westCoord = coord.get_West();
    std::unique_ptr<Chunk>* westPtr = _chunks.find_ptr(westCoord);
    if (westPtr != nullptr) {
        auto west = westPtr->get();
        chunk->Set_West(west);
        west->Set_East(chunk);
    }
}

void ChunkManager::UnlinkChunkNeighbors(const ChunkCoord& coord, Chunk* chunk){
 if (!chunk) return;
    
    // Usar las coordenadas para verificar existencia, no los punteros
    if (chunk->has_North()) {
        ChunkCoord northCoord = coord.get_North();
        if (_chunks.find_ptr(northCoord)) {
            Chunk* north = chunk->get_North();
            if (north) north->Set_South(nullptr);
        }
    }
    
    if (chunk->has_South()) {
        ChunkCoord southCoord = coord.get_South();
        if (_chunks.find_ptr(southCoord)) {
            Chunk* south = chunk->get_South();
            if (south) south->Set_North(nullptr);
        }
    }
    
    if (chunk->has_East()) {
        ChunkCoord eastCoord = coord.get_East();
        if (_chunks.find_ptr(eastCoord)) {
            Chunk* east = chunk->get_East();
            if (east) east->Set_West(nullptr);
        }
    }
    
    if (chunk->has_West()) {
        ChunkCoord westCoord = coord.get_West();
        if (_chunks.find_ptr(westCoord)) {
            Chunk* west = chunk->get_West();
            if (west) west->Set_East(nullptr);
        }
    }
    
    // Limpiar punteros del chunk actual
    chunk->Set_North(nullptr);
    chunk->Set_South(nullptr);
    chunk->Set_East(nullptr);
    chunk->Set_West(nullptr);
}

// PATH - Chunks
std::string ChunkManager::GetExecutableDirectory() const {
    #ifdef _WIN32
        // Windows
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        std::string exePath(path);
        return exePath.substr(0, exePath.find_last_of("\\/"));
    #else
        // Linux/macOS
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        if (count != -1) {
            std::string exePath(path, count);
            return exePath.substr(0, exePath.find_last_of("/"));
        }
        return "."; // Fallback
    #endif
}

std::string ChunkManager::GetChunkDirectory() const {
    std::string exeDir = GetExecutableDirectory();
    return exeDir + "/world/chunks";
}

// Iteradores - Metodos

ChunkManager::iterator ChunkManager::begin() {
    return iterator(_chunks.begin());
}

ChunkManager::iterator ChunkManager::end() {
    return iterator(_chunks.end());
}

// Iteradores constantes (desde objeto const)
ChunkManager::const_iterator ChunkManager::begin() const {
    return const_iterator(_chunks.begin());
}

ChunkManager::const_iterator ChunkManager::end() const {
    return const_iterator(_chunks.end());
}

// Iteradores constantes explícitos
ChunkManager::const_iterator ChunkManager::cbegin() const {
    return const_iterator(_chunks.cbegin());
}

ChunkManager::const_iterator ChunkManager::cend() const {
    return const_iterator(_chunks.cend());
}

