#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include "data_structures/DNode_list.hpp"

template<typename T>
class Double_Linked_List{
private:
    // ----- Atributos -----
    uint32_t _size = 0;
    std::shared_ptr<DNode_list<T>> _head = nullptr;
    std::weak_ptr<DNode_list<T>> _tail = nullptr;

public:
    // ----- Constructores -----
    Double_Linked_List() = default;                                         // Constructor por defecto

    explicit Double_Linked_List(const T& data){                             // Constructor para inicializar un dato
        _head = std::make_shared<DNode_list<T>>(data);
        _tail = _head;
        _size = 1;
    }

    Double_Linked_List(std::initializer_list<T> init) {                     // Constructor con lista de inicialización = {1, 2, ...}
        auto it = init.begin();
        _head = std::make_shared<DNode_list<T>>(*it);
        auto current = _head;
            
        ++it;
        for (; it != init.end(); ++it) {
            auto new_node = std::make_shared<DNode_list<T>>(*it);
            current->set_forward(new_node);
            new_node->set_backward(current);
            current = new_node;
        }
            
        _tail = current;
        _size = static_cast<uint32_t>(init.size());
    }

    Double_Linked_List(const Double_Linked_List& other) : _size(0){         // Constructor de copia
        for (auto it = other.begin(); it != other.end(); ++it) {
            push_back(*it);  
        }
    }

    Double_Linked_List(Double_Linked_List&& other) noexcept :
    _size(other._size),
    _head(std::move(other._head)),
    _tail(std::move(other._tail)) { 
        
        other._size = 0;
        other._head = nullptr;
        other._tail.reset();
    }

    // ----- Destructor -----
    ~Double_Linked_List() = default;

    // ----- Operadores -----
    Double_Linked_List& operator=(const Double_Linked_List& other) {        // Operador de asignación por copia
        if(this != &other) {
            _size = 0;
            for (auto it = other.begin(); it != other.end(); ++it) {
                push_back(*it);  
            }
        }
        return *this;
    }

    Double_Linked_List& operator=(Double_Linked_List&& other) noexcept {    // Operador de asignación por movimiento
        if(this != &other) {
            _size = other._size;
            _head = std::move(other._head);
            
            if (auto tail_ptr = other._tail.lock()) {
                _tail = tail_ptr; 
            }
                    
            other._size = 0;
            other._head = nullptr;
            other._tail.reset();    
        }
        return *this;
    }

    T& operator[](uint32_t index) {                                         // Operador de acceso
        if( index >= _size) {
            throw std::out_of_range("Indice fuera de rango");
        }
        
        auto current = _head;
        for(uint32_t i = 0; i < index ; i++){
            current = current->forward_ptr();
        }
        return current->data();
    }
        
    const T& operator[](uint32_t index) const {                             // Operador de acceso constanter
        if( index >= _size) {
            throw std::out_of_range("Indice fuera de rango");
        }
        
        auto current = _head;
        for(uint32_t i = 0; i < index ; i++){
            current = current->forward_ptr();
        }
        return current->data();
    }

    // ----- Iteradores -----

    iterator begin() { return iterator(_head.get()); }
    iterator end() { return iterator(nullptr); }
    
    const_iterator begin() const { return const_iterator(_head.get()); }
    const_iterator end() const { return const_iterator(nullptr); }
    
    const_iterator cbegin() const { return const_iterator(_head.get()); }
    const_iterator cend() const { return const_iterator(nullptr); }

    // ----- Métodos -----

    // Capacidad
    bool empty() const noexcept {                                           // Booleano - Arreglo vacio
        return _size == 0;
    };

    uint32_t size() const noexcept {                                        // Retornar el tamaño actual de la lista
        return _size;
    }

    // Asignacion y retorno
    void clear() noexcept{                                                  // Limpia la lista
        _head = nullptr;
        _tail = nullptr;
        _size = 0;
    }

    void push_back(const T& value){                                         // Ingresa el valor al final de la lista
        
        auto new_node = std::make_shared<DNode_list<T>>(value);  

        if(empty()){
            _head = new_node;
            _tail = new_node;
        }else{
            auto last_node = _tail.lock();
            last_node->set_forward(new_node);
            new_node->set_backward(last_node);

            _tail = new_node;
        }
        _size++;
    }

    void push_back(T&& value){                                              // Ingresa el valor al final de la lista con move semantics
        auto new_node = std::make_shared<DNode_list<T>>(std::move(value));

        if(empty()){
            _head = new_node;
            _tail = new_node;
        }else{
            auto last_node = _tail.lock();
            last_node->set_forward(new_node);
            new_node->set_backward(last_node);

            _tail = new_node;
        }
        _size++;
    }

    T pop_back(){                                                           // Retira el ultimo valor de la lista 
        if(_size < 1){
            throw std::out_of_range("No hay elementos para retirar del arreglo");
        }
        
        auto last_node = _tail.lock();
        T value = last_node->data();

        if (_size == 1){
            _head = nullptr;
            _tail = nullptr;
        }else{
            auto new_tail = last_node->backward_ptr();
            new_tail->set_forward(nullptr);
            _tail = new_tail;
        }

        _size--;
        return value;
    }

    void push_front(const T& value){                                              // Ingresa el valor al inicio de la lista
        
        auto new_node = std::make_shared<DNode_list<T>>(value);  

        if(empty()){
            _head = new_node;
            _tail = new_node;
        }else{
            new_node->set_forward(_head);
            _head->set_backward(new_node);
            
            _head = new_node;
        }
        _size++;
    }

    void push_front(T&& value){                                             // Ingresa el valor al inicio de la lista
        
        auto new_node = std::make_shared<DNode_list<T>>(std::move(value));  

        if(empty()){
            _head = new_node;
            _tail = new_node;
        }else{
            new_node->set_forward(_head);
            _head->set_backward(new_node);
            
            _head = new_node;
        }
        _size++;
    }

    T pop_front(){                                                          // Retira el primer valor de la lista 
        if(_size < 1){                                                      
            throw std::out_of_range("No hay elementos para retirar del arreglo");
        }
        
        T value = _head->data();

        if (_size == 1){
            _head = nullptr;
            _tail = nullptr;
        }else{
            auto new_head = _head->forward_ptr();
            new_head->set_backward(nullptr);
            _head = new_head;
        }

        _size--;
        return value;
    }

    T& front() {                                                            // Acceso primer valor
        if (empty()) throw std::out_of_range("Array is empty");
        return _head->data();
    }
    
    const T& front() const {                                                // Acceso constante al primer valor
        if (empty()) throw std::out_of_range("Array is empty");
        return _head->data();
    }
    
    T& back() {                                                             // Acceso ultimo valor
        if (empty()) throw std::out_of_range("Array is empty");
        return _tail.lock()->data();
    }
    
    const T& back() const {                                                 // Acceso constante al ultimo valor
        if (empty()) throw std::out_of_range("Array is empty");
        return _tail.lock()->data();    
    }

    void Insert_after(const T& value, uint32_t index){
        if( index >= _size) {
            throw std::out_of_range("Indice fuera de rango");
        }

        if(index == _size - 1){
            push_back(value);
        }else{
            auto new_node = std::make_shared<DNode_list<T>>(value);

            auto current = _head;
            for(uint32_t i = 0; i < index ; i++){
                current = current->forward_ptr();
            }

            new_node->set_forward(current->forward_ptr());
            new_node->set_backward(current);

            if (auto next_node = current->forward_ptr()) {
                next_node->set_backward(new_node);
            }


            current->set_forward(new_node);
            _size++;
        }
    }

    void Insert_after(const T& value, std::shared_ptr<DNode_list<T>> node){
        if (!node) return;
        
        auto new_node = std::make_shared<DNode_list<T>>(value);
        new_node->set_forward(node->forward_ptr());
        new_node->set_backward(node);

        if (auto next_node = node->forward_ptr()) {
            next_node->set_backward(new_node);
        }

        node->set_forward(new_node);

        // Actualizar tail si es necesario
        if (node == _tail.lock()) {
            _tail = new_node;
        }

        _size++;
    }

    void Delete_Node(uint32_t index){
        if( index >= _size) {
            throw std::out_of_range("Indice fuera de rango");
        }

        if(index == 0){
            pop_front();
        }else if(index == _size -1){
            pop_back();
        }else{
            auto current = _head;
            for(uint32_t i = 0; i < index ; i++){
                current = current->forward_ptr();
            }

            auto prev_node = current->backward_ptr();
            auto next_node = current->forward_ptr();
        
            if (prev_node) prev_node->set_forward(next_node);
            if (next_node) next_node->set_backward(prev_node);
            
            _size--;
        }
    }

    void Delete_Node(std::shared_ptr<DNode_list<T>> node){
        if (!node || empty()) return;

        // Caso 1: Es el head
        if (node == _head) {
            pop_front();
            return;
        }
        
        // Caso 2: Es el tail
        if (node == _tail.lock()) {
            pop_back();
            return;
        }
        
        // Caso 3: Nodo intermedio
        auto prev_node = node->backward_ptr();
        auto next_node = node->forward_ptr();
        
        if (prev_node) prev_node->set_forward(next_node);
        if (next_node) next_node->set_backward(prev_node);
        
        _size--;
    }



private:
    // ----- Iteradores -----

    class iterator{
    private:
        DNode_list<T>* _current;

    public:
        iterator(DNode_list<T>* node): _current(node){}

        T& operator*() {return _current->data(); }
        
        iterator& operator++(){
            if(_current) {
                _current = _current->forward_ptr().get();
            }            
            return *this;
        }
        
        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator!=(const iterator& other) const{
            return _current != other._current;
        }
    };

    class const_iterator {
    private:
        const DNode_list<T>* _current;
    
    public:
        const_iterator(const DNode_list<T>* node) : _current(node) {}
        
        const T& operator*() const { return _current->data(); }
        
        const_iterator& operator++() { 
            if(_current) {
                _current = _current->forward_ptr().get();
            }            
            return *this;
        }
        
        const_iterator operator++(int) {
            const_iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator!=(const const_iterator& other) const {
            return _current != other._current;
        }
    };
};