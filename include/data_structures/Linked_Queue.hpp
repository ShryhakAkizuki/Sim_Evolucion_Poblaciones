#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>

#include "data_structures/Double_Link_List.hpp"

template<typename T>
class Linked_Queue{
private:
    // ----- Atributos -----
    Double_Linked_List<T> _list;

public:
    // ----- Constructores -----
    Linked_Queue() = default;

    explicit Linked_Queue(const T& data) : _list(data) {}
    
    explicit Linked_Queue(T&& data){ _list(std::move(data));}

    Linked_Queue(std::initializer_list<T> init): _list(init) {}

    Linked_Queue(const Linked_Queue& other) = default;

    Linked_Queue(Linked_Queue&& other) noexcept = default;

    // ----- Destructor -----
    ~Linked_Queue() = default;

    // ----- Operadores -----
    Linked_Queue& operator=(const Linked_Queue& other) = default;

    Linked_Queue& operator=(Linked_Queue&& other) noexcept = default;

    // ----- Iteradores -----
    typename Double_Linked_List<T>::iterator begin() {
        return _list.begin();
    }
    
    typename Double_Linked_List<T>::iterator end() {
        return _list.end();
    }
    
    typename Double_Linked_List<T>::const_iterator begin() const {
        return _list.begin();
    }
    
    typename Double_Linked_List<T>::const_iterator end() const {
        return _list.end();
    }
    
    typename Double_Linked_List<T>::const_iterator cbegin() const {
        return _list.cbegin();
    }
    
    typename Double_Linked_List<T>::const_iterator cend() const {
        return _list.cend();
    }
    
    // ----- Métodos -----

     // Capacidad
    bool empty() const noexcept {
        return _list.empty();
    }

    uint32_t size() const noexcept {
        return _list.size();
    }

    // Asignacion y retorno
    void clear() noexcept {
        _list.clear();
    }

    void enqueue(const T& value) {
        _list.push_back(value);
    }
    
    void enqueue(T&& value) {
        _list.push_back(std::move(value));
    }    

    T dequeue() {
        if (empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return _list.pop_front();
    }

    T& front() {
        return _list.front();
    }
    
    const T& front() const {
        return _list.front();
    }

    T& back() {
        return _list.back();
    }
    
    const T& back() const {
        return _list.back();
    }

};