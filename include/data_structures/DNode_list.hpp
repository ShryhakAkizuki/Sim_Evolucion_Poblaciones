#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>

template<typename T>
class DNode_list {
protected:
    // ----- Atributos -----
    std::shared_ptr<DNode_list> _forward = nullptr;
    std::weak_ptr<DNode_list> _backward = nullptr;

    T _data = {};

public:
    // ----- Constructores -----
    DNode_list() = default;                                                 // Constructor por defecto

    explicit DNode_list(const T& data):                                     // Constructor con dato 
    _data(data) {}

    explicit DNode_list(const T& data,                                      // Constructor con dato y nodos enlazados
                              std::shared_ptr<DNode_list> forward = nullptr, 
                              std::shared_ptr<DNode_list> backward = nullptr):
    _data(data), _forward(std::move(forward)), _backward(backward){}

    DNode_list(DNode_list&& other) noexcept = default;                      // Constructor de movimiento

    // ----- Destructor -----
    ~DNode_list() = default;

    // ----- Operadores -----
    DNode_list& operator=(DNode_list&& other) noexcept = default;           // Operador de asignación por movimiento

    // ----- Métodos -----
    
    //  Asignacion y retorno
    const T& data() const { return _data; }
    T& data() { return _data; }

    std::shared_ptr<DNode_list> forward_ptr() { return _forward; }
    std::shared_ptr<const DNode_list> forward_ptr() const { return _forward; }

    std::shared_ptr<DNode_list> backward_ptr() { return _backward.lock(); }
    std::shared_ptr<const DNode_list> backward_ptr() const { return _backward.lock(); }


    void set_data(const T& data) { _data = data; }
    void set_forward(std::shared_ptr<DNode_list> forward) { 
        _forward = std::move(forward); 
    }
    void set_backward(std::shared_ptr<DNode_list> backward) { 
        _backward = backward; 
    }

};