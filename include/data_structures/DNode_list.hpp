#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>

template<typename T>
class DNode_list {
protected:
    // ----- Atributos -----
    std::unique_ptr<DNode_list> _forward = nullptr;
    DNode_list* _backward = nullptr;

    T _data = {};

public:
    // ----- Constructores -----
    DNode_list() = default;                                                 // Constructor por defecto

    explicit DNode_list(const T& data):                                     // Constructor con dato 
    _data(data) {}

    explicit DNode_list(const T& data,                                      // Constructor con dato y nodos enlazados
                              std::unique_ptr<DNode_list> forward = nullptr, 
                              DNode_list* backward = nullptr):
    _data(data), _forward(std::move(forward)), _backward(backward) {}

    DNode_list(DNode_list&& other) noexcept = default;                      // Constructor de movimiento

    // ----- Destructor -----
    ~DNode_list() = default;


    // ----- Operadores -----
    DNode_list& operator=(DNode_list&& other) noexcept = default;           // Operador de asignación por movimiento

    // ----- Métodos -----
    
    //  Asignacion y retorno
    const T& data() const { return _data; }
    T& data() { return _data; }
    const DNode_list* forward() const { return _forward.get(); }
    const DNode_list* backward() const { return _backward; }
    DNode_list* forward() { return _forward.get(); }
    DNode_list* backward() { return _backward; }

    void set_data(const T& data) { _data = data; }
    void set_forward(std::unique_ptr<DNode_list> forward) { 
        _forward = std::move(forward); 
    }
    void set_backward(DNode_list *backward) { 
        _backward =  backward; 
    }

};