#include <iostream>
#include <utility>

#include "biome/BiomeSystem.hpp"

// ----- Constructores -----

BiomeSystem::BiomeSystem(uint64_t worldSeed) 
: _worldSeed(worldSeed) {}

BiomeSystem::BiomeSystem(const BiomeSystem& other) 
: _worldSeed(other._worldSeed) {

    DynamicArray<std::unique_ptr<Bioma>> new_biomas(other._biomas.size());
    
    for (size_t i = 0; i < other._biomas.size(); ++i) {
        if (other._biomas[i]) {
            new_biomas[i] = std::make_unique<Bioma>(*(other._biomas[i]));
        }
    }
    
    _biomas = std::move(new_biomas);
}

BiomeSystem::BiomeSystem(BiomeSystem&& other) noexcept
    : _biomas(std::move(other._biomas))
    , _worldSeed(std::exchange(other._worldSeed, 0)) {}

// ----- Operadores -----

BiomeSystem& BiomeSystem::operator=(const BiomeSystem& other) {
    if (this != &other) {
        _worldSeed = other._worldSeed;
        
        DynamicArray<std::unique_ptr<Bioma>> new_biomas(other._biomas.size());
        
        for (size_t i = 0; i < other._biomas.size(); ++i) {
            if (other._biomas[i]) {
                new_biomas[i] = std::make_unique<Bioma>(*(other._biomas[i]));
            }
        }
        
        _biomas = std::move(new_biomas);
    }

    return *this;
}

BiomeSystem& BiomeSystem::operator=(BiomeSystem&& other) noexcept {
    if (this != &other) {
        _biomas = std::move(other._biomas);
        _worldSeed = std::exchange(other._worldSeed, 0);
    }
    return *this;
}

// ----- Métodos públicos -----

void BiomeSystem::registrarBioma(std::unique_ptr<Bioma>&& bioma) {
    if (!bioma) return;
    
    int id = bioma->getId();
    if (contieneBioma(id)) {
        return; // ID ya existe
    }
    
    // Asegurar capacidad
    if (id >= static_cast<int>(_biomas.size())) {
        _biomas.reserve(id + 1);
        while (_biomas.size() <= static_cast<size_t>(id)) {
            _biomas.push_back(nullptr);
        }
    }
    
    _biomas[id] = std::move(bioma);
}

const Bioma* BiomeSystem::getBioma(int id) const {
    if (id < 0 || static_cast<size_t>(id) >= _biomas.size()) {
        return nullptr;
    }
    
    return _biomas[id].get(); 
}

const Bioma* BiomeSystem::buscarPorNombre(const std::string& nombre) const {
    
    for (size_t i = 0; i < _biomas.size(); ++i) {
        if (_biomas[i] && _biomas[i]->getNombre() == nombre) {
            return _biomas[i].get();
        }
    }
    return nullptr;
}

void BiomeSystem::actualizarTodosBiomas(float horaDelDia) {
    for (size_t i = 0; i < _biomas.size(); ++i) {
        if (_biomas[i]) {
            _biomas[i]->actualizarEstado(horaDelDia);
        }
    }
}

DynamicArray<int> BiomeSystem::getTodosBiomasID() const {
    DynamicArray<int> IDs;
    for (const auto& bioma : _biomas) {
        if (bioma) IDs.push_back(bioma->getId());
    }
    return IDs;
}

void BiomeSystem::limpiar() {
    _biomas.clear();
}

bool BiomeSystem::contieneBioma(int id) const {
    return id >= 0 && 
           static_cast<size_t>(id) < _biomas.size() && 
           _biomas[id] != nullptr;
}