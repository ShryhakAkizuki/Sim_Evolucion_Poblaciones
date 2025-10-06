#include "mapa.hpp"
#include "estructuras.hpp"
#include <cmath>
#include <random>
#include <cstring>
#include <numbers>
#include <iostream>
#include <algorithm>

// ------ Clase Bioma ------------------ //
 
Bioma::Bioma(int identificador, std::string nombre,
float temp_media, float hum_media   , float rad_media   , float mov_media   , float expan_media , float reduc_media,
float temp_std  , float hum_std     , float rad_std     , float mov_std     , float expan_std   , float reduc_std)
: id(identificador), nombre(nombre),
temperatura_media(temp_media)   , humedad_media(hum_media)  , radiacion_solar_media(rad_media)  , costo_movimiento_media(mov_media) , tasa_expansion_recursos_media(expan_media)    , tasa_reduccion_recursos_media(reduc_media),
temperatura_std(temp_std)       , humedad_std(hum_std)      , radiacion_solar_std(rad_std)      , costo_movimiento_std(mov_std)     , tasa_expansion_recursos_std(expan_std)        , tasa_reduccion_recursos_std(reduc_std),
gen(std::random_device{}()), dist(-1.0f, 1.0f){}

void Bioma::actualizarEstado(float hora_del_dia) {

    float factor_1 = sin(((hora_del_dia - 6.0f) / 24.0f) * 2.0f * std::numbers::pi);
    float factor_2 = sin((2.0f*(hora_del_dia + 3.0f) / 24.0f) * 2.0f * std::numbers::pi);

    Estado_Bioma_Actual.temperatura = temperatura_media + (factor_1 + dist(gen)*0.05f) * temperatura_std;
    Estado_Bioma_Actual.humedad = humedad_media + (factor_1 + dist(gen)*0.05f) * humedad_std;
    Estado_Bioma_Actual.radiacion_solar = radiacion_solar_media + (factor_1 + dist(gen)*0.05f) * radiacion_solar_std;
    Estado_Bioma_Actual.costo_movimiento = costo_movimiento_media + (factor_2 + dist(gen)*0.05f) * costo_movimiento_std;
        
    Parametros_Bioma_Actual.expansion = tasa_expansion_recursos_media + (factor_1 + dist(gen)*0.05f) * tasa_expansion_recursos_std;
    Parametros_Bioma_Actual.reduccion = tasa_reduccion_recursos_media - (factor_1 - dist(gen)*0.05f) * tasa_reduccion_recursos_std;
}

EstadosBasico Bioma::getEstadoBioma() const {
    return Estado_Bioma_Actual;
}

ParametrosBasico Bioma::getParametrosBioma() const {
    return Parametros_Bioma_Actual;
}

int Bioma::getBiomaId() const {
    return id;
}

// ------ Clase Mapa ------------------ //

Mapa::Mapa(float ancho_km, float alto_km, int resolucion)
: ancho_km(ancho_km), alto_km(alto_km), resolucion(resolucion){

    cantidad_celdas_x = static_cast<int>(ancho_km*1000/resolucion);
    cantidad_celdas_y = static_cast<int>(alto_km*1000/resolucion);

    grid = new CeldaBase*[cantidad_celdas_y];
    for(int y = 0; y < cantidad_celdas_y; ++y){
        grid[y] = new CeldaBase[cantidad_celdas_x];
    }
    
    for (int y = 0; y < cantidad_celdas_y; ++y){
        for (int x = 0; x < cantidad_celdas_y; ++x) {
            grid[y][x].bioma = nullptr;
            grid[y][x].tiene_agua = false;
        }
    }

}

Mapa::~Mapa(){
    for (int y = 0; y < cantidad_celdas_y; ++y){
        for (int x = 0; x < cantidad_celdas_y; ++x) {
            grid[y][x].bioma = nullptr;
        }
        delete[] grid[y];
    }
    delete[] grid;
}

bool Mapa::dentroMapa(int x, int y) const{
    int idx_x = x + static_cast<int>(ancho_km / 2);
    int idx_y = x + static_cast<int>(ancho_km / 2);

    return (idx_x >= 0) && (idx_x < cantidad_celdas_x) && (idx_y >= 0) && (idx_y < cantidad_celdas_y);
}

CeldaBase& Mapa::obtenerCelda(int x, int y) const{
    int idx_x = x + static_cast<int>(ancho_km / 2);
    int idx_y = x + static_cast<int>(ancho_km / 2);

    return grid[idx_x][idx_y];
}

// void Mapa::DistribuirBiomas(const Bioma *biomas, int num_biomas){
    
//     std::mt19937 gen(std::random_device{}());
//     std::uniform_int_distribution<int> dist(0, num_biomas - 1);

//     for (int y = 0; y < cantidad_celdas_y; ++y) {
//         for (int x = 0; x < cantidad_celdas_x; ++x) {
//             int idx = dist(gen);             // elige un bioma aleatorio
//             grid[y][x].bioma = &biomas[idx]; // asigna puntero al bioma
//         }
//     }
// }


void Mapa::DistribuirBiomas_Voronoi(const Bioma *biomas, int num_biomas, int biomas_repetidos_max){
    
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> Dist_Ubicacion_x(0, cantidad_celdas_x - 1);
    std::uniform_int_distribution<int> Dist_Ubicacion_y(0, cantidad_celdas_x - 1);
    std::uniform_int_distribution<int> Cant_Bioma(1, biomas_repetidos_max);

    Lista_sencilla<Semilla> semillas;
    Lista_sencilla<Cord> Posicion_semillas;

    for (int i = 0; i < num_biomas; ++i) {
        int cant_semillas = Cant_Bioma(gen);

        for (int s = 0; s < cant_semillas; ++s){
            int x, y;
            
            do{
                x = Dist_Ubicacion_x(gen);
                y = Dist_Ubicacion_y(gen);
            }while(Posicion_semillas.find(Cord{x,y}));

            Posicion_semillas.push_front(Cord{x,y});
            semillas.push_front(Semilla{Cord{x,y}, &biomas[i]});
        }
    }
 
    for(int y = 0; y < cantidad_celdas_y; ++y){
        for(int x = 0; x < cantidad_celdas_x; ++x){
            int minDist = cantidad_celdas_x*cantidad_celdas_y;
            const Bioma* bioma_cercano = nullptr;
            
            for(Nodo_simple<Semilla>* nodo = semillas.getHead(); nodo!=nullptr; nodo = nodo -> siguiente){
                int dx = x - nodo->datos.Coordenada.x;
                int dy = y - nodo->datos.Coordenada.y;

                int dist = dx*dx + dy*dy;

                if(dist < minDist){
                    minDist = dist;
                    bioma_cercano = nodo->datos.bioma;
                }
            }

            grid[y][x].bioma = bioma_cercano;
        }
    }
}

void Mapa::generarRios(int numRios, int anchoRioMax, std::mt19937 &gen){
    std::uniform_int_distribution<int> distX(0, cantidad_celdas_x-1);
    std::uniform_int_distribution<int> distY(0, cantidad_celdas_y-1);
    std::uniform_int_distribution<int> distAnchoRio(1, anchoRioMax);
    std::uniform_int_distribution<int> distZigZag(-1, 1);
    std::uniform_int_distribution<int> distBorde(0,3); 

    for(int r=0; r<numRios; ++r){
        int x = distX(gen);
        int y = distY(gen);

        int borde = distBorde(gen);
        int x_dest, y_dest;

        switch(borde){
            case 0: x_dest = distX(gen); y_dest = 0; break;
            case 1: x_dest = distX(gen); y_dest = cantidad_celdas_y-1; break;
            case 2: x_dest = 0; y_dest = distY(gen); break;
            case 3: x_dest = cantidad_celdas_x-1; y_dest = distY(gen); break;
        }

            
        while(x != x_dest || y != y_dest) {
        int a = distAnchoRio(gen);

            for(int dx=-a; dx<=a; ++dx){
                for(int dy=-a; dy<=a; ++dy){
                    int nx=x+dx; 
                    int ny=y+dy;

                    if(nx>=0 && nx< cantidad_celdas_x && ny>=0 && ny<cantidad_celdas_y){
                        grid[ny][nx].tiene_agua = true;
                    }
                }
            }


        int dx = (x_dest > x ? 1 : (x_dest < x ? -1 : 0));
        int dy = (y_dest > y ? 1 : (y_dest < y ? -1 : 0));
        
        x += dx + distZigZag(gen);
        y += dy + distZigZag(gen);

        if(x<0) x=0; else if(x>=cantidad_celdas_x) x=cantidad_celdas_x-1;
        if(y<0) y=0; else if(y>=cantidad_celdas_y) y=cantidad_celdas_y-1;

        }
    }

}

void Mapa::generarEstanques(int numEstanques, int radioEstanqueMax, std::mt19937 &gen){
    std::uniform_int_distribution<int> distX(0, cantidad_celdas_x-1);
    std::uniform_int_distribution<int> distY(0, cantidad_celdas_y-1);
    std::uniform_int_distribution<int> distRadioEstanque(1, radioEstanqueMax);

    for(int e=0; e<numEstanques; ++e){
        int x = distX(gen);
        int y = distY(gen);
        int rx = distRadioEstanque(gen); 
        int ry = distRadioEstanque(gen); 

        for(int dx=-rx; dx<=rx; ++dx){
            for(int dy=-ry; dy<=ry; ++dy){
                int nx=x+dx;
                int ny=y+dy;
                
                if(nx>=0 && nx< cantidad_celdas_x && ny>=0 && ny<cantidad_celdas_y){
                    grid[ny][nx].tiene_agua = true;
                }
            }
        }
    }
}

void Mapa::ajustarPorcentajeAgua(float porcentajeAgua, std::mt19937 &gen){
    std::uniform_int_distribution<int> distX(0, cantidad_celdas_x-1);
    std::uniform_int_distribution<int> distY(0, cantidad_celdas_y-1);
    std::uniform_int_distribution<int> distCelda(0, cantidad_celdas_x*cantidad_celdas_y-1);
    
    int totalCeldas = cantidad_celdas_x * cantidad_celdas_y;
    int objetivoAgua = static_cast<int>(porcentajeAgua*totalCeldas);

    int actualAgua = 0;
    for(int y = 0; y < cantidad_celdas_y; ++y){
        for(int x = 0; x < cantidad_celdas_x; ++x){
            if(grid[y][x].tiene_agua){
                actualAgua++;
            }
        }
    }

    while(actualAgua != objetivoAgua){
        int x = distX(gen);
        int y = distY(gen);    

        if(!grid[y][x].tiene_agua && actualAgua<objetivoAgua) {
            grid[y][x].tiene_agua = true;
            actualAgua++;
        }else if(grid[y][x].tiene_agua && actualAgua>objetivoAgua){
            grid[y][x].tiene_agua = false;
            actualAgua--;
        }
    }

}

void Mapa::generarCuerposDeAgua(float porcentajeAgua, int numRios, int numEstanques, int anchoRioMax, int radioEstanqueMax) {
    std::mt19937 gen(std::random_device{}());

    generarRios(numRios, anchoRioMax, gen);
    generarEstanques(numEstanques, radioEstanqueMax, gen);
    ajustarPorcentajeAgua(porcentajeAgua, gen);
}

std::ostream& operator<<(std::ostream& os, const Mapa& mapa) {

    for (int y = mapa.cantidad_celdas_y - 1; y >= 0; --y) {
        for (int x = 0; x < mapa.cantidad_celdas_x; ++x) {
            const CeldaBase& celda = mapa.grid[y][x];
            if (celda.tiene_agua){
                os << "~";
            }else{
                if(celda.bioma == nullptr){
                    os << " -1 ";
                }else{
                    os <<" "<< celda.bioma->getBiomaId()<<" ";
                }
            }    
        }
        os << '\n';
    }
    return os;
}