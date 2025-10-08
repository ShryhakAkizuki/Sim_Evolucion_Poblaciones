#include "mapa.hpp"
#include "estructuras.hpp"
#include <cmath>
#include <random>
#include <cstring>
#include <numbers>
#include <iostream>
#include <algorithm>

// ------ Clase Bioma ------------------ //
 
Bioma::Bioma(int identificador, std::string nombre, Color ColorBioma,
float temp_media, float hum_media   , float rad_media   , float mov_media   , float expan_media , float reduc_media,
float temp_std  , float hum_std     , float rad_std     , float mov_std     , float expan_std   , float reduc_std)
: id(identificador), nombre(nombre), ColorBioma(ColorBioma),
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

std::string Bioma::getBiomaNombre() const {
    return nombre;
}

Color Bioma::getBiomaColor() const {
    return ColorBioma;
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
        for (int x = 0; x < cantidad_celdas_x; ++x) {
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

CeldaBase& Mapa::getCelda(int x, int y) const{
    return grid[x][y];
}

int Mapa::getMapaCeldas_x() const{
    return cantidad_celdas_x;
}

int Mapa::getMapaCeldas_y() const{
    return cantidad_celdas_y;
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

void Mapa::pincel_agua(int x, int y, int r_x, int r_y, bool circular, bool valorAgua){
    for(int dy = -r_y; dy<=r_y; ++dy){
        for(int dx = -r_x; dx<=r_x; ++dx){
            if(circular && 
            (dx*dx/static_cast<float>(r_x*r_x)+dy*dy/static_cast<float>(r_y*r_y)>1.0f)){
                continue;
            }
            
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < cantidad_celdas_x && ny >= 0 && ny < cantidad_celdas_y){
                grid[ny][nx].tiene_agua = valorAgua;
            }
        }
    }

}

int Mapa::pincel_agua_retorno(int x, int y, int r_x, int r_y, bool circular, bool valorAgua){
    int cantidad_agua= 0;
    for(int dy = -r_y; dy<=r_y; ++dy){
        for(int dx = -r_x; dx<=r_x; ++dx){
            if(circular && 
            (dx*dx/static_cast<float>(r_x*r_x)+dy*dy/static_cast<float>(r_y*r_y)>1.0f)){
                continue;
            }
            
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < cantidad_celdas_x && ny >= 0 && ny < cantidad_celdas_y && !grid[ny][nx].tiene_agua){
                grid[ny][nx].tiene_agua = valorAgua;
                cantidad_agua++;
            }
        }
    }
    return cantidad_agua;
}

void Mapa::generarRios(int numRios, int anchoRioMax, std::mt19937 &gen, const int &objetivoAgua, int &aguaActual){
    std::uniform_int_distribution<int> distX(0, cantidad_celdas_x-1);
    std::uniform_int_distribution<int> distY(0, cantidad_celdas_y-1);
    std::uniform_int_distribution<int> distAnchoRio(static_cast<int>(anchoRioMax/6), anchoRioMax);
    std::uniform_real_distribution<float> distCaos(-1.0f, 1.0f);
    std::uniform_int_distribution<int> distBorde(0,3); 

    for(int r=0; r<numRios && aguaActual < objetivoAgua; ++r){
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

        float dirX = (x_dest - x);
        float dirY = (y_dest - y);
        float mag = std::sqrt(dirX * dirX + dirY * dirY);
        dirX /= mag; dirY /= mag; 
        float desviacionX = 0.0f, desviacionY = 0.0f;

            
        while(x != x_dest || y != y_dest) {
            int a = distAnchoRio(gen);
            aguaActual+=pincel_agua_retorno(x,y,a,a,true,true);

            desviacionX += distCaos(gen) * 0.2f;  // menor valor = ríos más suaves
            desviacionY += distCaos(gen) * 0.2f;

            if (desviacionX > 1.5f) desviacionX = 1.5f;
            else if (desviacionX < -1.5f) desviacionX = -1.5f;

            if (desviacionY > 1.5f) desviacionY = 1.5f;
            else if (desviacionY < -1.5f) desviacionY = -1.5f;

            x += static_cast<int>(dirX + desviacionX);
            y += static_cast<int>(dirY + desviacionY);

            if(x<0) x=0; else if(x>=cantidad_celdas_x) x=cantidad_celdas_x-1;
            if(y<0) y=0; else if(y>=cantidad_celdas_y) y=cantidad_celdas_y-1;

            dirX = static_cast<float>(x_dest - x);
            dirY = static_cast<float>(y_dest - y);
            mag = std::sqrt(dirX * dirX + dirY * dirY);
            if (mag > 0.1f) {
                dirX /= mag;
                dirY /= mag;
            }
        }
    }
}

void Mapa::generarEstanques(int numEstanques, int radioEstanqueMax, std::mt19937 &gen, const int &objetivoAgua, int &aguaActual){
    std::uniform_int_distribution<int> distX(0, cantidad_celdas_x-1);
    std::uniform_int_distribution<int> distY(0, cantidad_celdas_y-1);
    std::uniform_int_distribution<int> distRadioEstanque(static_cast<int>(radioEstanqueMax/2), radioEstanqueMax);
    std::uniform_real_distribution<float> distProb(0.0f, 1.0f);

    for(int e=0; e<numEstanques && aguaActual < objetivoAgua; ++e){

        int rx = distRadioEstanque(gen); 
        int ry = distRadioEstanque(gen); 
        bool generarLejos = distProb(gen) < 0.5f;

        int x = 0;
        int y = 0;
        bool posicionValida = false;

        for (int intento = 0; intento < 1000 && !posicionValida; ++intento) {
            x = distX(gen);
            y = distY(gen);

            bool hayAguaCerca = false;
            int radioBusqueda_x = rx * 2; 
            int radioBusqueda_y = ry * 2; 

            for (int dy = -radioBusqueda_y; dy <= radioBusqueda_y && !hayAguaCerca; ++dy) {
                for (int dx = -radioBusqueda_x; dx <= radioBusqueda_x; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < cantidad_celdas_x && ny >= 0 && ny < cantidad_celdas_y) {
                        if (grid[ny][nx].tiene_agua) {
                            hayAguaCerca = true;
                            break;
                        }
                    }
                }
            }

            if (generarLejos && hayAguaCerca)
                continue;

            if (!generarLejos && !hayAguaCerca)
                continue;

            posicionValida = true;
        }

        if (posicionValida) aguaActual+=pincel_agua_retorno(x, y, rx, ry, true, true);
    }
}

void Mapa::generarSubRios(int anchoRioMax, std::mt19937 &gen, const int &objetivoAgua, int &aguaActual) {
    std::uniform_int_distribution<int> distX(0, cantidad_celdas_x - 1);
    std::uniform_int_distribution<int> distY(0, cantidad_celdas_y - 1);
    std::uniform_real_distribution<float> distAngulo(0, std::numbers::pi/3);
    std::uniform_real_distribution<float> distVariacion(-0.3f, 0.3f);
    std::uniform_int_distribution<int> distLongitud(std::min(cantidad_celdas_x,cantidad_celdas_y)/16, std::min(cantidad_celdas_x,cantidad_celdas_y)/8);
    std::uniform_int_distribution<int> distRadio(2*anchoRioMax/3, anchoRioMax);

    while (aguaActual < objetivoAgua) {
        int x = distX(gen);
        int y = distY(gen);
        bool encontrado = false;

        for (int intentos = 0; intentos < 1000 && !encontrado; ++intentos) {
            int px = distX(gen);
            int py = distY(gen);
            if (grid[py][px].tiene_agua) {
                x = px; y = py;
                encontrado = true;
            }
        }

        if (!encontrado) continue; 

        float angulo = distAngulo(gen);
        float variacion = distVariacion(gen);
        int longitud = distLongitud(gen);

        for (int paso = 0; paso < longitud; ++paso) {
            int rX = distRadio(gen);
            int rY = distRadio(gen);

            aguaActual += pincel_agua_retorno(x, y, rX, rY, true, true);

            if (aguaActual >= objetivoAgua)
                return;

            angulo += variacion;
            int nuevo_x = x + static_cast<int>(std::cos(angulo) * (rX / 2 + 1));
            int nuevo_y = y + static_cast<int>(std::sin(angulo) * (rY / 2 + 1));

            if (nuevo_x < 0 || nuevo_x >= cantidad_celdas_x || nuevo_y < 0 || nuevo_y >= cantidad_celdas_y)
                break;

            x = nuevo_x;
            y = nuevo_y;
        }
    }
}

void Mapa::generarCuerposDeAgua(float porcentajeAgua, int numRios, int numEstanques, int anchoRioMax, int radioEstanqueMax, int anchoSubRioMax) {
    std::mt19937 gen(std::random_device{}());

    int totalCeldas = cantidad_celdas_x * cantidad_celdas_y;
    int objetivoAgua = static_cast<int>(porcentajeAgua * totalCeldas);
    std::cout<<objetivoAgua<<std::endl;
    int aguaActual = 0;

    generarRios(numRios, anchoRioMax, gen, objetivoAgua, aguaActual);
    generarEstanques(numEstanques, radioEstanqueMax, gen, objetivoAgua, aguaActual);
    generarSubRios(anchoSubRioMax, gen, objetivoAgua, aguaActual);
    std::cout<<aguaActual<<std::endl;

}

std::ostream& operator<<(std::ostream& os, const Mapa& mapa) {

    for (int y = mapa.cantidad_celdas_y - 1; y >= 0; --y) {
        for (int x = 0; x < mapa.cantidad_celdas_x; ++x) {
            const CeldaBase& celda = mapa.grid[y][x];
            if (celda.tiene_agua){
                os <<" " << "~" <<" ";
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