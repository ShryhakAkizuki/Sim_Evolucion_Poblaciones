#include <algorithm>
#include <utility>

#include "biome/Biome.hpp"

// ----- Constructores -----

// Constructor principal
Bioma::Bioma(int id, std::string nombre,
             float tempMedia, float humMedia, float radMedia, 
             float movMedia, float expanMedia, float reducMedia,
             float tempStd, float humStd, float radStd,
             float movStd, float expanStd, float reducStd,
             uint64_t seed)
    : _id(id)
    , _nombre(std::move(nombre))
    , _temperaturaMedia(tempMedia)
    , _humedadMedia(humMedia)
    , _radiacionSolarMedia(radMedia)
    , _costoMovimientoMedia(movMedia)
    , _tasaExpansionMedia(expanMedia)
    , _tasaReduccionMedia(reducMedia)
    , _temperaturaStd(tempStd)
    , _humedadStd(humStd)
    , _radiacionSolarStd(radStd)
    , _costoMovimientoStd(movStd)
    , _tasaExpansionStd(expanStd)
    , _tasaReduccionStd(reducStd)
    , _dist(-1.0f, 1.0f) {
    
    inicializarRNG(seed);
    actualizarEstado(12.0f);    // Estado inicial al medio día
}

// Constructor de copia
Bioma::Bioma(const Bioma& other)
    : _id(other._id)
    , _nombre(other._nombre)
    , _temperaturaMedia(other._temperaturaMedia)
    , _humedadMedia(other._humedadMedia)
    , _radiacionSolarMedia(other._radiacionSolarMedia)
    , _costoMovimientoMedia(other._costoMovimientoMedia)
    , _tasaExpansionMedia(other._tasaExpansionMedia)
    , _tasaReduccionMedia(other._tasaReduccionMedia)
    , _temperaturaStd(other._temperaturaStd)
    , _humedadStd(other._humedadStd)
    , _radiacionSolarStd(other._radiacionSolarStd)
    , _costoMovimientoStd(other._costoMovimientoStd)
    , _tasaExpansionStd(other._tasaExpansionStd)
    , _tasaReduccionStd(other._tasaReduccionStd)
    , _estadoActual(other._estadoActual)
    , _parametrosActual(other._parametrosActual)
    , _dist(-1.0f, 1.0f) {
    
    // Recrear el RNG con el mismo estado sería complejo, 
    // así que lo inicializamos con valores por defecto
    inicializarRNG(0);
}

// Constructor de movimiento
Bioma::Bioma(Bioma&& other) noexcept
    : _id(std::exchange(other._id, -1))
    , _nombre(std::move(other._nombre))
    , _temperaturaMedia(std::exchange(other._temperaturaMedia, 20.0f))
    , _humedadMedia(std::exchange(other._humedadMedia, 50.0f))
    , _radiacionSolarMedia(std::exchange(other._radiacionSolarMedia, 50.0f))
    , _costoMovimientoMedia(std::exchange(other._costoMovimientoMedia, 1.0f))
    , _tasaExpansionMedia(std::exchange(other._tasaExpansionMedia, 0.0f))
    , _tasaReduccionMedia(std::exchange(other._tasaReduccionMedia, 0.0f))
    , _temperaturaStd(std::exchange(other._temperaturaStd, 5.0f))
    , _humedadStd(std::exchange(other._humedadStd, 10.0f))
    , _radiacionSolarStd(std::exchange(other._radiacionSolarStd, 15.0f))
    , _costoMovimientoStd(std::exchange(other._costoMovimientoStd, 0.2f))
    , _tasaExpansionStd(std::exchange(other._tasaExpansionStd, 0.05f))
    , _tasaReduccionStd(std::exchange(other._tasaReduccionStd, 0.05f))
    , _estadoActual(std::exchange(other._estadoActual, {}))
    , _parametrosActual(std::exchange(other._parametrosActual, {}))
    , _gen(std::move(other._gen))
    , _dist(std::move(other._dist)) {
}
    
// ----- Operadores -----

// Operador de asignación por copia
Bioma& Bioma::operator=(const Bioma& other) {
    if (this != &other) {
        _id = other._id;
        _nombre = other._nombre;
        _temperaturaMedia = other._temperaturaMedia;
        _humedadMedia = other._humedadMedia;
        _radiacionSolarMedia = other._radiacionSolarMedia;
        _costoMovimientoMedia = other._costoMovimientoMedia;
        _tasaExpansionMedia = other._tasaExpansionMedia;
        _tasaReduccionMedia = other._tasaReduccionMedia;
        _temperaturaStd = other._temperaturaStd;
        _humedadStd = other._humedadStd;
        _radiacionSolarStd = other._radiacionSolarStd;
        _costoMovimientoStd = other._costoMovimientoStd;
        _tasaExpansionStd = other._tasaExpansionStd;
        _tasaReduccionStd = other._tasaReduccionStd;
        _estadoActual = other._estadoActual;
        _parametrosActual = other._parametrosActual;
        
        // Recrear RNG
        inicializarRNG(0);
    }
    return *this;
}

// Operador de asignación por movimiento
Bioma& Bioma::operator=(Bioma&& other) noexcept {
    if (this != &other) {
        _id = std::exchange(other._id, -1);
        _nombre = std::move(other._nombre);
        _temperaturaMedia = std::exchange(other._temperaturaMedia, 20.0f);
        _humedadMedia = std::exchange(other._humedadMedia, 50.0f);
        _radiacionSolarMedia = std::exchange(other._radiacionSolarMedia, 50.0f);
        _costoMovimientoMedia = std::exchange(other._costoMovimientoMedia, 1.0f);
        _tasaExpansionMedia = std::exchange(other._tasaExpansionMedia, 0.0f);
        _tasaReduccionMedia = std::exchange(other._tasaReduccionMedia, 0.0f);
        _temperaturaStd = std::exchange(other._temperaturaStd, 5.0f);
        _humedadStd = std::exchange(other._humedadStd, 10.0f);
        _radiacionSolarStd = std::exchange(other._radiacionSolarStd, 15.0f);
        _costoMovimientoStd = std::exchange(other._costoMovimientoStd, 0.2f);
        _tasaExpansionStd = std::exchange(other._tasaExpansionStd, 0.05f);
        _tasaReduccionStd = std::exchange(other._tasaReduccionStd, 0.05f);
        _estadoActual = std::exchange(other._estadoActual, {});
        _parametrosActual = std::exchange(other._parametrosActual, {});
        _gen = std::move(other._gen);
        _dist = std::move(other._dist);
    }
    return *this;
}

// ----- Métodos privados -----

void Bioma::inicializarRNG(uint64_t seed) {
    _gen = std::mt19937(seed + _id); // Semilla única por bioma
}

// ----- Métodos públicos -----
// Actualizacion dinamica
void Bioma::actualizarEstado(float horaDelDia) {
    constexpr float pi = 3.14159265358979323846f;
    
    float factor1 = std::sin(((horaDelDia - 6.0f) / 24.0f) * 2.0f * pi);
    float factor2 = std::sin((2.0f * (horaDelDia + 3.0f) / 24.0f) * 2.0f * pi);

    // Aplicar variación diaria + ruido controlado
    float ruido = _dist(_gen) * 0.05f;
    
    _estadoActual.temperatura = _temperaturaMedia + (factor1 + ruido) * _temperaturaStd;
    _estadoActual.humedad = _humedadMedia + (factor1 + ruido) * _humedadStd;
    _estadoActual.radiacion_solar = _radiacionSolarMedia + (factor1 + ruido) * _radiacionSolarStd;
    _estadoActual.costo_movimiento = _costoMovimientoMedia + (factor2 + ruido) * _costoMovimientoStd;
        
    _parametrosActual.expansion = _tasaExpansionMedia + (factor1 + ruido) * _tasaExpansionStd;
    _parametrosActual.reduccion = _tasaReduccionMedia - (factor1 - ruido) * _tasaReduccionStd;
    
    // Asegurar valores dentro de rangos razonables
    _estadoActual.temperatura = std::clamp(_estadoActual.temperatura, -50.0f, 60.0f);
    _estadoActual.humedad = std::clamp(_estadoActual.humedad, 0.0f, 100.0f);
    _estadoActual.radiacion_solar = std::max(0.0f, _estadoActual.radiacion_solar);
}
