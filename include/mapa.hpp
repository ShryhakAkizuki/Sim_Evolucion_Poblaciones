#ifndef MAPA_HPP
#define MAPA_HPP

#include <random>
#include <cstring>
#include <iostream>

struct Cord{
    int x;
    int y;

    bool operator==(const Cord& other) const {
        return x == other.x && y == other.y;
    }
};

struct EstadosBasico {
    float temperatura;
    float humedad;
    float radiacion_solar;
    float costo_movimiento;
};

struct ParametrosBasico {
    float expansion;
    float reduccion;
};

class Bioma {
private:
    int id;
    std::string nombre;

    // Inicializacion
    float temperatura_media;
    float humedad_media;
    float radiacion_solar_media;
    float costo_movimiento_media;
    float tasa_expansion_recursos_media;
    float tasa_reduccion_recursos_media;

    float temperatura_std;
    float humedad_std;
    float radiacion_solar_std;
    float costo_movimiento_std;
    float tasa_expansion_recursos_std;
    float tasa_reduccion_recursos_std;

    // Estado dinámico
    EstadosBasico Estado_Bioma_Actual;
    ParametrosBasico Parametros_Bioma_Actual;

    // Generador de ruido aleatorio
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist;

public:
    
    // Constructor de Bioma
    Bioma(int identificador, std::string nombre,
        float temp_media, float hum_media   , float rad_media, 
        float mov_media , float expan_media , float reduc_media,
        float temp_std  , float hum_std     , float rad_std, 
        float mov_std   , float expan_std   , float reduc_std);

    void actualizarEstado(float hora_del_dia);

    EstadosBasico getEstadoBioma() const;
    ParametrosBasico getParametrosBioma() const;
    int getBiomaId() const;
};

struct Semilla {
    Cord Coordenada;
    const Bioma* bioma;
};

struct CeldaBase {
    const Bioma* bioma;  
    bool tiene_agua;
};

class Mapa {
private:
    // tamaño del mapa en km    
    float ancho_km;       
    float alto_km;

    int cantidad_celdas_x;
    int cantidad_celdas_y;

    int resolucion;
    CeldaBase** grid;   // Matriz 2D de celdas

    void generarRios(int numRios, int anchoRioMax, std::mt19937 &gen);
    void generarEstanques(int numEstanques, int radioEstanqueMax, std::mt19937 &gen);
    void ajustarPorcentajeAgua(float porcentajeAgua, std::mt19937 &gen);

public:
    // Constructor / Destructor
    Mapa(float ancho_km, float alto_km, int resolucion);
    ~Mapa();

    // Acceso y límites
    bool dentroMapa(int x, int y) const;
    CeldaBase& obtenerCelda(int x, int y) const;

    // Generación procedural
    void DistribuirBiomas_Voronoi(const Bioma *Biomas, int num_biomas, int biomas_repetidos_max);
    void generarCuerposDeAgua(float porcentaje_agua, int numRios, int numEstanques, int anchoRioMax, int radioEstanqueMax);

    // Depuración / Visualización
    void graficarMapa();
    friend std::ostream& operator<<(std::ostream& os, const Mapa& mapa);

};

#endif
