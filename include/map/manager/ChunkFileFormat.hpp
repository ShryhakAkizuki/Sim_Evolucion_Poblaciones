#pragma once
#include <cstdint>
#include "map/manager/Chunk.hpp"

#pragma pack(push, 1)  // Ensure no padding
struct ChunkFileHeader {
    char magic[4] = {'C', 'H', 'N', 'K'};   // Identificador mágico
    uint32_t version = 1;                   // Versión del formato
    int32_t chunkX;                         // Coordenada X del chunk
    int32_t chunkY;                         // Coordenada Y del chunk  
    uint32_t chunkSize;                     // Tamaño del chunk (16)
    State  state;                           // Estado anterior del chunk
    uint32_t tileDataSize;                  // Tamaño de datos de tiles
    uint64_t seed;                          // Seed del mundo con el que fue generado el chunk
};
#pragma pack(pop)
