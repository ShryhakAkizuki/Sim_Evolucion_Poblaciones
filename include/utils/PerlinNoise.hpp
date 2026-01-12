#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <cmath>

#include "data_structures/DynamicArray.hpp"

class PerlinNoise {
private:
    DynamicArray<int> p;
    
    // Función de fade mejorada para más suavidad
    double fade(double t) { 
        // Versión más suave que la original de Perlin
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    
    // Interpolación cúbica para más suavidad (opcional)
    double cubicInterpolate(double a, double b, double c, double d, double t) {
        double p = (d - c) - (a - b);
        double q = (a - b) - p;
        double r = c - a;
        double s = b;
        return p*t*t*t + q*t*t + r*t + s;
    }
    
    double lerp(double t, double a, double b) { 
        return a + t * (b - a); 
    }
    
    double grad(int hash, double x, double y, double z) {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }
    
public:
    PerlinNoise(uint64_t seed) {
        // Inicializar con 256 valores
        p = DynamicArray<int>(256);
        
        // Llenar con 0-255
        for (int i = 0; i < 256; ++i) {
            p[i] = i;
        }
        
        // Barajar usando la semilla
        std::mt19937_64 rng(seed);
        for (int i = 255; i > 0; --i) {
            int j = rng() % (i + 1);
            int temp = p[i];
            p[i] = p[j];
            p[j] = temp;
        }
        
        // Duplicar para evitar overflow
        DynamicArray<int> p2(512);
        for (int i = 0; i < 512; ++i) {
            p2[i] = p[i % 256];
        }
        p = std::move(p2);
    }

    PerlinNoise() {
        // Inicializar con 256 valores
        p = DynamicArray<int>(256);
        
        // Llenar con 0-255
        for (int i = 0; i < 256; ++i) {
            p[i] = i;
        }
        
        // Barajar usando la semilla
        std::mt19937_64 rng(12345);
        for (int i = 255; i > 0; --i) {
            int j = rng() % (i + 1);
            int temp = p[i];
            p[i] = p[j];
            p[j] = temp;
        }
        
        // Duplicar para evitar overflow
        DynamicArray<int> p2(512);
        for (int i = 0; i < 512; ++i) {
            p2[i] = p[i % 256];
        }
        p = std::move(p2);
    }
    
    // Ruido básico mejorado
    double noise(double x, double y, double z = 0.0) {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;
        int Z = (int)std::floor(z) & 255;
        
        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);
        
        double u = fade(x);
        double v = fade(y);
        double w = fade(z);
        
        int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
        int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
        
        return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
                                   grad(p[BA], x - 1, y, z)),
                           lerp(u, grad(p[AB], x, y - 1, z),
                                   grad(p[BB], x - 1, y - 1, z))),
                   lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
                                   grad(p[BA + 1], x - 1, y, z - 1)),
                           lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                                   grad(p[BB + 1], x - 1, y - 1, z - 1))));
    }
    
    // Ruido suavizado con múltiples técnicas
    double smoothNoise(double x, double y, double z = 0.0, int octaves = 3) {
        double total = 0.0;
        double frequency = 1.0;
        double amplitude = 1.0;
        double maxValue = 0.0;
        
        for (int i = 0; i < octaves; ++i) {
            total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= 0.5;
            frequency *= 2.0;
        }
        
        return total / maxValue;
    }
    
    // Ruido con filtro de suavizado
    double filteredNoise(double x, double y, double z = 0.0, double filterSize = 0.5) {
        double total = 0.0;
        double samples = 0.0;
        
        // Muestreo simple para suavizar
        for (double dx = -filterSize; dx <= filterSize; dx += filterSize) {
            for (double dy = -filterSize; dy <= filterSize; dy += filterSize) {
                total += noise(x + dx, y + dy, z);
                samples += 1.0;
            }
        }
        
        return total / samples;
    }
    
    // NUEVO: Ruido de ríos optimizado
    double riverNoise(double x, double y, double mainScale, double meanderingScale, double edgeScale) {
        // Ruido principal para el curso del río
        double mainRiver = smoothNoise(x * mainScale, y * mainScale, 0.0, 2);
        
        // Ruido de meandros para hacerlo más natural
        double meanders = noise(x * meanderingScale, y * meanderingScale, 2000.0) * 0.2;
        
        // Ruido de bordes para suavizar transiciones
        double edges = filteredNoise(x * edgeScale, y * edgeScale, 4000.0, 0.3) * 0.3;
        
        return mainRiver + meanders + edges;
    }
};