#pragma once

#include "data_structures/Double_Linked_List.hpp"
#include "data_structures/DynamicArray.hpp"
#include "data_structures/Pair.hpp"

template<typename Key, typename T>
class Unordered_map{
public:
    // ----- Aliases -----
    using key_type  = Key;
    using key_reference = Key&;
    using const_key_reference = const Key&;

    using mapped_type = T;
    using mapped_reference = T&;
    using const_mapped_reference = const T&;

    using value_type = Pair<const Key, T>;
    using value_Type_reference = Pair<const Key, T>&;
    using const_value_Type_reference = const Pair<const Key, T>&;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using bucket_type = Double_Linked_List<value_type>;
    using bucket_array = DynamicArray<bucket_type>;

    using reference       = value_type&;
    using const_reference = const value_type&;

    // ----- Iteradores -----
    class iterator;
    class const_iterator;

    // ----- Funciones especiales -----
    Unordered_map();
    Unordered_map(const Unordered_map& other) = delete;
    Unordered_map(Unordered_map&& other) noexcept;
    Unordered_map& operator=(const Unordered_map& other) = delete;
    Unordered_map& operator=(Unordered_map&& other) noexcept;
    ~Unordered_map() = default;

    Unordered_map(size_type bucket_count);
    Unordered_map(std::initializer_list<value_type> init);
    Unordered_map(std::span<value_type> s);

    // ----- Acceso de elementos -----
    mapped_reference operator[](const_key_reference key);
    mapped_reference at(const_key_reference key);
    const_mapped_reference at(const_key_reference key) const;

    // ----- Iteradores -----
    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    Pair<iterator, bool> insert(const_value_Type_reference value);
    Pair<iterator, bool> insert(value_type&& value);

    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    iterator find(const_key_reference key);
    const_iterator find(const_key_reference key) const;

    // ----- Capacidad -----
    bool empty() const noexcept;
    size_type size() const noexcept;

    // ----- Modificacion -----


private:
    // ----- Atributos -----
    static constexpr size_type DEFAULT_CAPACITY = 16;
    static constexpr float MAX_LOAD_FACTOR = 0.75f;
    
    bucket_array buckets_ = bucket_array();

    size_type size_ = 0;
    std::hash<Key> hasher_ = std::hash<Key>();




   
//     // ----- Iteradores -----
//     class iterator;
//     class const_iterator;

//     iterator begin() {
//         for(size_t i = 0; i<_bucket_count; ++i){
//             if (!_buckets[i].empty()){
//                 return iterator(this, i, _buckets[i].begin());
//             }
//         }
//         return end();
//     }

//     iterator end(){
//         return iterator(this, _bucket_count, _buckets[0].end());
//     }

//     const_iterator begin() const {
//         for(size_t i = 0; i<_bucket_count; ++i){
//             if (!_buckets[i].empty()){
//                 return const_iterator(this, i, _buckets[i].begin());
//             }
//         }
//         return end();
//     }

//     const_iterator end() const{
//         return const_iterator(this, _bucket_count, _buckets[0].end());
//     }
    
//     const_iterator cbegin() const {
//         for(size_t i = 0; i<_bucket_count; ++i){
//             if (!_buckets[i].empty()){
//                 return const_iterator(this, i, _buckets[i].begin());
//             }
//         }
//         return cend();
//     }

//     const_iterator cend() const{
//         return const_iterator(this, _bucket_count, _buckets[0].end());
//     }

//     // ----- Métodos -----

//     // Capacidad
//     bool empty() const noexcept {                                           // Booleano - Arreglo vacio
//         return _size == 0;
//     };

//     size_t size() const {  
//         return _size; 
//     }

//     size_t bucket_count() const { return _bucket_count; }
    
//     double max_load_factor() const { return _max_load_factor; }

//     // Asignacion y retorno
//     void clear() {
//         for (auto& bucket : _buckets) {
//             bucket.clear();
//         }
//         _size = 0;
//     }

//     void insert(const Key& key, const Value& value){
//         if(load_factor() >= _max_load_factor){
//             rehash(_bucket_count * 2);
//         }

//         size_t bucket_index = hash_function(key);
//         auto& bucket_list = _buckets[bucket_index];

//         for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it){
//             if(it->first() == key){
//                 it->second() = value;
//                 return;
//             }
//         }
//         bucket_list.push_back(Pair<Key,Value>(key,value));
//         _size++;
//     }

//     void insert(const Pair<Key, Value>& pair){ 
//         insert(pair.first(), pair.second());
//     }

//     void insert(Key&& key, Value&& value) {
//         if (load_factor() >= _max_load_factor) {
//             rehash(_bucket_count * 2);
//         }

//         size_t bucket_index = hash_function(key);
//         auto& bucket_list = _buckets[bucket_index];

//         for (auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
//             if (it->first() == key) {
//                 it->second() = std::move(value);
//                 return;
//             }
//         }
        
//         bucket_list.push_back(Pair<Key, Value>(std::move(key), std::move(value)));
//         _size++;
//     }

//     // ----- EMPLACE - Construye en el lugar -----
//     template<typename... Args>
//     Value& emplace(const Key& key, Args&&... args) {
//         if (load_factor() >= _max_load_factor) {
//             rehash(_bucket_count * 2);
//         }

//         size_t bucket_index = hash_function(key);
//         auto& bucket_list = _buckets[bucket_index];

//         // Buscar si ya existe
//         for (auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
//             if (it->first() == key) {
//                 // Ya existe - asignar nuevo valor
//                 it->second() = Value(std::forward<Args>(args)...);
//                 return it->second();
//             }
//         }

//         // No existe - crear nuevo con perfect forwarding
//         bucket_list.push_back(Pair<Key, Value>(key, Value(std::forward<Args>(args)...)));
//         _size++;
//         return bucket_list.back().second();
//     }

//     // Versión con key por movimiento
//     template<typename... Args>
//     Value& emplace(Key&& key, Args&&... args) {
//         if (load_factor() >= _max_load_factor) {
//             rehash(_bucket_count * 2);
//         }

//         size_t bucket_index = hash_function(key);
//         auto& bucket_list = _buckets[bucket_index];

//         // Buscar si ya existe
//         for (auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
//             if (it->first() == key) {
//                 // Ya existe - asignar nuevo valor
//                 it->second() = Value(std::forward<Args>(args)...);
//                 return it->second();
//             }
//         }

//         // No existe - crear nuevo con perfect forwarding
//         bucket_list.push_back(Pair<Key, Value>(std::move(key), Value(std::forward<Args>(args)...)));
//         _size++;
//         return bucket_list.back().second();
//     }

//     Value& find(const Key& key) {
//         size_t bucket_index = hash_function(key);
//         auto& bucket_list = _buckets[bucket_index];

//         for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
//             if(it->first() == key) {
//                 return it->second();  // Retorna referencia directa
//             }
//         }
//         throw std::out_of_range("Key not found in Unordered_map");
//     }

//     const Value& find(const Key& key) const {
//         size_t bucket_index = hash_function(key);
//         const auto& bucket_list = _buckets[bucket_index];

//         for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
//             if(it->first() == key) {
//                 return it->second();  // Retorna const referencia
//             }
//         }
//         throw std::out_of_range("Key not found in Unordered_map");
//     }
        
//     Value* find_ptr(const Key& key) {
//         size_t bucket_index = hash_function(key);
//         auto& bucket_list = _buckets[bucket_index];

//         for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
//             if(it->first() == key) {
//                 return &(it->second());
//             }
//         }
//         return nullptr;
//     }

//     const Value* find_ptr(const Key& key) const{
//         size_t bucket_index = hash_function(key);
//         const auto& bucket_list = _buckets[bucket_index];

//         for(auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
//             if(it->first() == key) {
//                 return &(it->second());
//             }
//         }
//         return nullptr;
//     }

//     void erase(const Key& key){
//         size_t bucket_index = hash_function(key);
//         auto& bucket_list = _buckets[bucket_index];
        
//         for (auto it = bucket_list.begin(); it != bucket_list.end(); ++it) {
//             if (it->first() == key) {
//                 bucket_list.erase(it);
//                 _size--;
//                 break;
//             }
//         }
        
//         if (_bucket_count > 16 && load_factor() < _max_load_factor / 4) {
//             size_t new_size = (std::max)(size_t(16), _size * 2);  // No reducir demasiado
//             rehash(new_size);
//         }
//     }

//     bool contains(const Key& key) const {
//         return find_ptr(key) != nullptr;
//     }

//     void reserve(size_t count) {
//         size_t new_size = (std::max)(count, static_cast<size_t>(_size / _max_load_factor));
//         rehash(new_size);
//     }

//     iterator erase(iterator pos) {
//         if (pos == end()) return end();
        
//         Key key_to_erase = pos->first();
//         iterator next = pos;
//         ++next;
        
//         erase(key_to_erase);
        
//         return next;
//     }

//     const_iterator erase(const_iterator pos) {
//         if (pos == end()) return end();

//         Key key_to_erase = pos->first();
//         const_iterator next = pos;
//         ++next;
        
//         erase(key_to_erase);
        
//         return next;
//     }

//     // ----- Iteradores - Clases -----

//     class iterator{
//     private:
        
//         Unordered_map* _map = nullptr;
//         size_t _bucket_index = 0;
//         typename Double_Linked_List<Pair<Key, Value>>::iterator _list_iterator; 

//     public:

//         iterator(Unordered_map* map = nullptr,
//                  size_t bucket_index = 0,
//                 typename Double_Linked_List<Pair<Key,Value>>::iterator list_iterator = {})
//         : _map(map), _bucket_index(bucket_index), _list_iterator(list_iterator){

//             if(_map && _bucket_index < _map->_bucket_count
//                && _list_iterator == _map->_buckets[_bucket_index].end()){
//                 find_next_non_empty();
//             }
//         }

//         Pair<Key, Value>& operator*(){
//             return *_list_iterator;
//         }

//         Pair<Key, Value>* operator->(){
//             return &(*_list_iterator);
//         }

//         iterator& operator++(){
//             if(_map && _bucket_index < _map->_bucket_count){
//                 ++_list_iterator;
//                 find_next_non_empty();
//             }
//             return *this;
//         }

//         iterator operator++(int){
//             iterator temp = *this;
//             ++(*this);
//             return temp;
//         }

//         bool operator==(const iterator& other) const {
//             return _map == other._map &&
//                    _bucket_index == other._bucket_index &&
//                    _list_iterator == other._list_iterator;
//         }

//         bool operator!=(const iterator& other) const {
//             return !(*this == other);
//         }

//     private:

//         void find_next_non_empty(){
//             while(_bucket_index < _map ->_bucket_count
//                 && _list_iterator == _map->_buckets[_bucket_index].end()){

//                 _bucket_index++;
//                 if(_bucket_index < _map->_bucket_count){
//                     _list_iterator = _map->_buckets[_bucket_index].begin();
//                 }
//             }

//         }


//     };

//     class const_iterator{
//     private:
        
//         const Unordered_map* _map = nullptr;
//         size_t _bucket_index = 0;
//         typename Double_Linked_List<Pair<Key, Value>>::const_iterator _list_iterator; 

//     public:

//         const_iterator(const Unordered_map* map = nullptr,
//                        size_t bucket_index = 0,
//                        typename Double_Linked_List<Pair<Key,Value>>::const_iterator list_iterator = {})
//         : _map(map), _bucket_index(bucket_index), _list_iterator(list_iterator){

//             if(_map && _bucket_index < _map->_bucket_count
//                && _list_iterator == _map->_buckets[_bucket_index].end()){
//                 find_next_non_empty();
//             }
//         }

//         const Pair<Key, Value>& operator*() const{
//             return *_list_iterator;
//         }

//         const Pair<Key, Value>* operator->() const{
//             return &(*_list_iterator);
//         }

//         const_iterator& operator++(){
//             if(_map && _bucket_index < _map->_bucket_count){
//                 ++_list_iterator;
//                 find_next_non_empty();
//             }
//             return *this;
//         }

//         const_iterator operator++(int){
//             const_iterator temp = *this;
//             ++(*this);
//             return temp;
//         }

//         bool operator==(const const_iterator& other) const {
//             return _map == other._map &&
//                    _bucket_index == other._bucket_index &&
//                    _list_iterator == other._list_iterator;
//         }

//         bool operator!=(const const_iterator& other) const {
//             return !(*this == other);
//         }

//     private:
        
//         void find_next_non_empty(){
//             while(_bucket_index < _map ->_bucket_count
//                 && _list_iterator == _map->_buckets[_bucket_index].end()){

//                 _bucket_index++;
//                 if(_bucket_index < _map->_bucket_count){
//                     _list_iterator = _map->_buckets[_bucket_index].begin();
//                 }
//             }

//         }
//     };


// private:
//     // ----- Calculo de hashes -----
//     size_t hash_function(const Key& key) const{
//         return _hasher(key) % _bucket_count;
//     }

//     double load_factor() const { 
//         return static_cast<double>(_size) / _bucket_count; 
//     }

//     void rehash(size_t new_bucket_count){
//         DynamicArray<Double_Linked_List<Pair<Key, Value>>> new_buckets(new_bucket_count);
        
//         for (auto& bucket : _buckets) {
//             while (!bucket.empty()) {
//                 auto node = bucket.pop_front(); // Si tu lista lo soporta
//                 size_t new_index = _hasher(node.first()) % new_bucket_count;
//                 new_buckets[new_index].push_back(std::move(node));
//             }
//         }

//         _buckets = std::move(new_buckets);
//         _bucket_count = new_bucket_count;

//     }
    
};

template<typename Key, typename T>
class Unordered_map<Key,T>::iterator {
public:
    // ----- Aliases -----
    using iterator_category = std:: ;


    using node_iterator = Double_Linked_List<Pair<Key, T>>::iterator;
    using const_unordered_map_pointer = const Unordered_map<Key,TDouble_Linked>*;

private:


};





// ----- Funciones especiales -----
template<typename Key, typename T>
Unordered_map<Key,T>::Unordered_map() :
buckets_(bucket_array()), size_(0), hasher_(std::hash<Key>()) {}

template<typename Key, typename T>
Unordered_map<Key,T>::Unordered_map(Unordered_map&& other) noexcept :
buckets_(std::move(other.buckets_)), size_(std::move(other.size_)), hasher_(std::move(other.hasher_)) {
    other.buckets_ = bucket_array();
    other.size_ = 0;
    other.hasher_ = std::hash<Key>();
}

template<typename Key, typename T>
Unordered_map<Key,T>& Unordered_map<Key,T>::operator=(Unordered_map&& other) noexcept {
    if(this != &other){
        size_ = 0;

        buckets_ = std::move(other.buckets_);
        size_ = std::move(other.size_);
        hasher_ = std::move(other.hasher_);

        other.buckets_ = bucket_array();
        other.size_ = 0;
        other.hasher_ = std::hash<Key>();
    }
    return *this;
}

template<typename Key, typename T>
Unordered_map<Key,T>::Unordered_map(size_type bucket_count) :
buckets_(bucket_count, bucket_type()){}

template<typename Key, typename T>
Unordered_map<Key,T>::Unordered_map(std::initializer_list<value_type> init) : 
size_(0), hasher_(std::hash<Key>()) {
    if (init.size() == 0) {
        buckets_ = bucket_array();
        return;
    }

    size_type capacity = (init.size() >= DEFAULT_CAPACITY*MAX_LOAD_FACTOR/2) ? static_cast<size_type>(init.size() * 2 / MAX_LOAD_FACTOR) :
                                                                               DEFAULT_CAPACITY;

    buckets_ = bucket_array(capacity, bucket_type());
    size_type bucket_index = 0;
    typename bucket_type::iterator bucket_node;

    for(auto it = init.begin(); it != init.end(); ++it) {
        bucket_index = hash_function(it->First());

        bucket_node = buckets_[bucket_index].find(it->First());
        if(bucket_node != buckets_[bucket_index].end()) bucket_node->Set_Second(it->Second());
        else {
            buckets_[bucket_index].push_back(it->Second());
            ++size_;
        }
    }
}

template<typename Key, typename T>
Unordered_map<Key,T>::Unordered_map(std::span<value_type> s) : 
size_(0), hasher_(std::hash<Key>()) {
    if (s.empty()) {
        buckets_ = bucket_array();
        return;
    }

    size_type capacity = (s.size() >= DEFAULT_CAPACITY*MAX_LOAD_FACTOR/2) ? static_cast<size_type>(s.size() * 2 / MAX_LOAD_FACTOR) :
                                                                            DEFAULT_CAPACITY;

    buckets_ = bucket_array(capacity, bucket_type());
    size_type bucket_index = 0;
    typename bucket_type::iterator bucket_node;

    for(auto it = s.begin(); it != s.end(); ++it) {
        bucket_index = hash_function(it->First());

        bucket_node = buckets_[bucket_index].find(it->First());
        if(bucket_node != buckets_[bucket_index].end()) bucket_node->Set_Second(it->Second());
        else {
            buckets_[bucket_index].push_back(it->Second());
            ++size_;
        }
    }
}

// ----- Acceso de elementos -----
template<typename Key, typename T>
Unordered_map<Key,T>::mapped_reference Unordered_map<Key,T>::operator[](const_key_reference key) {
    size_type bucket_index = hash_function(key);
    typename bucket_type::iterator bucket_node = buckets_[bucket_index].find(key);

    if(bucket_node != buckets_[bucket_index].end()) return bucket_node->Second();
    else {
        ++size_;
        return buckets_[bucket_index].emplace_back(mapped_type());
    }
}

template<typename Key, typename T>
Unordered_map<Key,T>::mapped_reference Unordered_map<Key,T>::at(const_key_reference key) {
    size_type bucket_index = hash_function(key);
    typename bucket_type::iterator bucket_node = buckets_[bucket_index].find(key);

    if(bucket_node != buckets_[bucket_index].end()) return bucket_node->Second();
    else throw std::out_of_range("Unordered_map::at: key not found");
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_mapped_reference Unordered_map<Key,T>::at(const_key_reference key) const {
    size_type bucket_index = hash_function(key);
    typename bucket_type::iterator bucket_node = buckets_[bucket_index].find(key);

    if(bucket_node != buckets_[bucket_index].end()) return bucket_node->Second();
    else throw std::out_of_range("Unordered_map::at: key not found");
}


