#pragma once
#include <memory>
#include <stdexcept>

#include "data_structures/DNode_list.hpp"

template<typename T>
class Double_Linked_List{
private:
    // ----- Atributos -----
    size_t _size = 0;
    std::unique_ptr<DNode_list<T>> _head = nullptr;
    DNode_list<T>* _tail = nullptr;

public:
    // ----- Constructores -----
    Double_Linked_List() = default;                                         // Constructor por defecto

    explicit Double_Linked_List(const T& data){                             // Constructor para inicializar un dato
        _head = std::make_unique<DNode_list<T>>(data);
        _tail = _head.get();
        _size = 1;
    }
    
    explicit Double_Linked_List(T&& data){                                  // Constructor para inicializar un dato
        _head = std::make_unique<DNode_list<T>>(std::move(data));
        _tail = _head.get();
        _size = 1;
    }

    Double_Linked_List(std::initializer_list<T> init) {                     // Constructor con lista de inicialización = {1, 2, ...}
        if (init.size() == 0) return;

        auto it = init.begin();
        _head = std::make_unique<DNode_list<T>>(*it);
        DNode_list<T>* current = _head.get();
            
        ++it;
        for (; it != init.end(); ++it) {
            std::unique_ptr<DNode_list<T>> new_node = std::make_unique<DNode_list<T>>(*it);
            current->set_forward(std::move(new_node));
            current = current->get_forward();
        }
            
        _tail = current;
        _size = init.size();
    }

    Double_Linked_List(const Double_Linked_List& other) = delete;

    Double_Linked_List(Double_Linked_List&& other) noexcept :               // Constructor de movimiento
    _size(other._size),
    _head(std::move(other._head)),
    _tail(std::move(other._tail)) { 
        
        other._size = 0;
        other._head = nullptr;
        other._tail = nullptr;
    }

    // ----- Destructor -----
    ~Double_Linked_List() = default;

    // ----- Operadores -----
    Double_Linked_List& operator=(const Double_Linked_List& other) = delete;

    Double_Linked_List& operator=(Double_Linked_List&& other) noexcept {    // Operador de asignación por movimiento
        if(this != &other) {
            _size = other._size;
            _head = std::move(other._head);
            _tail = std::move(other._tail);
                    
            other._size = 0;
            other._head = nullptr;
            other._tail = nullptr;
        }
        return *this;
    }

    T& operator[](uint32_t index) {                                         // Operador de acceso
        if( index >= _size) {
            throw std::out_of_range("Indice fuera de rango");
        }

        if (index == _size - 1) return _tail->get_data();
        
        DNode_list<T>* current = _head;
        for(uint32_t i = 0; i < index ; i++){
            current = current->get_forward();
        }
        return current->get_data();
    }
        
    const T& operator[](uint32_t index) const {                             // Operador de acceso constanter
        if( index >= _size) {
            throw std::out_of_range("Indice fuera de rango");
        }
        
        if (index == _size - 1) return _tail->get_data();

        DNode_list<T>* current = _head;
        for(uint32_t i = 0; i < index ; i++){
            current = current->get_forward();
        }
        return current->get_data();
    }

    // ----- Iteradores -----
    class iterator;
    class const_iterator;
    
    iterator begin() { return iterator(_head.get()); }
    iterator end() { return iterator(nullptr); }
    
    const_iterator begin() const { return const_iterator(_head.get()); }
    const_iterator end() const { return const_iterator(nullptr); }
    
    const_iterator cbegin() const { return const_iterator(_head.get()); }
    const_iterator cend() const { return const_iterator(nullptr); }

    // ----- Métodos -----

    // Utilidades
    bool empty() const noexcept {                                           // Booleano - Arreglo vacio
        return _size == 0;
    };

    size_t size() const noexcept {                                          // Retornar el tamaño actual de la lista
        return _size;
    }

    void clear() noexcept{                                                  // Limpia la lista
        _head = nullptr;
        _tail = nullptr;
        _size = 0;
    }

    // Push
    void push_back(const T& value){                                         // Ingresa el valor al final de la lista
        std::unique_ptr<DNode_list<T>> new_node = std::make_unique<DNode_list<T>>(value);  

        if(empty()){
            _head = std::move(new_node);
            _tail = _head.get();
        }else{
            _tail->set_forward(std::move(new_node));
            _tail = _tail->get_forward();
        }
        _size++;
    }

    void push_back(T&& value){                                              // Ingresa el valor al final de la lista con move semantics
        std::unique_ptr<DNode_list<T>> new_node = std::make_unique<DNode_list<T>>(std::move(value));

        if(empty()){
            _head = std::move(new_node);
            _tail = _head.get();
        }else{
            _tail->set_forward(std::move(new_node));
            _tail = _tail->get_forward();
        }
        _size++;
    }

    void push_front(const T& value){                                        // Ingresa el valor al inicio de la lista
        std::unique_ptr<DNode_list<T>> new_node = std::make_unique<DNode_list<T>>(value);  

        if(empty()){
            _head = std::move(new_node);
            _tail = _head.get();
        }else{
            new_node->set_forward(std::move(_head));            
            _head = std::move(new_node);
        }
        _size++;
    }

    void push_front(T&& value){                                             // Ingresa el valor al inicio de la lista
        std::unique_ptr<DNode_list<T>> new_node = std::make_unique<DNode_list<T>>(std::move(value));  

        if(empty()){
            _head = std::move(new_node);
            _tail = _head.get();
        }else{
            new_node->set_forward(std::move(_head));            
            _head = std::move(new_node);
        }
        _size++;
    }
 
    // Pop
    T pop_back() {
        if (!_tail) throw std::out_of_range("Lista vacia");
        
        // Mover, no copiar
        T value = std::move(_tail->get_data());
        
        _tail = _tail->get_backward();
        
        if (_tail != nullptr) _tail->release_forward();
        else _head = nullptr;
    
        _size--;
        return value;  // Se mueve al retorno
    }

    T pop_front() {
        if (!_head) throw std::out_of_range("Lista vacia");
        
        // Mover, no copiar
        T value = std::move(_head->get_data());
                
        _head = _head->release_forward();

        if (_head != nullptr) _head->set_backward(nullptr);
        else _tail = nullptr; 

        _size--;
        return value;  // Se mueve al retorno
    }

    // Acceso
    T& back() {                                                             // Acceso ultimo valor
        if (empty()) throw std::out_of_range("Array is empty");
        return _tail->get_data();
    }
    
    const T& back() const {                                                 // Acceso constante al ultimo valor
        if (empty()) throw std::out_of_range("Array is empty");
        return _tail->get_data();    
    }

    T& front() {                                                            // Acceso primer valor
        if (empty()) throw std::out_of_range("Array is empty");
        return _head->get_data();
    }
    
    const T& front() const {                                                // Acceso constante al primer valor
        if (empty()) throw std::out_of_range("Array is empty");
        return _head->get_data();
    }
    
    // Insert
    void insert_after(const T& value, size_t index){
        if( index >= _size) throw std::out_of_range("Indice fuera de rango");

        if(index == _size - 1) push_back(value);
        else{
            DNode_list<T>* current = _head;
            for(size_t i = 0; i < index ; i++) current = current->get_forward();
            
            insert_after(value, current);
        }
    }

    void insert_after(T&& value, size_t index){
        if( index >= _size) throw std::out_of_range("Indice fuera de rango");

        if(index == _size - 1) push_back(std::move(value));
        else{
            DNode_list<T>* current = _head;
            for(size_t i = 0; i < index ; i++) current = current->get_forward();
            
            insert_after(std::move(value), current);
        }
    }

    // Delete
    void delete_node(uint32_t index){
        if( index >= _size) throw std::out_of_range("Indice fuera de rango");

        DNode_list<T>* current = _head.get();
        for(uint32_t i = 0; i < index ; i++){
            current = current->get_forward();
        }

        delete_Node(current);
    }

    iterator erase(const_iterator pos) {
        if (pos == end() || empty()) return end();
        
        // Obtenemos el shared_ptr directamente del iterador
        DNode_list<T>* node_to_delete = pos.get_node();;
        
        // Caso especial: si es el único nodo
        if (_size == 1) {
            clear();
            return end();
        }
        
        // Encontrar el siguiente nodo antes de eliminar
        DNode_list<T>* next_node = nullptr;
        if (node_to_delete != nullptr) next_node = node_to_delete->get_forward();
        
        // Usar Delete_Node existente que ya maneja todos los casos
        delete_node(node_to_delete);
        
        // Devolver iterador al siguiente nodo
        if (next_node)  return iterator(next_node);
        else return end();
    }

    iterator erase(const_iterator first, const_iterator last) {
        auto it = first;
        while (it != last) {
            it = erase(it);
        }
        return iterator(last.get_node());
    }
    
private:
    // Insert
    void insert_after(const T& value, DNode_list<T>* node){
        if (node == nullptr) return;
        
        std::unique_ptr<DNode_list<T>> new_node = std::make_unique<DNode_list<T>>(value);
        std::unique_ptr<DNode_list<T>> list_after = node->release_forward();

        new_node->set_forward(list_after);
        node->set_forward(new_node);

        _size++;
    }

    void insert_after(T&& value, DNode_list<T>* node){
        if (node == nullptr) return;
        
        std::unique_ptr<DNode_list<T>> new_node = std::make_unique<DNode_list<T>>(std::move(value));
        std::unique_ptr<DNode_list<T>> list_after = node->release_forward();

        new_node->set_forward(list_after);
        node->set_forward(new_node);

        _size++;
    }

    // Delete
    void delete_node(DNode_list<T>* node){
        if (node == nullptr || empty()) return;

        // Caso 1: Es el head
        if (node == _head.get()) {
            pop_front();
            return;
        }
        
        // Caso 2: Es el tail
        if (node == _tail) {
            pop_back();
            return;
        }
        
        // Caso 3: Nodo intermedio
        std::unique_ptr<DNode_list<T>> list_after = node->release_forward();
        node->get_backward()->set_forward(std::move(list_after));

        _size--;
    }

public:
    // Clase - Iteradores
    class iterator{
    private:
        DNode_list<T>* _current;

    public:
        iterator(DNode_list<T>* node): _current(node){}

        T& operator*() { 
            if (!_current) throw std::runtime_error("Dereferencing end iterator");
            return _current->get_data(); 
        }
        
        T* operator->() { 
            if (!_current) throw std::runtime_error("Accessing end iterator");
            return &_current->get_data(); 
        }

        iterator& operator++(){
            if(_current) {
                _current = _current->get_forward();
            }            
            return *this;
        }
        
        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        DNode_list<T>* get_node() const { return _current; }
        
        bool operator==(const iterator& other) const {
            return _current == other._current;
        }

        bool operator!=(const iterator& other) const{
            return _current != other._current;
        }
        
        // Para comparación con const_iterator
        bool operator==(const const_iterator& other) const {
            return _current == other.get_node();
        }
        
        bool operator!=(const const_iterator& other) const {
            return _current != other.get_node();
        }
    };

    class const_iterator {
    private:
        DNode_list<T>* _current;
    
    public:
        const_iterator(DNode_list<T>* node) : _current(node) {}
        const_iterator(const iterator& other) : _current(other.get_node()) {} // Conversión

        const T& operator*() const { 
            if (!_current) throw std::runtime_error("Dereferencing end iterator");
            return _current->get_data(); 
        }
        
        const T* operator->() const { 
            if (!_current) throw std::runtime_error("Accessing end iterator");
            return &_current->get_data(); 
        }        
        
        const_iterator& operator++() { 
            if(_current) {
                _current = _current->get_forward();
            }            
            return *this;
        }
        
        const_iterator operator++(int) {
            const_iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        DNode_list<T>* get_node() const { return _current; }
        
        bool operator==(const const_iterator& other) const {
            return _current == other._current;
        }

        bool operator!=(const const_iterator& other) const {
            return _current != other._current;
        }
        
        // Para comparación con iterator
        bool operator==(const iterator& other) const {
            return _current == other.get_node();
        }
        
        bool operator!=(const iterator& other) const {
            return _current != other.get_node();
        }
    };
};