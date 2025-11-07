// BiomeSystem.cpp
#include "biome/BiomeSystem.hpp"
#include <algorithm>
#include <utility>
#include <iostream>

// ----- Constructores -----

// Constructor principal
BiomeSystem::BiomeSystem(uint32_t worldSeed) 
: _worldSeed(worldSeed) {}

// Constructor de copia
BiomeSystem::BiomeSystem(const BiomeSystem& other) 
: _worldSeed(other._worldSeed) {
    copiarDesde(other);
}

// Constructor de movimiento
BiomeSystem::BiomeSystem(BiomeSystem&& other) noexcept
    : _biomas(std::move(other._biomas))
    , _worldSeed(std::exchange(other._worldSeed, 0)) {}

// ----- Operadores -----

// Operador de asignación por copia
BiomeSystem& BiomeSystem::operator=(const BiomeSystem& other) {
    if (this != &other) {
        _worldSeed = other._worldSeed;
        limpiar();
        copiarDesde(other);
    }
    return *this;
}

// Operador de asignación por movimiento
BiomeSystem& BiomeSystem::operator=(BiomeSystem&& other) noexcept {
    if (this != &other) {
        _biomas = std::move(other._biomas);
        _worldSeed = std::exchange(other._worldSeed, 0);
    }
    return *this;
}

// ----- Métodos públicos -----

void BiomeSystem::registrarBioma(std::shared_ptr<Bioma> bioma) {
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

std::shared_ptr<Bioma> BiomeSystem::getBioma(int id) {
    if (id < 0 || static_cast<size_t>(id) >= _biomas.size()) {
        return nullptr;
    }
    
    return _biomas[id];
}

std::shared_ptr<const Bioma> BiomeSystem::getBioma(int id) const {
    if (id < 0 || static_cast<size_t>(id) >= _biomas.size()) {
        return nullptr;
    }
    
    return _biomas[id]; 
}

std::shared_ptr<Bioma> BiomeSystem::buscarPorNombre(const std::string& nombre) {
    
    for (size_t i = 0; i < _biomas.size(); ++i) {
        if (_biomas[i] && _biomas[i]->getNombre() == nombre) {
            return _biomas[i];
        }
    }
    return nullptr;
}

std::shared_ptr<const Bioma> BiomeSystem::buscarPorNombre(const std::string& nombre) const {
    
    for (size_t i = 0; i < _biomas.size(); ++i) {
        if (_biomas[i] && _biomas[i]->getNombre() == nombre) {
            return _biomas[i];
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

void BiomeSystem::limpiar() {
    _biomas.clear();
}

bool BiomeSystem::contieneBioma(int id) const {
    return id >= 0 && 
           static_cast<size_t>(id) < _biomas.size() && 
           _biomas[id] != nullptr;
}

// ----- Métodos privados -----

void BiomeSystem::copiarDesde(const BiomeSystem& other) {

    _biomas = DynamicArray<std::shared_ptr<Bioma>>(other._biomas.size());
    for (size_t i = 0; i < other._biomas.size(); ++i) {
        _biomas[i] = other._biomas[i]; 
    }
}