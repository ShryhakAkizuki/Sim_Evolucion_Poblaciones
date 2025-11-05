#pragma once
#include <memory>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include "data_structures/Double_Link_List.hpp"
#include "data_structures/DynamicArray.hpp"
#include "data_structures/Pair.hpp"
#include <functional>                           // Para std::hash

template<typename Key, typename Value>
class Unordered_map{
private:
    // ----- Atributos -----
    DynamicArray<Double_Linked_List<Pair<Key, Value>>> _buckets;
    size_t _size = 0;
    size_t _bucket_count = 16;
    double _max_load_factor = 0.75;

    std::hash<Key> _hasher;

public:
    // ----- Constructores -----
    Unordered_map(){
        _buckets.resize(_bucket_count);
    }

    explicit Unordered_map(size_t bucket_count) :
    _bucket_count(bucket_count){
        _buckets.resize(_bucket_count);
    }

    Unordered_map(size_t bucket_count, size_t max_load_factor) :
    _bucket_count(bucket_count), _max_load_factor(max_load_factor){
        _buckets.resize(_bucket_count);
    }

    Unordered_map(const Unordered_map& other) 
    : _size(other._size),
    _bucket_count(other._bucket_count),
    _max_load_factor(other._max_load_factor) {
        
        _buckets.resize(_bucket_count);
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
        other._buckets.resize(other._bucket_count); 
    }

    // ----- Operadores -----

    Unordered_map& operator=(const Unordered_map& other){
        if(this != &other){
            _size = other._size;
            _bucket_count = other._bucket_count;
            _max_load_factor = other._max_load_factor;

            _buckets.resize(_bucket_count);
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
            other._buckets.resize(other._bucket_count); 
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


    // ----- Métodos -----

    // Capacidad
    bool empty() const noexcept {                                           // Booleano - Arreglo vacio
        return _size == 0;
    };

    size_t size() const {  
        return _size; 
    }

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

    Value* find(const Key& key){
        size_t bucket_index = hash_function(key);
        auto& bucket_list = _buckets[bucket_index];

        for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it){
            if(it->first() == key){
                return &(it->second());
            }
        }
        return nullptr;
    }

    const Value* find(const Key& key) const {
        size_t bucket_index = hash_function(key);
        const auto& bucket_list = _buckets[bucket_index];  // const&

        for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it){
            if(it->first() == key){
                return &(it->second());  // Retorna const Value*
            }
        }
        return nullptr;
    }

    void erase(const Key& key){
        size_t bucket_index = hash_function(key);
        auto& bucket_list = _buckets[bucket_index];
        
        // Buscar el elemento y su índice
        uint32_t index = 0;
        for (auto it = bucket_list.begin(); it != bucket_list.end(); ++it, ++index) {
            if (it->first() == key) {
                bucket_list.Delete_Node(index);
                _size--;
                break;
            }
        }
        
        if (_bucket_count > 16 && load_factor() < _max_load_factor / 4) {
            size_t new_size = std::max(size_t(16), _size * 2);  // No reducir demasiado
            rehash(new_size);
        }
    }

    bool contains(const Key& key) const {
        return find(key) != nullptr;
    }

    // ----- Destructor -----
    ~Unordered_map() = default;

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
            for (auto it = bucket.begin(); it != bucket.end(); ) {
                auto current = it++;
                size_t new_index = _hasher(current->first()) % new_bucket_count;
                new_buckets[new_index].push_back(std::move(*current));
            }
        }

        _buckets = std::move(new_buckets);
        _bucket_count = new_bucket_count;

    }
    
    // ----- Iteradores -----

    class iterator{
    private:

    public:

    }

    
    class const_iterator{
    private:

    public:

    }

};