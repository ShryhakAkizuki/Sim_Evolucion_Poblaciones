#include <stdexcept>
#include "map/WorldSystem.hpp"

WorldSystem::WorldSystem(const BiomeSystem& biomeSystem, 
                        uint64_t worldSeed, 
                        uint32_t chunkSize, 
                        int simulation_distance, 
                        int keep_loaded_distance)
    : _Manager(chunkSize,worldSeed),
      _Generator(biomeSystem, worldSeed),
      _simulation_distance(simulation_distance),
      _keep_loaded_distance(keep_loaded_distance) {};


WorldSystem::WorldSystem(const BiomeSystem& biomeSystem, 
                        uint64_t worldSeed, 
                        uint32_t chunkSize, 
                        LakeConfig lake_config, 
                        int simulation_distance, 
                        int keep_loaded_distance)
    : _Manager(chunkSize, worldSeed),
      _Generator(biomeSystem, lake_config, worldSeed),
      _simulation_distance(simulation_distance),
      _keep_loaded_distance(keep_loaded_distance) {};

WorldSystem::WorldSystem(WorldSystem&& other) noexcept 
  :   _Manager(std::move(other._Manager)),
      _Generator(std::move(other._Generator)),
      _simulation_distance(std::move(other._simulation_distance)),
      _keep_loaded_distance(std::move(other._keep_loaded_distance)) {};

// ----- Operadores -----
WorldSystem& WorldSystem::operator=(WorldSystem&& other) noexcept {
  if (this != &other) {
    _Generator = std::move(other._Generator);
    _Manager = std::move(other._Manager);
    _simulation_distance = std::move(other._simulation_distance);
    _keep_loaded_distance = std::move(other._keep_loaded_distance);
  }
  return *this;
}

// ----- Métodos Base -----
Tile& WorldSystem::GetTile(int WorldX, int WorldY){
  ChunkCoord coord = _Manager.WorldToChunkPos(WorldX,WorldY);
  Chunk* Access_Chunk = _Manager.GetChunk(coord);
  
  if (Access_Chunk == nullptr){
    std::unique_ptr<Chunk> New_Chunk = _Generator.generateChunk(coord, _Manager.getChunkSize());    
    _Manager.SetChunk(coord, std::move(New_Chunk));

    Access_Chunk = _Manager.GetChunk(coord);
    Access_Chunk->setState(State::LOADED);
  }

  return _Manager.GetTile(WorldX, WorldY, Access_Chunk);
}

const Tile& WorldSystem::GetTile(int WorldX, int WorldY) const{
  ChunkCoord coord = _Manager.WorldToChunkPos(WorldX,WorldY);
  const Chunk* Access_Chunk = _Manager.GetChunk(coord);
  
  return _Manager.GetTile(WorldX, WorldY, Access_Chunk);
}

const Chunk* WorldSystem::GetChunk(ChunkCoord coord) const{
  return _Manager.GetChunk(coord);
}


void WorldSystem::SetTile(int WorldX, int WorldY, const Tile& Value){
  ChunkCoord coord = _Manager.WorldToChunkPos(WorldX,WorldY);
  Chunk* Access_Chunk = _Manager.GetChunk(coord);
  
  if (Access_Chunk == nullptr){
    std::unique_ptr<Chunk> New_Chunk = _Generator.generateChunk(coord, _Manager.getChunkSize());    
    _Manager.SetChunk(coord, std::move(New_Chunk));

    Access_Chunk = _Manager.GetChunk(coord);
    Access_Chunk->setState(State::LOADED);
  }

  _Manager.GetTile(WorldX, WorldY, Access_Chunk) = Value;
}

void WorldSystem::SetTile(int WorldX, int WorldY, Tile&& Value){
  ChunkCoord coord = _Manager.WorldToChunkPos(WorldX,WorldY);
  Chunk* Access_Chunk = _Manager.GetChunk(coord);
  
  if (Access_Chunk == nullptr){
    std::unique_ptr<Chunk> New_Chunk = _Generator.generateChunk(coord, _Manager.getChunkSize());    
    _Manager.SetChunk(coord, std::move(New_Chunk));

    Access_Chunk = _Manager.GetChunk(coord);
    Access_Chunk->setState(State::LOADED);
  }

  _Manager.GetTile(WorldX, WorldY, Access_Chunk) = std::move(Value);
}

void WorldSystem::UnloadChunk(const ChunkCoord& coord){
  _Manager.eraseChunk(coord);
}

void WorldSystem::UnloadChunk(int WorldX, int WorldY){
  ChunkCoord coord = _Manager.WorldToChunkPos(WorldX, WorldY);
  UnloadChunk(coord);
}

void WorldSystem::LoadChunk(ChunkCoord coord){
  Chunk* Access_Chunk = _Manager.GetChunk(coord);
  
  if (Access_Chunk == nullptr){
    std::unique_ptr<Chunk> New_Chunk = _Generator.generateChunk(coord, _Manager.getChunkSize());    
    New_Chunk->setState(State::LOADED);
    _Manager.SetChunk(coord, std::move(New_Chunk));
  }
}

void WorldSystem::LoadChunk(int WorldX, int WorldY){
  ChunkCoord coord = _Manager.WorldToChunkPos(WorldX,WorldY);
  LoadChunk(coord);
}

const uint32_t& WorldSystem::GetChunkSize() const{
  return _Manager.getChunkSize();
}

// ------ Carga y descarga masiva ------
void WorldSystem::loadAllChunksInSquare(int WorldX_1, int WorldY_1, int WorldX_2, int WorldY_2){
  if(WorldX_2 < WorldX_1 || WorldY_2 < WorldY_1){
    throw std::out_of_range("Cuadrado invalido, WorldX_2 > WorldX_1 y WorldY_2 > WorldY_1");
  }

  for(int i = WorldX_1; i<=WorldX_2; ++i){
    for(int j = WorldY_1; j<=WorldY_2; ++j){
      LoadChunk(i,j);
    }
  }
}

void WorldSystem::UnloadAllChunksInSquare(int WorldX_1, int WorldY_1, int WorldX_2, int WorldY_2){
  if(WorldX_2 < WorldX_1 || WorldY_2 < WorldY_1){
    throw std::out_of_range("Cuadrado invalido, WorldX_2 > WorldX_1 y WorldY_2 > WorldY_1");
  }

  for(int i = WorldX_1; i<=WorldX_2; ++i){
    for(int j = WorldY_1; j<=WorldY_2; ++j){
      UnloadChunk(i,j);
    }
  }
}

void WorldSystem::loadAllChunksInCircle(int WorldX, int WorldY, int Radius){
  if(Radius<1){
    throw std::out_of_range("Circulo invalido, Radio > 1");
  }

  for(int i = -Radius; i<=Radius; ++i){
    for(int j = -Radius; j<=Radius; ++j){
      if (i*i + j*j <= Radius*Radius){
        LoadChunk(WorldX+i,WorldY+j);
      }
    }
  }
}

void WorldSystem::UnloadAllChunksInCircle(int WorldX, int WorldY, int Radius){
  if(Radius<1){
    throw std::out_of_range("Circulo invalido, Radio > 1");
  }

  for(int i = -Radius; i<=Radius; ++i){
    for(int j = -Radius; j<=Radius; ++j){
      if (i*i + j*j <= Radius*Radius){
        UnloadChunk(WorldX+i,WorldY+j);
      }
    }
  }
}

void WorldSystem::loadAllChunksInVector(DynamicArray<ChunkCoord> Chunk_Array){
  for (ChunkCoord& Chunk: Chunk_Array){
    LoadChunk(Chunk);
  }
}

void WorldSystem::UnloadAllChunksInVector(DynamicArray<ChunkCoord> Chunk_Array){
  for (ChunkCoord& Chunk: Chunk_Array){
    UnloadChunk(Chunk);
  }
}

// ------ Gestion de Chunks y estados ------
void WorldSystem::DynamicChunkStates(){
  _Distant_Chunks.clear();

  for(auto Chunk_it = _Manager.begin(); Chunk_it!=_Manager.end(); ++Chunk_it){
    
    float minDistance = _keep_loaded_distance*2;

    for(auto &Center: _Activity_Centers){
      float Distance = Center.euclideanDistance(Chunk_it.coord());
      if(Distance < minDistance) minDistance = Distance;
    }

    if(minDistance > _simulation_distance){
      (*Chunk_it)->distant();
      _Distant_Chunks.push_back(*Chunk_it);
    }else{
      (*Chunk_it)->activate();
    }
  }
}

void WorldSystem::UnloadFarChunks(){
  for(auto Chunk_it = _Distant_Chunks.begin(); Chunk_it!=_Distant_Chunks.end(); ++Chunk_it){
    
    float minDistance = _keep_loaded_distance*2;

    for(auto &Center: _Activity_Centers){
      float Distance = Center.euclideanDistance((*Chunk_it)->getChunkCoord());
      if(Distance < minDistance) minDistance = Distance;
    }

    if(minDistance > _keep_loaded_distance){
      Chunk* Chunk_Pop = *Chunk_it;
      _Distant_Chunks.erase(Chunk_it);
      Chunk_Pop->isLoaded();
      UnloadChunk(Chunk_Pop->getChunkCoord());
    }
  }
}

void WorldSystem::LoadActivityChunks(){
  for(auto &Center: _Activity_Centers){
    for(int i = -_simulation_distance; i<=_simulation_distance; ++i){
      for(int j = -_simulation_distance; j<=_simulation_distance; ++j){
        ChunkCoord Coord= Center.getNeighbor(i,j);
        LoadChunk(Coord);
      }
    }
  }
}

// ------ Gestion de centros de actividad ------
void WorldSystem::Set_Center(ChunkCoord coord) {
  for(const auto &center: _Activity_Centers){
    if(center == coord) return;
  }
  _Activity_Centers.push_back(coord);

  return;
};

void WorldSystem::Set_Erase_Center(ChunkCoord coord) {
  for(auto i = _Activity_Centers.begin(); i!=_Activity_Centers.end(); ++i){
    if (*i == coord){
      _Activity_Centers.erase(i);
        return;
    }
  }

  return;
};
