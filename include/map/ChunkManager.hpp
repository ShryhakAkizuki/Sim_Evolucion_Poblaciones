#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <functional>

#include <fstream>
#include <filesystem>
#include <String>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif
#include <psapi.h>

#include "map/ChunkCord.hpp"
#include "map/Chunk.hpp"
#include "data_structures/Unordered_map.hpp"
#include "map/Tile.hpp"
#include "map/ChunkFileFormat.hpp"
#include "map/WorldGenerator.hpp"

#include <iostream>

class ChunkManager{
private:
    // ----- Atributos -----
    Unordered_map<ChunkCoord, std::shared_ptr<Chunk>> _chunks;
    uint32_t _chunk_size = 16;
    uint32_t _simulation_distance = 8;
    uint32_t _keep_loaded_distance = 12;

    uint32_t _Initial_Global_size = 1000;
    
    std::shared_ptr<WorldGenerator> _worldGenerator;  

public:
    // ----- Constructores -----
    ChunkManager() = default;

    explicit ChunkManager(uint32_t chunk_size, 
                          uint32_t simulation_distance = 8, 
                          uint32_t keep_loaded_distance = 12, 
                          uint32_t Initial_Global_size = 1000, 
                          uint64_t worldSeed = 12345) : 
    _chunk_size(chunk_size), _simulation_distance(simulation_distance), _keep_loaded_distance(keep_loaded_distance), _Initial_Global_size(Initial_Global_size) {
        _worldGenerator = std::make_shared<WorldGenerator>(worldSeed);
        Initializate_All_Chunks();
    }

    explicit ChunkManager(uint32_t chunk_size, 
                          std::shared_ptr<WorldGenerator> worldGenerator,
                          uint32_t simulation_distance = 8, 
                          uint32_t keep_loaded_distance = 12, 
                          uint32_t Initial_Global_size = 1000) : 
    _chunk_size(chunk_size), _simulation_distance(simulation_distance), _keep_loaded_distance(keep_loaded_distance), _Initial_Global_size(Initial_Global_size), _worldGenerator(std::move(worldGenerator)) {
        
        if (!_worldGenerator) {
            _worldGenerator = std::make_shared<WorldGenerator>(12345);
        }
        Initializate_All_Chunks();
    }


    ChunkManager(const ChunkManager& other) :          
    _chunk_size(other._chunk_size), _simulation_distance(other._simulation_distance), _keep_loaded_distance(other._keep_loaded_distance), _Initial_Global_size(other._Initial_Global_size),
    _chunks(other._chunks), _worldGenerator(other._worldGenerator) {}

    ChunkManager(ChunkManager&& other) noexcept :              
    _chunk_size(other._chunk_size), _simulation_distance(other._simulation_distance), _keep_loaded_distance(other._keep_loaded_distance), _Initial_Global_size(other._Initial_Global_size),
    _chunks(std::move(other._chunks)), _worldGenerator(std::move(other._worldGenerator)) {
 
        other._chunk_size = 16;
        other._simulation_distance = 8;
        other._keep_loaded_distance = 12;
        other._Initial_Global_size = 1000;
        other._chunks = Unordered_map<ChunkCoord, std::shared_ptr<Chunk>>(); // Estado vacío pero válido
        other._worldGenerator = nullptr;  
    }

    // ----- Destructor -----
    ~ChunkManager() = default;

    // ----- Operadores -----
    ChunkManager& operator=(const ChunkManager& other) {        // Operador de asignación por copia
        if(this != &other) {
            _chunk_size = other._chunk_size;
            _simulation_distance = other._simulation_distance;
            _keep_loaded_distance = other._keep_loaded_distance;
            _Initial_Global_size = other._Initial_Global_size;
            _chunks = other._chunks;
            _worldGenerator = other._worldGenerator;  
        }
        return *this;
    }

    ChunkManager& operator=(ChunkManager&& other) noexcept {    // Operador de asignación por movimiento
        if(this != &other) {
            _chunk_size = other._chunk_size;
            _simulation_distance = other._simulation_distance;
            _keep_loaded_distance = other._keep_loaded_distance;
            _Initial_Global_size = other._Initial_Global_size;
            _chunks = std::move(other._chunks);
            _worldGenerator = std::move(other._worldGenerator);

            other._chunk_size = 16;
            other._simulation_distance = 8;
            other._keep_loaded_distance = 12;
            other._Initial_Global_size = 1000;
            other._chunks = Unordered_map<ChunkCoord, std::shared_ptr<Chunk>>(); // Estado vacío pero válido
            other._worldGenerator = nullptr;  // NUEVO: Resetear generador
        }
        return *this;
    }

    // ----- Métodos -----

    // Conversiones
    ChunkCoord WorldToChunkPos(int worldX, int worldY) const {
        int chunk_size_int = static_cast<int>(_chunk_size);

        int ChunkX = (worldX >= 0) ? worldX / chunk_size_int : (worldX - chunk_size_int + 1) / chunk_size_int;
        int ChunkY = (worldY >= 0) ? worldY / chunk_size_int : (worldY - chunk_size_int + 1) / chunk_size_int;
        return {ChunkX, ChunkY};
    }

    // Acceso y retorno
    std::shared_ptr<Chunk> GetChunk(const ChunkCoord& coord) {
        std::shared_ptr<Chunk>* found_ptr = _chunks.find_ptr(coord);
        if (found_ptr != nullptr) {
            return *found_ptr;
        }

        return loadChunk(coord);
    }

    const std::shared_ptr<Chunk> GetChunk(const ChunkCoord& coord) const {
        const std::shared_ptr<Chunk>* found_ptr = _chunks.find_ptr(coord);
        if (found_ptr != nullptr) {
            return *found_ptr;
        }
        throw std::out_of_range("Chunk no encontrado: (" + 
                            std::to_string(coord.x()) + ", " + 
                            std::to_string(coord.y()) + ")");
    }

    Tile& GetTile(int worldX, int worldY) {
        ChunkCoord chunkPos = WorldToChunkPos(worldX, worldY);
        auto chunk = GetChunk(chunkPos);

        Pair<int,int> LocalPos = chunk->worldToLocal(worldX, worldY);

        return chunk->at(static_cast<uint32_t>(LocalPos.first()), 
                        static_cast<uint32_t>(LocalPos.second()));
    }

    const Tile& GetTile(int worldX, int worldY) const {
        ChunkCoord chunkPos = WorldToChunkPos(worldX, worldY);
        auto chunk = GetChunk(chunkPos);

        Pair<int,int> LocalPos = chunk->worldToLocal(worldX, worldY);

        return chunk->at(static_cast<uint32_t>(LocalPos.first()), 
                        static_cast<uint32_t>(LocalPos.second()));
    }

    Tile& GetTile(int worldX, int worldY, std::shared_ptr<Chunk> chunk) {
        Pair<int,int> LocalPos = chunk->worldToLocal(worldX, worldY);

        return chunk->at(static_cast<uint32_t>(LocalPos.first()), 
                        static_cast<uint32_t>(LocalPos.second()));
    }

    const Tile& GetTile(int worldX, int worldY, const std::shared_ptr<Chunk> chunk) const {
        Pair<int,int> LocalPos = chunk->worldToLocal(worldX, worldY);

        return chunk->at(static_cast<uint32_t>(LocalPos.first()), 
                        static_cast<uint32_t>(LocalPos.second()));
    }

    void SetTile(int worldX, int worldY, const Tile& Value){
        GetTile(worldX, worldY) = Value;
    }

    void SetTile(int worldX, int worldY, Tile&& Value){
        GetTile(worldX, worldY) = Value;
    }

    const uint32_t& getChunkSize() { return _chunk_size; }

    void setWorldGenerator(std::shared_ptr<WorldGenerator> generator) {
        _worldGenerator = std::move(generator);
    }
    
    std::shared_ptr<WorldGenerator> getWorldGenerator() const {
        return _worldGenerator;
    }
    
    void setWorldSeed(uint64_t seed) {
        if (_worldGenerator) {
            // Crear nuevo generador con la semilla
            _worldGenerator = std::make_shared<WorldGenerator>(seed);
        }
    }
    
    // Carga y descarga
    std::shared_ptr<Chunk> loadChunk(const ChunkCoord& coord){
        
        try {
            auto disk_chunk = LoadChunkFromDisk(coord.x(), coord.y());
            if (disk_chunk) {
                _chunks[coord] = disk_chunk;
                LinkChunkNeighbors(disk_chunk);
                return disk_chunk;
            }
        } catch (const std::exception& e) {
            std::cout << "DEBUG: Error cargando chunk desde disco: " << e.what() << "\n";
        }
        
        std::shared_ptr<Chunk> chunk;
        
        if (_worldGenerator) {
            chunk = _worldGenerator->generateChunk(coord.x(), coord.y(), _chunk_size);
        } else {
            // Fallback: chunk vacío
            chunk = std::make_shared<Chunk>(coord.x(), coord.y(), _chunk_size);
        }
        
        _chunks[coord] = chunk;
        LinkChunkNeighbors(chunk);
        
        return chunk;
    }

    void unloadChunk(const ChunkCoord& coord){
        auto chunk = GetChunk(coord);
        if (!chunk) return;

        SaveChunkToDisk(chunk);

        UnlinkChunkNeighbors(chunk);

        _chunks.erase(coord);
    }
    
    // Utilidades
    bool HasChunk(const ChunkCoord& coord) const {
        return _chunks.find_ptr(coord) != nullptr;
    }

    size_t GetLoadedChunkCount() const {
        return _chunks.size();
    }

    void actualizarEstadosBiomas(float horaDelDia) {
        if (_worldGenerator) {
            _worldGenerator->actualizarEstadosBiomas(horaDelDia);
        }
    }

    void generarRegionInicial(int centroX, int centroY, int radio) {
        std::cout << "DEBUG: Generando región inicial alrededor de (" 
                  << centroX << ", " << centroY << ") con radio " << radio << "\n";
        
        for (int x = centroX - radio; x <= centroX + radio; ++x) {
            for (int y = centroY - radio; y <= centroY + radio; ++y) {
                ChunkCoord coord(x, y);
                if (!HasChunk(coord)) {
                    loadChunk(coord);
                }
            }
        }
        
        std::cout << "DEBUG: Región inicial generada con " 
                  << GetLoadedChunkCount() << " chunks cargados\n";
    }

private:

    void initializeEmptyChunk(std::shared_ptr<Chunk> chunk) {
        // Rellenar con tiles básicos
        for (uint32_t y = 0; y < chunk->getChunkSize(); ++y) {
            for (uint32_t x = 0; x < chunk->getChunkSize(); ++x) {
                Tile& tile = chunk->at(x, y);
                tile.setBiomeId(3); // Pradera por defecto
            }
        }
    }

    void Initializate_All_Chunks(){
        // Calcular cuántos chunks necesitamos para cubrir el área global
        int chunks_needed = _Initial_Global_size / _chunk_size;
        int chunk_radius = chunks_needed / 2;
        
        int low_lim = -chunk_radius-1;
        int high_lim = chunk_radius+1;
        
        std::cout << "DEBUG: Creando chunks desde (" << low_lim << ", " << low_lim 
                << ") hasta (" << high_lim << ", " << high_lim << ")\n";
        
        uint32_t counter = 0;
        for(int i = low_lim; i <= high_lim; i++) {
            for(int j = low_lim; j <= high_lim; j++) {
                ChunkCoord ChunkPos(i, j);
                loadChunk(ChunkPos);
                counter++;
            }
        }
        std::cout << "Se crearon: " << counter << " Chunks\n";
    }

    std::shared_ptr<Chunk> LoadChunkFromDisk(int ChunkX, int ChunkY) {
        // Generar nombre de archivo
        std::string filename = GetChunkDirectory() + "/chunk_" + 
                            std::to_string(ChunkX) + "_" + 
                            std::to_string(ChunkY) + ".chnk";
        
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
        if (header.chunkX != ChunkX || header.chunkY != ChunkY) {
            std::cout << "ERROR: Coordenadas en archivo no coinciden: " 
                    << "esperado (" << ChunkX << ", " << ChunkY << "), "
                    << "encontrado (" << header.chunkX << ", " << header.chunkY << ")\n";
            return nullptr;
        }

        // Verificar que el tamaño del chunk coincide
        if (header.chunkSize != _chunk_size) {
            std::cout << "WARNING: Tamaño de chunk no coincide: " 
                    << header.chunkSize << " vs " << _chunk_size << "\n";
            // Puedes redimensionar o rechazar
        }

        // Crear chunk
        auto chunk = std::make_shared<Chunk>(ChunkX, ChunkY, header.chunkSize);
        
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
        std::cout << "DEBUG: Chunk cargado desde disco: " << filename << "\n";
        
        return chunk;

    }

    void SaveChunkToDisk(std::shared_ptr<Chunk> chunk){
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
        std::cout << "DEBUG: Chunk guardado: " << filename << "\n";

    }

    void LinkChunkNeighbors(std::shared_ptr<Chunk> chunk){
        if (!chunk) return;
            
        ChunkCoord coord(chunk->getChunkX(), chunk->getChunkY());
        
        // Norte
        ChunkCoord northCoord = coord.get_North();
        std::shared_ptr<Chunk>* northPtr = _chunks.find_ptr(northCoord);
        if (northPtr != nullptr) {
            auto north = *northPtr;
            chunk->Set_North(north);
            north->Set_South(chunk);
        }

        // Sur
        ChunkCoord southCoord = coord.get_South();
        std::shared_ptr<Chunk>* southPtr = _chunks.find_ptr(southCoord);
        if (southPtr != nullptr) {
            auto south = *southPtr;
            chunk->Set_South(south);
            south->Set_North(chunk);
        }

        // Este
        ChunkCoord eastCoord = coord.get_East();
        std::shared_ptr<Chunk>* eastPtr = _chunks.find_ptr(eastCoord);
        if (eastPtr != nullptr) {
            auto east = *eastPtr;
            chunk->Set_East(east);
            east->Set_West(chunk);
        }

        // Oeste
        ChunkCoord westCoord = coord.get_West();
        std::shared_ptr<Chunk>* westPtr = _chunks.find_ptr(westCoord);
        if (westPtr != nullptr) {
            auto west = *westPtr;
            chunk->Set_West(west);
            west->Set_East(chunk);
        }
    }

    void UnlinkChunkNeighbors(std::shared_ptr<Chunk> chunk){
        if (!chunk) return;

        ChunkCoord coord(chunk->getChunkX(), chunk->getChunkY());
        
        if (auto north = chunk->get_North()) {
            north->Set_South(nullptr);
        }
        
        if (auto south = chunk->get_South()) {
            south->Set_North(nullptr);
        }
        
        if (auto east = chunk->get_East()) {
            east->Set_West(nullptr);
        }
        
        if (auto west = chunk->get_West()) {
            west->Set_East(nullptr);
        }

        chunk->Set_North(nullptr);
        chunk->Set_South(nullptr);
        chunk->Set_East(nullptr);
        chunk->Set_West(nullptr);
    }

    std::string GetExecutableDirectory() const {
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

    std::string GetChunkDirectory() const {
        std::string exeDir = GetExecutableDirectory();
        return exeDir + "/world/chunks";
    }

};