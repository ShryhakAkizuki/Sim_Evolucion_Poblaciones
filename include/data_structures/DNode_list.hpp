#pragma once
#include <memory>

template<typename T>
class DNode_list {
protected:
    // ----- Atributos -----
    std::unique_ptr<DNode_list> _forward = nullptr;
    DNode_list* _backward = nullptr;

    T _data = T();

public:
    // ----- Constructores -----
    DNode_list() = default;                                                             // Constructor por defecto

    explicit DNode_list(const T& data) : _data(data) {}                                 // Constructor con dato 
    explicit DNode_list(T&& data) : _data(std::move(data)) {}                           // Constructor con dato 
    
    explicit DNode_list(std::unique_ptr<DNode_list> forward, const T& data = T()) :     // Constructor con Nodo adelante
        _forward(std::move(forward)), _data(data) {}

    DNode_list(const T& data,                                                           // Constructor con dato y nodos enlazados
               std::unique_ptr<DNode_list> forward, 
               DNode_list* backward):
    _data(data), _forward(std::move(forward)), _backward(backward){}
    
    DNode_list(const DNode_list& other) = delete;
    DNode_list(DNode_list&& other) noexcept = default;                                  // Constructor de movimiento

    // ----- Destructor -----
    ~DNode_list() = default;

    // ----- Operadores -----
    DNode_list& operator=(const DNode_list& other) = delete;
    DNode_list& operator=(DNode_list&& other) noexcept = default;                       // Operador de asignación por movimiento

    // ----- Métodos -----
    
    //  Getters
    const T& get_data() const { return _data; }
    T& get_data() { return _data; }

    DNode_list* get_forward() { return _forward.get(); }
    const DNode_list* get_forward() const { return _forward.get(); }
    
    DNode_list* get_backward() { return _backward; }
    const DNode_list* get_backward() const { return _backward; }

    //  Setters
    void set_data(const T& Value) {_data=Value; }
    void set_data(const T&& Value) {_data=std::move(Value); }

    void set_forward(std::unique_ptr<DNode_list> forward) { 
        if(forward != nullptr) forward->_backward = this;        
        _forward = std::move(forward); 
    }
    
    void set_backward(DNode_list* backward) { 
         _backward = backward; 
    }

    // Insert
    void insert_after(std::unique_ptr<DNode_list> new_node) {
        if(new_node == nullptr) return;

        new_node->_backward = this;

        if (_forward != nullptr) new_node->set_forward(std::move(_forward));

        _forward = std::move(new_node);
    }

    //  Release
    std::unique_ptr<DNode_list> release_forward() {
        if (_forward != nullptr) _forward->_backward = nullptr;
        return std::move(_forward);
    }

    // Comparadores
    bool has_forward() const { return _forward != nullptr; }
    bool has_backward() const { return _backward != nullptr; }
};