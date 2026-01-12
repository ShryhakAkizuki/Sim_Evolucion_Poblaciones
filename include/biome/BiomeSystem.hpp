#pragma once
#include <memory>
#include <cstdint>
#include <string>

#include "data_structures/DynamicArray.hpp"
#include "biome/Biome.hpp"

class BiomeSystem {
private:
    DynamicArray<std::unique_ptr<Bioma>> _biomas;  
    uint64_t _worldSeed = 0;

public:
    // ----- Constructores -----
    BiomeSystem() = default;
    explicit BiomeSystem(uint64_t worldSeed);
    BiomeSystem(const BiomeSystem& other);
    BiomeSystem(BiomeSystem&& other) noexcept;

    // ----- Destructor -----
    ~BiomeSystem() = default;

    // ----- Operadores -----
    BiomeSystem& operator=(const BiomeSystem& other);
    BiomeSystem& operator=(BiomeSystem&& other) noexcept;

    // ----- Métodos -----
    void registrarBioma(std::unique_ptr<Bioma>&& bioma);

    const Bioma* getBioma(int id) const;        
    const Bioma* buscarPorNombre(const std::string& nombre) const;

    void actualizarTodosBiomas(float horaDelDia);
    
    size_t getCantidadBiomas() const { return _biomas.size(); }
    bool estaVacio() const { return _biomas.empty(); }
    uint32_t getWorldSeed() const { return _worldSeed; }

    const DynamicArray<std::unique_ptr<Bioma>>& getTodosBiomas() const { return _biomas; }    
    DynamicArray<int> getTodosBiomasID() const;

    void limpiar();
    bool contieneBioma(int id) const;
};