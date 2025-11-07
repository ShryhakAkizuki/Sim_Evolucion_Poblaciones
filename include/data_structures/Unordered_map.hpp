#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <functional>                           // Para std::hash
#include <algorithm>

#include "data_structures/Double_Link_List.hpp"
#include "data_structures/DynamicArray.hpp"
#include "data_structures/Pair.hpp"

#include <iostream>

template<typename Key, typename Value>
class Unordered_map{
private:
    // ----- Atributos -----
    size_t _size = 0;
    size_t _bucket_count = 16;
    double _max_load_factor = 0.75;

    std::hash<Key> _hasher;

    DynamicArray<Double_Linked_List<Pair<Key, Value>>> _buckets;

public:
    // ----- Constructores -----
    Unordered_map() : _bucket_count(16), 
    _buckets(DynamicArray<Double_Linked_List<Pair<Key, Value>>>(_bucket_count)) {}

    explicit Unordered_map(size_t bucket_count) :
    _bucket_count(bucket_count),
    _buckets(DynamicArray<Double_Linked_List<Pair<Key, Value>>>(_bucket_count)) {}
    
    Unordered_map(size_t bucket_count, size_t max_load_factor) :
    _bucket_count(bucket_count), _max_load_factor(max_load_factor)
    ,_buckets(DynamicArray<Double_Linked_List<Pair<Key, Value>>>(_bucket_count)) {}

    Unordered_map(const Unordered_map& other) 
    : _size(other._size),
    _bucket_count(other._bucket_count),
    _max_load_factor(other._max_load_factor) {
        
        _buckets = DynamicArray<Double_Linked_List<Pair<Key, Value>>>(_bucket_count);
        for (size_t i = 0; i < _bucket_count; ++i) {
            _buckets[i] = other._buckets[i];  
        }
    }

    Unordered_map(Unordered_map&& other) noexcept
    : _buckets(std::move(other._buckets)),
    _size(other._size),
    _bucket_count(other._bucket_count),
    _max_load_factor(other._max_load_factor) {
        
        other._size = 0;
        other._bucket_count = 16;  
    }
    
    // ----- Destructor -----
    
    ~Unordered_map() = default;

    // ----- Operadores -----

    Unordered_map& operator=(const Unordered_map& other){
        if(this != &other){
            _size = other._size;
            _bucket_count = other._bucket_count;
            _max_load_factor = other._max_load_factor;

            _buckets = DynamicArray<Double_Linked_List<Pair<Key, Value>>>(_bucket_count);
            for(size_t i = 0; i < _bucket_count; ++i){
                _buckets[i] = other._buckets[i];
            }
        }
        return *this;
    }

    Unordered_map& operator=(Unordered_map&& other) noexcept {
        if(this != &other){
            _size = other._size;
            _bucket_count = other._bucket_count;
            _max_load_factor = other._max_load_factor;
            _buckets = std::move(other._buckets);
            
            other._size = 0;
            other._bucket_count = 16;  
            _buckets = DynamicArray<Double_Linked_List<Pair<Key, Value>>>(other._bucket_count);        
        }
        return *this;
    }

    Value& operator[](const Key& key){
        size_t bucket_index = hash_function(key);
        auto& bucket_list = _buckets[bucket_index];

        for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it){
            if(it->first() == key){
                return it->second();
            }
        }

        bucket_list.push_back(Pair<Key, Value>(key, Value{}));
        _size++;
    
        return bucket_list.back().second();    
    }

    const Value& at(const Key& key) const {
        if (const Value* found = find(key)) {
            return *found;
        }
        throw std::out_of_range("Key not found");
    }

    Value& at(const Key& key) {
        if (Value* found = find(key)) {
            return *found;
        }
        throw std::out_of_range("Key not found");
    }
   
    // ----- Iteradores -----
    class iterator;
    class const_iterator;

    iterator begin() {
        for(size_t i = 0; i<_bucket_count; ++i){
            if (!_buckets[i].empty()){
                return iterator(this, i, _buckets[i].begin());
            }
        }
        return end();
    }

    iterator end(){
        return iterator(this, _bucket_count, _buckets[0].end());
    }

    const_iterator begin() const {
        for(size_t i = 0; i<_bucket_count; ++i){
            if (!_buckets[i].empty()){
                return const_iterator(this, i, _buckets[i].begin());
            }
        }
        return end();
    }

    const_iterator end() const{
        return const_iterator(this, _bucket_count, _buckets[0].end());
    }
    
    const_iterator cbegin() const {
        for(size_t i = 0; i<_bucket_count; ++i){
            if (!_buckets[i].empty()){
                return const_iterator(this, i, _buckets[i].begin());
            }
        }
        return cend();
    }

    const_iterator cend() const{
        return const_iterator(this, _bucket_count, _buckets[0].end());
    }

    // ----- Métodos -----

    // Capacidad
    bool empty() const noexcept {                                           // Booleano - Arreglo vacio
        return _size == 0;
    };

    size_t size() const {  
        return _size; 
    }

    size_t bucket_count() const { return _bucket_count; }
    
    double max_load_factor() const { return _max_load_factor; }

    // Asignacion y retorno
    void clear() {
        for (auto& bucket : _buckets) {
            bucket.clear();
        }
        _size = 0;
    }

    void insert(const Key& key, const Value& value){
        if(load_factor() >= _max_load_factor){
            rehash(_bucket_count * 2);
        }

        size_t bucket_index = hash_function(key);
        auto& bucket_list = _buckets[bucket_index];

        for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it){
            if(it->first() == key){
                it->second() = value;
                return;
            }
        }
        bucket_list.push_back(Pair<Key,Value>(key,value));
        _size++;
    }

    void insert(const Pair<Key, Value>& pair){ 
        insert(pair.first(), pair.second());
    }

    Value& find(const Key& key) {
        size_t bucket_index = hash_function(key);
        auto& bucket_list = _buckets[bucket_index];

        for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
            if(it->first() == key) {
                return it->second();  // Retorna referencia directa
            }
        }
        throw std::out_of_range("Key not found in Unordered_map");
    }

    const Value& find(const Key& key) const {
        size_t bucket_index = hash_function(key);
        const auto& bucket_list = _buckets[bucket_index];

        for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
            if(it->first() == key) {
                return it->second();  // Retorna const referencia
            }
        }
        throw std::out_of_range("Key not found in Unordered_map");
    }
        
    Value* find_ptr(const Key& key) {
        size_t bucket_index = hash_function(key);
        auto& bucket_list = _buckets[bucket_index];

        for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
            if(it->first() == key) {
                return &(it->second());
            }
        }
        return nullptr;
    }

    const Value* find_ptr(const Key& key) const{
        size_t bucket_index = hash_function(key);
        const auto& bucket_list = _buckets[bucket_index];

        for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
            if(it->first() == key) {
                return &(it->second());
            }
        }
        return nullptr;
    }

    void erase(const Key& key){
        size_t bucket_index = hash_function(key);
        auto& bucket_list = _buckets[bucket_index];
        
        for (auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
            if (it->first() == key) {
                bucket_list.erase(it);
                _size--;
                break;
            }
        }
        
        if (_bucket_count > 16 && load_factor() < _max_load_factor / 4) {
            size_t new_size = (std::max)(size_t(16), _size * 2);  // No reducir demasiado
            rehash(new_size);
        }
    }

    bool contains(const Key& key) const {
        return find_otr(key) != nullptr;
    }

    void reserve(size_t count) {
        size_t new_size = (std::max)(count, static_cast<size_t>(_size / _max_load_factor));
        rehash(new_size);
    }

    iterator erase(iterator pos) {
        if (pos == end()) return end();
        
        Key key_to_erase = pos->first();
        iterator next = pos;
        ++next;
        
        erase(key_to_erase);
        
        return next;
    }

    const_iterator erase(const_iterator pos) {
        if (pos == end()) return end();

        Key key_to_erase = pos->first();
        const_iterator next = pos;
        ++next;
        
        erase(key_to_erase);
        
        return next;
    }

    // ----- Iteradores - Clases -----

    class iterator{
    private:
        
        Unordered_map* _map = nullptr;
        size_t _bucket_index = 0;
        typename Double_Linked_List<Pair<Key, Value>>::iterator _list_iterator; 

    public:

        iterator(Unordered_map* map = nullptr,
                 size_t bucket_index = 0,
                typename Double_Linked_List<Pair<Key,Value>>::iterator list_iterator = {})
        : _map(map), _bucket_index(bucket_index), _list_iterator(list_iterator){

            if(_map && _bucket_index < _map->_bucket_count
               && _list_iterator == _map->_buckets[_bucket_index].end()){
                find_next_non_empty();
            }
        }

        Pair<Key, Value>& operator*(){
            return *_list_iterator;
        }

        Pair<Key, Value>* operator->(){
            return &(*_list_iterator);
        }

        iterator& operator++(){
            if(_map && _bucket_index < _map->_bucket_count){
                ++_list_iterator;
                find_next_non_empty();
            }
            return *this;
        }

        iterator operator++(int){
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const iterator& other) const {
            return _map == other._map &&
                   _bucket_index == other._bucket_index &&
                   _list_iterator == other._list_iterator;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

    private:

        void find_next_non_empty(){
            while(_bucket_index < _map ->_bucket_count
                && _list_iterator == _map->_buckets[_bucket_index].end()){

                _bucket_index++;
                if(_bucket_index < _map->_bucket_count){
                    _list_iterator = _map->_buckets[_bucket_index].begin();
                }
            }

        }


    };

    class const_iterator{
    private:
        
        const Unordered_map* _map = nullptr;
        size_t _bucket_index = 0;
        typename Double_Linked_List<Pair<Key, Value>>::const_iterator _list_iterator; 

    public:

        const_iterator(const Unordered_map* map = nullptr,
                       size_t bucket_index = 0,
                       typename Double_Linked_List<Pair<Key,Value>>::const_iterator list_iterator = {})
        : _map(map), _bucket_index(bucket_index), _list_iterator(list_iterator){

            if(_map && _bucket_index < _map->_bucket_count
               && _list_iterator == _map->_buckets[_bucket_index].end()){
                find_next_non_empty();
            }
        }

        const Pair<Key, Value>& operator*() const{
            return *_list_iterator;
        }

        const Pair<Key, Value>* operator->() const{
            return &(*_list_iterator);
        }

        const_iterator& operator++(){
            if(_map && _bucket_index < _map->_bucket_count){
                ++_list_iterator;
                find_next_non_empty();
            }
            return *this;
        }

        const_iterator operator++(int){
            const_iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const const_iterator& other) const {
            return _map == other._map &&
                   _bucket_index == other._bucket_index &&
                   _list_iterator == other._list_iterator;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

    private:
        
        void find_next_non_empty(){
            while(_bucket_index < _map ->_bucket_count
                && _list_iterator == _map->_buckets[_bucket_index].end()){

                _bucket_index++;
                if(_bucket_index < _map->_bucket_count){
                    _list_iterator = _map->_buckets[_bucket_index].begin();
                }
            }

        }
    };


private:
    // ----- Calculo de hashes -----
    size_t hash_function(const Key& key) const{
        return _hasher(key) % _bucket_count;
    }

    double load_factor() const { 
        return static_cast<double>(_size) / _bucket_count; 
    }

    void rehash(size_t new_bucket_count){
        DynamicArray<Double_Linked_List<Pair<Key, Value>>> new_buckets(new_bucket_count);
        
        for (auto& bucket : _buckets) {
            while (!bucket.empty()) {
                auto node = bucket.pop_front(); // Si tu lista lo soporta
                size_t new_index = _hasher(node.first()) % new_bucket_count;
                new_buckets[new_index].push_back(std::move(node));
            }
        }

        _buckets = std::move(new_buckets);
        _bucket_count = new_bucket_count;

    }
    
};