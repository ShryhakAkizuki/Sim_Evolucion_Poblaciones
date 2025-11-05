#pragma once
#include <iostream>
#include <stdexcept>
#include <functional>
#include <filesystem>
#include <fstream>

// ----------------- Mapa - Chunks jerarquicos ------------
template<typename T>
struct CellData {
    int x, y;
    T data;
};

template<typename T>
struct Cell {
    int x, y; 
    T data;

    Cell* up = nullptr;
    Cell* down = nullptr;
    Cell* left = nullptr;
    Cell* right = nullptr;
};

template<typename T>
struct Chunk {
    int chunkX, chunkY;     
    int originX, originY; 
    int sizeX, sizeY;    
    DinamycArray<DinamycArray<Cell<T>>> cells;

    Chunk* up = nullptr;
    Chunk* down = nullptr;
    Chunk* left = nullptr;
    Chunk* right = nullptr;

    // For hierarchy (optional)
    Chunk* parent = nullptr;
    DinamycArray<Chunk<T>*> children;
};

template<typename T, unsigned int N_BUCKETS = 4096>
class Dynamic_Map {
private:
    int baseChunkSize;
    int chunksPerLevel;
    int maxLevels;
    std::filesystem::path basePath;

    DinamycArray<SimpleHashMap<Chunk<T>*>> levels;

public:

    Dynamic_Map(int baseChunkSize, int chunksPerLevel, int maxLevels, const std::string& baseDir)
    : baseChunkSize(baseChunkSize), chunksPerLevel(chunksPerLevel), maxLevels(maxLevels), basePath(baseDir){

        if (!std::filesystem::exists(basePath)) {
            std::filesystem::create_directories(basePath);
        }

        for (int i = 0; i < maxLevels; ++i) {
            levels.push_back(SimpleHashMap<Chunk<T>*, N_BUCKETS>());
        }
    }

    ~Dynamic_Map() {
        for (int level = 0; level < maxLevels; ++level) {
            DinamycArray<Chunk<T>*> chunks = levels[level].getAllValues();
            for (int i = 0; i < chunks.size(); ++i) {
                delete chunks[i];
            }
        }
    }

    Chunk<T>* loadChunk(int Chunk_X, int Chunk_Y, int level = 0){
        Pares<int> key{Chunk_X, Chunk_Y};
        Chunk<T>** existing = levels[level].find(key);
        if (existing && *existing) return *existing;

        Chunk<T>* chunk = nullptr;

        chunk = loadChunkFromDisk(Chunk_X, Chunk_Y, level);
        if (!chunk) {
            chunk = generateNewChunk(Chunk_X, Chunk_Y, level);
        }

        levels[level].insert(key, chunk);

        linkCellsNeighbors(chunk);
        linkChunksNeighbors(chunk, level);

        Chunk<T>* currentChild = chunk;
        for(int lvl = level + 1; lvl < maxLevels; ++lvl){
            int parentX = currentChild->chunkX / chunksPerLevel;
            int parentY = currentChild->chunkY / chunksPerLevel;
            Pares<int> parentKey{parentX, parentY};

            Chunk<T>** existingParent = levels[lvl].find(parentKey);
            Chunk<T>* parent = nullptr;

            if(existingParent && *existingParent){
                parent = *existingParent;
            } else {
                parent = generateNewChunk(parentX, parentY, lvl);
                levels[lvl].insert(parentKey, parent);
            }

            bool alreadyLinked = false;
            for(int i = 0; i < parent->children.size(); ++i){
                if(parent->children[i] == currentChild){
                    alreadyLinked = true;
                    break;
                }
            }
            if(!alreadyLinked)
                parent->children.push_back(currentChild);

            currentChild->parent = parent;
            linkChunksNeighbors(parent, lvl);
            currentChild = parent;

        }
    return chunk;
    }

    void unloadChunk(int Chunk_X, int Chunk_Y, int level = 0){
        Pares<int> key{Chunk_X, Chunk_Y};
        Chunk<T>** existing = levels[level].find(key);
        if (!existing || !*existing) return;

        Chunk<T>* chunk = *existing;

        unlinkChunksNeighbors(chunk, level);
        unlinkCellsNeighbors(chunk);

        saveChunkToDisk(chunk, level);

        levels[level].erase(key);

        Chunk<T>* parent = chunk->parent;
        if(parent){
            for(int i = 0; i < parent->children.size(); ++i){
                if(parent->children[i] == chunk){
                    // eliminar child
                    for(int j = i; j + 1 < parent->children.size(); ++j)
                        parent->children[j] = parent->children[j + 1];
                    parent->children.pop_back();
                    break;
                }
            }
        }

        delete chunk;

        Chunk<T>* currentParent = parent;
        int currentLevel = level + 1;
        
        while(currentParent && currentParent->children.size() == 0 && currentLevel < maxLevels){
            Pares<int> parentKey{currentParent->chunkX, currentParent->chunkY};
            Chunk<T>* grandParent = currentParent->parent;

            unlinkChunksNeighbors(currentParent, currentLevel);

            levels[currentLevel].erase(parentKey);
            delete currentParent;

            if(grandParent){
                for(int i = 0; i < grandParent->children.size(); ++i){
                    if(grandParent->children[i] == currentParent){
                        for(int j = i; j + 1 < grandParent->children.size(); ++j)
                            grandParent->children[j] = grandParent->children[j + 1];
                        grandParent->children.pop_back();
                        break;
                    }
                }
            }

            currentParent = grandParent;
            currentLevel++;
        }
    }

    Chunk<T>* getChunk(int globalX, int globalY, int level = 0) {
        if (level < 0 || level >= maxLevels) return nullptr;

        int scale = 1;
        for (int i = 0; i < level; ++i) scale *= chunksPerLevel;

        int chunkX = globalX / (baseChunkSize * scale);
        int chunkY = globalY / (baseChunkSize * scale);

        Pares<int> key{chunkX, chunkY};
        Chunk<T>** existing = levels[level].find(key);

        if (existing && *existing)
            return *existing;

        // Si no está cargado, lo cargamos o lo generamos
        return loadChunk(chunkX, chunkY, level);        
    }

    Cell<T>& getCell(int globalX, int globalY) {
        Chunk<T>* chunk = getChunk(globalX, globalY, 0);
        int localX = globalX - chunk->originX;
        int localY = globalY - chunk->originY;

        return chunk->cells[localY][localX];
    }

    void setCell(int globalX, int globalY, const T& value) {
        getCell(globalX, globalY).data = value;
    }

    DinamycArray<Chunk<T>*> getChunksAtLevel(int level) {
        DinamycArray<Chunk<T>*> chunks;
        if (level < 0 || level >= maxLevels) return chunks;

        chunks = levels[level].getAllValues();
        return chunks;
    }

    friend std::ostream& operator<<(std::ostream& os, const Dynamic_Map<T, N_BUCKETS>& map) {
        os << "=== MAPA DINAMICO ===\n";

        for (int lvl = 0; lvl < map.maxLevels; ++lvl) {
            os << "Nivel " << lvl << ":\n";
            auto chunks = map.levels[lvl].getAllValues();

            if (chunks.size() == 0) {
                os << "  (sin chunks cargados)\n";
                continue;
            }

            for (int i = 0; i < chunks.size(); ++i) {
                Chunk<T>* c = chunks[i];
                if (!c) continue;

                os << "  Chunk (" << c->chunkX << ", " << c->chunkY << ")";
                if (lvl == 0 && c->cells.size() > 0) {
                    os << " - Cells:\n";
                    for (int y = 0; y < c->sizeY; ++y) {
                        for (int x = 0; x < c->sizeX; ++x) {
                            const Cell<T>& cell = c->cells[y][x];
                            os << "Cell(" << cell.x << ", " << cell.y << "): " << cell.data << " ";
                        }
                        os << "\n";
                    }
                } else {
                    os << " - (sin celdas)\n";

                    if (c->children.size() > 0) {
                        os << " - Hijos: ";
                        for (int j = 0; j < c->children.size(); ++j) {
                            Chunk<T>* child = c->children[j];
                            if (child)
                                os << "(" << child->chunkX << ", " << child->chunkY << ") ";
                        }
                    }
                }
                os << "\n";
                
                // Imprimir vecinos siempre, nivel 0 incluido
                os << "   Vecinos: ";
                os << "[Up: "    << (c->up    ? "(" + std::to_string(c->up->chunkX) + ", " + std::to_string(c->up->chunkY) + ")" : "null") << "] ";
                os << "[Down: "  << (c->down  ? "(" + std::to_string(c->down->chunkX) + ", " + std::to_string(c->down->chunkY) + ")" : "null") << "] ";
                os << "[Left: "  << (c->left  ? "(" + std::to_string(c->left->chunkX) + ", " + std::to_string(c->left->chunkY) + ")" : "null") << "] ";
                os << "[Right: " << (c->right ? "(" + std::to_string(c->right->chunkX) + ", " + std::to_string(c->right->chunkY) + ")" : "null") << "]\n";
            }
        }

        os << "======================\n";
        return os;
    }
private:

    Chunk<T>* generateNewChunk(int Chunk_X, int Chunk_Y, int level){
        Chunk<T>* chunk = new Chunk<T>;
        chunk->chunkX = Chunk_X;
        chunk->chunkY = Chunk_Y;

        // Escala para niveles superiores
        int scale = 1;
        for (int i = 0; i < level; ++i) scale *= chunksPerLevel;

        chunk->sizeX = baseChunkSize * scale;
        chunk->sizeY = baseChunkSize * scale;
        chunk->originX = Chunk_X * chunk->sizeX;
        chunk->originY = Chunk_Y * chunk->sizeY;

        // Solo para nivel 0 (celdas individuales)
        if (level == 0) {
            chunk->cells = DinamycArray<DinamycArray<Cell<T>>>(); // Inicial vacío

            for (int i = 0; i < chunk->sizeY; ++i) {
                DinamycArray<Cell<T>> row; // Inicial vacío
                for (int j = 0; j < chunk->sizeX; ++j) {
                    Cell<T> cell;
                    cell.x = chunk->originX + j;
                    cell.y = chunk->originY + i;
                    cell.data = T{}; // Inicializar dato
                    row.push_back(cell);
                }
                chunk->cells.push_back(row); // Agregar fila completa
            }
        }

        return chunk;
    }

    Chunk<T>* loadChunkFromDisk(int Chunk_X, int Chunk_Y, int level){
        
        if(level != 0) return nullptr;

        std::filesystem::path chunkFile = basePath / ("chunk_" + std::to_string(Chunk_X) + "_" + std::to_string(Chunk_Y) + ".bin");

        if(!std::filesystem::exists(chunkFile)){
            std::cout << "Chunk file does not exist: " << chunkFile << std::endl;
            return nullptr;
        }

        std::ifstream file(chunkFile, std::ios::binary);
        if(!file.is_open()){
            std::cerr << "Failed to open chunk file: " << chunkFile << std::endl;
            return nullptr;
        }

        Chunk<T>* chunk = new Chunk<T>;
        chunk->chunkX = Chunk_X;
        chunk->chunkY = Chunk_Y;
        chunk->sizeX = baseChunkSize;
        chunk->sizeY = baseChunkSize;
        chunk->originX = Chunk_X * chunk->sizeX;
        chunk->originY = Chunk_Y * chunk->sizeY;
        chunk->cells = DinamycArray<DinamycArray<Cell<T>>>(chunk->sizeY);
        
        for(int i = 0; i < chunk->sizeY; ++i){
            DinamycArray<Cell<T>> row(chunk->sizeX);
            for(int j = 0; j < chunk->sizeX; ++j){

                Cell<T> cell;
                CellData<T> temp;

                if (!file.read(reinterpret_cast<char*>(&temp), sizeof(CellData<T>))) {
                    std::cerr << "Error leyendo chunk file: " << chunkFile << std::endl;
                    delete chunk;
                    return nullptr;
                }             
                cell.x = temp.x;
                cell.y = temp.y;
                cell.data = temp.data;   
                row.push_back(cell);
            }
            chunk->cells.push_back(row);
        }
        file.close();
        
        // Implement disk loading logic here
        return chunk;
    }

    void saveChunkToDisk(Chunk<T>* chunk, int level){

        if(level != 0) return;
        std::filesystem::path chunkFile = basePath / ("chunk_" + std::to_string(chunk->chunkX) + "_" + std::to_string(chunk->chunkY) + ".bin");

        std::ofstream file(chunkFile, std::ios::binary);
        if(!file.is_open()){
            std::cerr << "Failed to open chunk file for writing: " << chunkFile << std::endl;
            return;
        }

        for(int i = 0; i < chunk->sizeY; ++i){
            for(int j = 0; j < chunk->sizeX; ++j){
                Cell<T>& cell = chunk->cells[i][j];
                CellData temp{cell.x, cell.y, cell.data};

                file.write(reinterpret_cast<const char*>(&temp), sizeof(CellData<T>));
            }
        }       

        file.close();
    }

    void linkChunksNeighbors(Chunk<T>* chunk, int level){
        if(!chunk) return;

        Pares<int> upKey{chunk->chunkX, chunk->chunkY > 0 ? chunk->chunkY - 1 : 0};
        Pares<int> downKey{chunk->chunkX, chunk->chunkY + 1};
        Pares<int> leftKey{chunk->chunkX > 0 ? chunk->chunkX - 1 : 0, chunk->chunkY};
        Pares<int> rightKey{chunk->chunkX + 1, chunk->chunkY};

        Chunk<T>** up = levels[level].find(upKey);
        Chunk<T>** down = levels[level].find(downKey);
        Chunk<T>** left = levels[level].find(leftKey);
        Chunk<T>** right = levels[level].find(rightKey);

        chunk->up    = (up && *up) ? *up : nullptr;
        chunk->down  = (down && *down) ? *down : nullptr;
        chunk->left  = (left && *left) ? *left : nullptr;
        chunk->right = (right && *right) ? *right : nullptr;

        if(chunk->up && chunk->up->down != chunk)       chunk->up->down = chunk;
        if(chunk->down && chunk->down->up != chunk)     chunk->down->up = chunk;
        if(chunk->left && chunk->left->right != chunk)  chunk->left->right = chunk;
        if(chunk->right && chunk->right->left != chunk) chunk->right->left = chunk;
    }
    
    void linkCellsNeighbors(Chunk<T>* chunk){
        for(int i = 0; i < chunk->sizeY; ++i){
            for(int j = 0; j < chunk->sizeX; ++j){
                Cell<T>& cell = chunk->cells[i][j];
                
                if(i > 0) cell.up = &chunk->cells[i - 1][j];
                if(i < chunk->sizeY - 1) cell.down = &chunk->cells[i + 1][j];
                if(j > 0) cell.left = &chunk->cells[i][j - 1];
                if(j < chunk->sizeX - 1) cell.right = &chunk->cells[i][j + 1];
            }
        }
    }

    void unlinkChunksNeighbors(Chunk<T>* chunk, int level){
        if(chunk->up) chunk->up->down = nullptr;
        if(chunk->down) chunk->down->up = nullptr;
        if(chunk->left) chunk->left->right = nullptr;
        if(chunk->right) chunk->right->left = nullptr;
            
        chunk->up = nullptr;
        chunk->down = nullptr;
        chunk->left = nullptr;
        chunk->right = nullptr;
    }

    void unlinkCellsNeighbors(Chunk<T>* chunk){
        for(int i = 0; i < chunk->sizeY; ++i){
            for(int j = 0; j < chunk->sizeX; ++j){
                Cell<T>& cell = chunk->cells[i][j];

                if(i > 0) cell.up = nullptr;
                if(i < chunk->sizeY - 1) cell.down = nullptr;
                if(j > 0) cell.left = nullptr;
                if(j < chunk->sizeX - 1) cell.right = nullptr;
            }
        }
    }

};














