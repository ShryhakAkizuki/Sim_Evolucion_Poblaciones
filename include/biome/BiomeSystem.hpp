#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <string>

#include "data_structures/DynamicArray.hpp"
#include "biome/Biome.hpp"

class BiomeSystem {
private:
    DynamicArray<std::shared_ptr<Bioma>> _biomas;  
    uint32_t _worldSeed = 0;

public:
    // ----- Constructores -----
    BiomeSystem() = default;
    explicit BiomeSystem(uint32_t worldSeed);
    BiomeSystem(const BiomeSystem& other);
    BiomeSystem(BiomeSystem&& other) noexcept;

    // ----- Destructor -----
    ~BiomeSystem() = default;

    // ----- Operadores -----
    BiomeSystem& operator=(const BiomeSystem& other);
    BiomeSystem& operator=(BiomeSystem&& other) noexcept;

    // ----- Métodos -----
    void registrarBioma(std::shared_ptr<Bioma> bioma);

    std::shared_ptr<Bioma> getBioma(int id);
    std::shared_ptr<const Bioma> getBioma(int id) const;

    std::shared_ptr<Bioma> buscarPorNombre(const std::string& nombre);
    std::shared_ptr<const Bioma> buscarPorNombre(const std::string& nombre) const;

    void actualizarTodosBiomas(float horaDelDia);
    
    size_t getCantidadBiomas() const { return _biomas.size(); }
    bool estaVacio() const { return _biomas.empty(); }
    uint32_t getWorldSeed() const { return _worldSeed; }

    const DynamicArray<std::shared_ptr<Bioma>>& getTodosBiomas() const { return _biomas; }

    void limpiar();
    bool contieneBioma(int id) const;

private:
    // ----- Métodos -----
    void copiarDesde(const BiomeSystem& other);
};