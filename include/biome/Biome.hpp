#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>

#include <string>
#include <random>

struct EstadoBioma {
    float temperatura = 0.0f;
    float humedad = 0.0f;
    float radiacion_solar = 0.0f;
    float costo_movimiento = 1.0f;
};

struct ParametrosBioma {
    float expansion = 0.0f;
    float reduccion = 0.0f;
};

struct Color {
    uint8_t r = 0, g = 0, b = 0;
    
    Color() = default;
    Color(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}
};


class Bioma {
private:
    int _id = -1;
    std::string _nombre = "Desconocido";
    Color _color = {0,0,0};

    // Propiedades base para generación
    float _temperaturaMedia = 20.0f;
    float _humedadMedia = 50.0f;
    float _radiacionSolarMedia = 50.0f;
    float _costoMovimientoMedia = 1.0f;
    float _tasaExpansionMedia = 0.0f;
    float _tasaReduccionMedia = 0.0f;

    float _temperaturaStd = 5.0f;
    float _humedadStd = 10.0f;
    float _radiacionSolarStd = 15.0f;
    float _costoMovimientoStd = 0.2f;
    float _tasaExpansionStd = 0.05f;
    float _tasaReduccionStd = 0.05f;

    // Estado dinámico actual
    EstadoBioma _estadoActual = {};
    ParametrosBioma _parametrosActual = {};

    // RNG determinista basado en semilla mundial
    std::mt19937 _gen;
    std::uniform_real_distribution<float> _dist;

public:
    // ----- Constructores -----
    Bioma() = default;

    Bioma(int id, std::string nombre, Color color = {0, 0, 0},
          float tempMedia = 20.0f, float humMedia = 50.0f, float radMedia = 50.0f, 
          float movMedia = 1.0f, float expanMedia = 0.0f, float reducMedia = 0.0f,
          float tempStd = 5.0f, float humStd = 10.0f, float radStd = 15.0f,
          float movStd = 0.2f, float expanStd = 0.05f, float reducStd = 0.05f,
          uint64_t seed = 0);

    Bioma(int id, std::string nombre) : Bioma(id, std::move(nombre), {0, 0, 0}) {}
    Bioma(const Bioma& other);
    Bioma(Bioma&& other) noexcept;

    // ----- Destructor -----
    ~Bioma() = default;

    // ----- Operadores -----
    Bioma& operator=(const Bioma& other);
    Bioma& operator=(Bioma&& other) noexcept;

    // ----- Métodos -----
    // Actualizacion dinamica
    void actualizarEstado(float horaDelDia);

    // Acceso y retorno
    const EstadoBioma& getEstadoActual() const { return _estadoActual; }
    const ParametrosBioma& getParametrosActual() const { return _parametrosActual; }
    int getId() const { return _id; }
    const std::string& getNombre() const { return _nombre; }
    Color getColor() const { return _color; }

private:
    // ----- Métodos -----
    void inicializarRNG(uint64_t seed);
};