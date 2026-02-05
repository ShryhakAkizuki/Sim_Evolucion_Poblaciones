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

    using value_type = Pair<Key, T>;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using bucket_type = Double_Linked_List<value_type>;
    using bucket_array = DynamicArray<bucket_type>;

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

    Pair<iterator, bool> insert(const_reference value);
    Pair<iterator, bool> insert(value_type&& value);

    template<class... Args>
    Pair<iterator, bool> emplace(Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    iterator find(const_key_reference key);
    const_iterator find(const_key_reference key) const;

    // ----- Capacidad -----
    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type bucket_count() const noexcept;
    size_type bucket_size(size_type bucket_index) const;     

    // ----- Modificacion -----
    void clear();

    void erase(const_key_reference key);

    // ----- Comparadores -----
    bool operator==(const Unordered_map& other) const;
    bool operator!=(const Unordered_map& other) const;

    // ----- Helpers -----
    void swap(Unordered_map& other) noexcept;

private:
    // ----- Atributos -----
    static constexpr size_type DEFAULT_CAPACITY = 16;
    static constexpr float MAX_LOAD_FACTOR = 0.75f;
    
    bucket_array buckets_ = bucket_array();

    size_type size_ = 0;
    std::hash<Key> hasher_ = std::hash<Key>();

    // ----- Helpers -----
    float load_factor() const;
    void rehash(size_type new_bucket_count);
};

template<typename Key, typename T>
class Unordered_map<Key,T>::iterator {
public:
    // ----- Aliases -----
    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = Pair<Key, T>;
    using difference_type = std::ptrdiff_t;

    using reference = value_type&;
    using pointer = value_type*;

    using node_iterator = typename bucket_type::iterator;
    using const_unordered_map_pointer = const Unordered_map<Key,T>*;

    friend class const_iterator;
    friend class Unordered_map;

    // ----- Funciones especiales -----
    iterator();
    explicit iterator(size_type bucket_idx, node_iterator node_it, const_unordered_map_pointer map);
    iterator(const iterator& other);
    iterator& operator=(const iterator& other);

    // ----- Operadores especiales -----
    reference operator*() const;
    pointer operator->() const;       
    iterator& operator++();           
    iterator operator++(int);         
    iterator& operator--();          
    iterator operator--(int);     

    // ----- Comparadores -----
    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& other) const;


private:
    // ----- Atributos -----
    size_type current_bucket_ = 0;
    node_iterator node_it_ = node_iterator(nullptr, nullptr);
    const_unordered_map_pointer map_ = nullptr;

    // ----- Funciones especiales -----
    explicit iterator(const const_iterator& other);
    iterator& operator=(const const_iterator& other);

    // ----- Acceso de elementos -----
    void find_valid_position(bool forward = true);
    void advance_forward();
    void retreat_backward();

};

template<typename Key, typename T>
class Unordered_map<Key,T>::const_iterator {
public:
    // ----- Aliases -----
    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = Pair<Key, T>;
    using difference_type = std::ptrdiff_t;

    using const_reference = const value_type&;
    using const_pointer = const value_type*;

    using const_node_iterator = typename bucket_type::const_iterator;
    using const_unordered_map_pointer = const Unordered_map<Key,T>*;

    friend class iterator;
    friend class Unordered_map;
    
    // ----- Funciones especiales -----
    const_iterator();
    explicit const_iterator(size_type bucket_idx, const_node_iterator node_it, const_unordered_map_pointer map);
    explicit const_iterator(const iterator& it);
    const_iterator(const const_iterator& other);
    const_iterator& operator=(const const_iterator& other);
    const_iterator& operator=(const iterator& other);

    // ----- Operadores especiales -----
    const_reference operator*() const;
    const_pointer operator->() const;       
    const_iterator& operator++();           
    const_iterator operator++(int);         
    const_iterator& operator--();          
    const_iterator operator--(int);   

    // ----- Comparadores -----
    bool operator==(const const_iterator& other) const;
    bool operator!=(const const_iterator& other) const;

private:
    // ----- Atributos -----
    size_type current_bucket_ = 0;
    const_node_iterator node_it_ = const_node_iterator(nullptr, nullptr);
    const_unordered_map_pointer map_ = nullptr;

    // ----- Acceso de elementos -----
    void find_valid_position(bool forward = true);
    void advance_forward();
    void retreat_backward();
};

// #################### iterator ###################
// ----- Funciones especiales -----
template<typename Key, typename T>
Unordered_map<Key,T>::iterator::iterator() :
current_bucket_(0), node_it_(nullptr, nullptr), map_(nullptr) {}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator::iterator(size_type bucket_idx, node_iterator node_it, const_unordered_map_pointer map) :
current_bucket_(bucket_idx), node_it_(node_it), map_(map) {
    if (map_ != nullptr && node_it_ != node_iterator(nullptr, nullptr)) {
        find_valid_position();
    }
}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator::iterator(const iterator& other) :
current_bucket_(other.current_bucket_), node_it_(other.node_it_), map_(other.map_) {}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator::iterator(const const_iterator& other) :
current_bucket_(other.current_bucket_), node_it_(other.node_it_), map_(other.map_) {}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator& Unordered_map<Key,T>::iterator::operator=(const iterator& other) {
    current_bucket_ = other.current_bucket_;
    node_it_ = other.node_it_;
    map_ = other.node_it_;
    return *this;
}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator& Unordered_map<Key,T>::iterator::operator=(const const_iterator& other) {
    current_bucket_ = other.current_bucket_;
    node_it_ = other.node_it_;
    map_ = other.node_it_;
    return *this;
}

// ----- Acceso de elementos -----
template<typename Key, typename T>
void Unordered_map<Key,T>::iterator::find_valid_position(bool forward) {
    if (map_ == nullptr || map_->bucket_count() == 0) {
        current_bucket_ = 0;
        node_it_ = node_iterator(nullptr,nullptr);
        return;
    }

    if(current_bucket_ < map_->bucket_count() &&
       node_it_ != node_iterator(nullptr,nullptr) &&
       node_it_ != map_->buckets_[current_bucket_].end()) return;

    if (forward) advance_forward();
    else retreat_backward();
}

template<typename Key, typename T>
void Unordered_map<Key,T>::iterator::advance_forward() {
    if (map_ == nullptr || map_->bucket_count() == 0) {
        node_it_ = node_iterator(nullptr, nullptr);
        return;
    }
    
    if (current_bucket_ < map_->bucket_count() && 
        node_it_ != node_iterator(nullptr, nullptr)) {

        ++node_it_;
        if (node_it_ != map_->buckets_[current_bucket_].end()) return; 
    }
    
    for (size_type i = current_bucket_ + 1; i < map_->bucket_count(); ++i) {
        if (!map_->buckets_[i].empty()) {
            current_bucket_ = i;
            node_it_ = map_->buckets_[i].begin();
            return;
        }
    }

    if (map_->bucket_count() > 0) {
        current_bucket_ = map_->bucket_count() - 1;
        node_it_ = map_->buckets_[current_bucket_].end();
    } else {
        current_bucket_ = 0;
        node_it_ = node_iterator(nullptr, nullptr);
    }
}

template<typename Key, typename T>
void Unordered_map<Key,T>::iterator::retreat_backward() {
    if (map_ == nullptr || map_->bucket_count() == 0) {
        node_it_ = node_iterator(nullptr, nullptr);
        return;
    }

    if (current_bucket_ >= map_->bucket_count() || 
        node_it_ == node_iterator(nullptr, nullptr) ||
        node_it_ == map_->buckets_[current_bucket_].end()) {
        
        for (size_type i = map_->bucket_count()-1; i >= 0; --i) {
            if (!map_->buckets_[i].empty()) {
                current_bucket_ = i;
                node_it_ = --map_->buckets_[i].end();
                return;
            }
        }
        
        if (map_->bucket_count() > 0) {
            current_bucket_ = 0;
            node_it_ = map_->buckets_[0].begin();
        }
        return;
    }

    if (node_it_ != map_->buckets_[current_bucket_].begin()) {
        --node_it_;
        return;
    }

    for (size_type i = current_bucket_ - 1; i >= 0; --i) {
        if (!map_->buckets_[i].empty()) {
            current_bucket_ = i;
            node_it_ = --map_->buckets_[i].end();
            return;
        }
    }

    current_bucket_ = 0;
    node_it_ = map_->buckets_[0].begin();
}

// ----- Operadores especiales -----
template<typename Key, typename T>
Unordered_map<Key,T>::iterator::reference Unordered_map<Key,T>::iterator::operator*() const {
    if(map_ == nullptr || node_it_ == node_iterator(nullptr, nullptr)) throw std::runtime_error("Unordered_map::iterator::operator*: Dereferencing end iterator");
    return *node_it_;
}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator::pointer Unordered_map<Key,T>::iterator::operator->() const {
    if(map_ == nullptr || node_it_ == node_iterator(nullptr, nullptr)) throw std::runtime_error("Unordered_map::iterator::operator->: Dereferencing end iterator");
    return &(*node_it_);
}       

template<typename Key, typename T>
Unordered_map<Key,T>::iterator& Unordered_map<Key,T>::iterator::operator++() {
    if (map_ != nullptr) advance_forward();
    return *this;
}      

template<typename Key, typename T>
Unordered_map<Key,T>::iterator Unordered_map<Key,T>::iterator::operator++(int) {
    iterator temp = *this;
    ++(*this);
    return temp;
}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator& Unordered_map<Key,T>::iterator::operator--() {
    if (map_ != nullptr) retreat_backward();
    return *this;
}    

template<typename Key, typename T>
Unordered_map<Key,T>::iterator Unordered_map<Key,T>::iterator::operator--(int) {
    iterator temp = *this;
    --(*this);
    return temp;
} 

// ----- Comparadores -----
template<typename Key, typename T>
bool Unordered_map<Key,T>::iterator::operator==(const iterator& other) const {
    if(map_ == nullptr || other.map_ == nullptr) return map_ == other.map_;

    return (map_ == other.map_) && 
            (current_bucket_ == other.current_bucket_) && 
            (node_it_ == other.node_it_);
}

template<typename Key, typename T>
bool Unordered_map<Key,T>::iterator::operator!=(const iterator& other) const {
    return !(*this == other);
}

// #################### const_iterator ###################
// ----- Funciones especiales -----
template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator::const_iterator() : 
current_bucket_(0), node_it_(nullptr,nullptr), map_(nullptr) {}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator::const_iterator(size_type bucket_idx, const_node_iterator node_it, const_unordered_map_pointer map) : 
current_bucket_(bucket_idx), node_it_(node_it), map_(map) {
    if (map_ != nullptr && node_it_ != const_node_iterator(nullptr, nullptr)) {
        find_valid_position();
    }
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator::const_iterator(const iterator& it) : 
current_bucket_(it.current_bucket_), node_it_(it.node_it_), map_(it.map_) {}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator::const_iterator(const const_iterator& other) :
current_bucket_(other.current_bucket_), node_it_(other.node_it_), map_(other.map_) {}


template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator& Unordered_map<Key,T>::const_iterator::operator=(const const_iterator& other) {
    current_bucket_ = other.current_bucket_;
    node_it_ = other.node_it_;
    map_ = other.node_it_;
    return *this;
}
    
template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator& Unordered_map<Key,T>::const_iterator::operator=(const iterator& other) {
    current_bucket_ = other.current_bucket_;
    node_it_ = other.node_it_;
    map_ = other.node_it_;
    return *this;
}

// ----- Acceso de elementos -----
template<typename Key, typename T>
void Unordered_map<Key,T>::const_iterator::find_valid_position(bool forward) {
    if (map_ == nullptr || map_->bucket_count() == 0) {
        current_bucket_ = 0;
        node_it_ = const_node_iterator(nullptr,nullptr);
        return;
    }

    if(current_bucket_ < map_->bucket_count() &&
       node_it_ != const_node_iterator(nullptr,nullptr) &&
       node_it_ != map_->buckets_[current_bucket_].end()) return;

    if (forward) advance_forward();
    else retreat_backward();
}

template<typename Key, typename T>
void Unordered_map<Key,T>::const_iterator::advance_forward() {
    if (map_ == nullptr || map_->bucket_count() == 0) {
        node_it_ = const_node_iterator(nullptr, nullptr);
        return;
    }
    
    if (current_bucket_ < map_->bucket_count() && 
        node_it_ != const_node_iterator(nullptr, nullptr)) {

        ++node_it_;
        if (node_it_ != map_->buckets_[current_bucket_].cend()) return; 
    }
    
    for (size_type i = current_bucket_ + 1; i < map_->bucket_count(); ++i) {
        if (!map_->buckets_[i].empty()) {
            current_bucket_ = i;
            node_it_ = map_->buckets_[i].cbegin();
            return;
        }
    }

    if (map_->bucket_count() > 0) {
        current_bucket_ = map_->bucket_count() - 1;
        node_it_ = map_->buckets_[current_bucket_].cend();
    } else {
        current_bucket_ = 0;
        node_it_ = const_node_iterator(nullptr, nullptr);
    }
}

template<typename Key, typename T>
void Unordered_map<Key,T>::const_iterator::retreat_backward() {
    if (map_ == nullptr || map_->bucket_count() == 0) {
        node_it_ = const_node_iterator(nullptr, nullptr);
        return;
    }

    if (current_bucket_ >= map_->bucket_count() || 
        node_it_ == const_node_iterator(nullptr, nullptr) ||
        node_it_ == map_->buckets_[current_bucket_].cend()) {
        
        for (size_type i = map_->bucket_count()-1; i >= 0; --i) {
            if (!map_->buckets_[i].empty()) {
                current_bucket_ = i;
                node_it_ = --map_->buckets_[i].cend();
                return;
            }
        }
        
        if (map_->bucket_count() > 0) {
            current_bucket_ = 0;
            node_it_ = map_->buckets_[0].cbegin();
        }
        return;
    }

    if (node_it_ != map_->buckets_[current_bucket_].cbegin()) {
        --node_it_;
        return;
    }

    for (size_type i = current_bucket_ - 1; i >= 0; --i) {
        if (!map_->buckets_[i].empty()) {
            current_bucket_ = i;
            node_it_ = --map_->buckets_[i].cend();
            return;
        }
    }

    current_bucket_ = 0;
    node_it_ = map_->buckets_[0].cbegin();
}

// ----- Operadores especiales -----
template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator::const_reference Unordered_map<Key,T>::const_iterator::operator*() const {
    if(map_ == nullptr || node_it_ == const_node_iterator(nullptr, nullptr)) throw std::runtime_error("Unordered_map::const_iterator::operator*: Dereferencing end iterator");
    return *node_it_;
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator::const_pointer Unordered_map<Key,T>::const_iterator::operator->() const {
    if(map_ == nullptr || node_it_ == const_node_iterator(nullptr, nullptr)) throw std::runtime_error("Unordered_map::const_iterator::operator->: Dereferencing end iterator");
    return &(*node_it_);
}  

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator& Unordered_map<Key,T>::const_iterator::operator++() {
    if (map_ != nullptr) advance_forward();
    return *this;
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator Unordered_map<Key,T>::const_iterator::operator++(int) {
    const_iterator temp = *this;
    ++(*this);
    return temp;
}         

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator& Unordered_map<Key,T>::const_iterator::operator--() {
    if (map_ != nullptr) retreat_backward();
    return *this;
}        

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator Unordered_map<Key,T>::const_iterator::operator--(int) {
    const_iterator temp = *this;
    --(*this);
    return temp;
}

// ----- Comparadores -----
template<typename Key, typename T>
bool Unordered_map<Key,T>::const_iterator::operator==(const const_iterator& other) const {
    if(map_ == nullptr || other.map_ == nullptr) return map_ == other.map_;

    return (map_ == other.map_) && 
            (current_bucket_ == other.current_bucket_) && 
            (node_it_ == other.node_it_);
}

template<typename Key, typename T>
bool Unordered_map<Key,T>::const_iterator::operator!=(const const_iterator& other) const {
    return !(*this == other);
}

// #################### Unordered_map ###################
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
buckets_(bucket_count, bucket_type()), size_(0), hasher_(std::hash<Key>()) {}

template<typename Key, typename T>
Unordered_map<Key,T>::Unordered_map(std::initializer_list<value_type> init) : 
size_(0), hasher_(std::hash<Key>()) {
    if (init.size() == 0) return;
    
    size_type capacity = (init.size() >= DEFAULT_CAPACITY*MAX_LOAD_FACTOR/2) ? static_cast<size_type>(init.size() * 2 / MAX_LOAD_FACTOR) :
                                                                               DEFAULT_CAPACITY;

    buckets_ = bucket_array(capacity, bucket_type());
    for (const auto& pair : init) insert(pair);
}

template<typename Key, typename T>
Unordered_map<Key,T>::Unordered_map(std::span<value_type> s) : 
size_(0), hasher_(std::hash<Key>()) {
    if (s.empty()) return;

    size_type capacity = (s.size() >= DEFAULT_CAPACITY*MAX_LOAD_FACTOR/2) ? static_cast<size_type>(s.size() * 2 / MAX_LOAD_FACTOR) :
                                                                            DEFAULT_CAPACITY;

    buckets_ = bucket_array(capacity, bucket_type());

    for (const auto& pair : s) insert(pair);
}

// ----- Acceso de elementos -----
template<typename Key, typename T>
Unordered_map<Key,T>::mapped_reference Unordered_map<Key,T>::operator[](const_key_reference key) {
    if (buckets_.empty()) rehash(1);

    size_type bucket_index = hasher_(key) % buckets_.size();
    auto& bucket = buckets_[bucket_index];

    for (auto& pair : bucket) {
        if (pair.First() == key) {
            return pair.Second();
        }
    }

    ++size_;
    if (load_factor() >= MAX_LOAD_FACTOR) {
        rehash(buckets_.size() * 2);
        bucket_index = hasher_(key) % buckets_.size();
        bucket = buckets_[bucket_index];
    }
    
    return bucket.emplace_back(key, mapped_type()).Second();
}

template<typename Key, typename T>
Unordered_map<Key,T>::mapped_reference Unordered_map<Key,T>::at(const_key_reference key) {
    size_type bucket_index = hasher_(key) % buckets_.size();

    typename bucket_type::iterator bucket_node = buckets_[bucket_index].find(key);

    if(bucket_node != buckets_[bucket_index].end()) return bucket_node->Second();
    else throw std::out_of_range("Unordered_map::at: key not found");
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_mapped_reference Unordered_map<Key,T>::at(const_key_reference key) const {
    size_type bucket_index = hasher_(key) % buckets_.size();

    typename bucket_type::iterator bucket_node = buckets_[bucket_index].find(key);

    if(bucket_node != buckets_[bucket_index].end()) return bucket_node->Second();
    else throw std::out_of_range("Unordered_map::at: key not found");
}

// ----- Iteradores -----
template<typename Key, typename T>
Unordered_map<Key,T>::iterator Unordered_map<Key,T>::begin() {
    if (size_ == 0) return end();
    
    for (size_type i = 0; i < buckets_.size(); ++i) {
        if (!buckets_[i].empty()) return iterator(i, buckets_[i].begin(), this);
    }
}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator Unordered_map<Key,T>::end() {
    if (buckets_.size() > 0) return iterator(buckets_.size() - 1, buckets_[buckets_.size() - 1].end(), this);
    
    return iterator(0, typename bucket_type::iterator(nullptr, nullptr), this);
}
    
template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator Unordered_map<Key,T>::begin() const {
    if (size_ == 0) return cend();
    
    for (size_type i = 0; i < buckets_.size(); ++i) {
        if (!buckets_[i].empty()) return const_iterator(i, buckets_[i].cbegin(), this);
    }
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator Unordered_map<Key,T>::end() const {
    if (buckets_.size() > 0) return const_iterator(buckets_.size() - 1, buckets_[buckets_.size() - 1].cend(), this);
    
    return const_iterator(0, typename bucket_type::const_iterator(nullptr, nullptr), this);
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator Unordered_map<Key,T>::cbegin() const {
    if (size_ == 0) return cend();
    
    for (size_type i = 0; i < buckets_.size(); ++i) {
        if (!buckets_[i].empty()) return const_iterator(i, buckets_[i].cbegin(), this);
    }
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator Unordered_map<Key,T>::cend() const {
    if (buckets_.size() > 0) return const_iterator(buckets_.size() - 1, buckets_[buckets_.size() - 1].cend(), this);
    
    return const_iterator(0, typename bucket_type::const_iterator(nullptr, nullptr), this);
}

template<typename Key, typename T>
Pair<typename Unordered_map<Key,T>::iterator, bool> Unordered_map<Key,T>::insert(const_reference value) {
    if (buckets_.empty()) rehash(1);
    if (load_factor() >= MAX_LOAD_FACTOR) rehash(buckets_.empty() ? 1 : buckets_.size() * 2);
    
    iterator it = find(value.First());
    if(it != end()) return Pair(it, false);
    else {
        size_type bucket_index = hasher_(value.First()) % buckets_.size();
        auto node_it = buckets_[bucket_index].insert(buckets_[bucket_index].cend(),value);
        ++size_;

        iterator map_it(bucket_index,node_it,this);
        return Pair(map_it,true);
    }
}

template<typename Key, typename T>
Pair<typename Unordered_map<Key,T>::iterator, bool> Unordered_map<Key,T>::insert(value_type&& value) {
    if (buckets_.empty()) rehash(1);
    if (load_factor() >= MAX_LOAD_FACTOR) rehash(buckets_.empty() ? 1 : buckets_.size() * 2);
    
    iterator it = find(value.First());
    if(it != end()) return Pair(it, false);
    else {
        size_type bucket_index = hasher_(value.First()) % buckets_.size();
        auto node_it = buckets_[bucket_index].insert(buckets_[bucket_index].cend(),std::move(value));
        ++size_;

        iterator map_it(bucket_index,node_it,this);
        return Pair(map_it,true);
    }
}

template<typename Key, typename T>
template<class... Args>
Pair<typename Unordered_map<Key,T>::iterator, bool> Unordered_map<Key,T>::emplace(Args&&... args) {
    if (buckets_.empty()) rehash(1);
    if (load_factor() >= MAX_LOAD_FACTOR) rehash(buckets_.empty() ? 1 : buckets_.size() * 2);

    value_type new_value(std::forward<Args>(args)...);

    iterator it = find(new_value.First());
    if(it != end()) return Pair(it, false);
    else {
        size_type bucket_index = hasher_(new_value.First()) % buckets_.size();
        auto node_it = buckets_[bucket_index].emplace(buckets_[bucket_index].cend(),std::forward<Args>(args)...);
        ++size_;

        iterator map_it(bucket_index,node_it,this);
        return Pair(map_it,true);
    }
}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator Unordered_map<Key,T>::erase(const_iterator pos) {
    if(pos == cend() || pos.map_ != this || empty()) return end();

    size_type bucket_idx = pos.current_bucket_;
    auto bucket_it = pos.node_it_;

    auto& bucket = buckets_[bucket_idx];

    if (bucket_it == bucket.end()) return end();

    iterator next_it(bucket_idx, bucket_it, this);
    ++next_it;

    bucket.erase(bucket_it);
    --size_;

    return next_it;
}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator Unordered_map<Key,T>::erase(const_iterator first, const_iterator last) {
    iterator it(first);
    
    if (first == last) return it; 

    while(it!=iterator(last)) it = erase(const_iterator(it));

    return it;
}

template<typename Key, typename T>
Unordered_map<Key,T>::iterator Unordered_map<Key,T>::find(const_key_reference key) {
    size_type bucket_index = hasher_(key) % buckets_.size();
    
    for(auto it = buckets_[bucket_index].begin(); it != buckets_[bucket_index].end(); ++it){
        if(it->First() == key) return iterator(bucket_index, it, this);
    }

    return end();
}

template<typename Key, typename T>
Unordered_map<Key,T>::const_iterator Unordered_map<Key,T>::find(const_key_reference key) const {
    size_type bucket_index = hasher_(key) % buckets_.size();

    for(auto it = buckets_[bucket_index].cbegin(); it != buckets_[bucket_index].cend(); ++it){
        if(it->First() == key) return const_iterator(bucket_index, it, this);
    }

    return cend();
}

// ----- Capacidad -----
template<typename Key, typename T>
bool Unordered_map<Key,T>::empty() const noexcept {
    return size_ == 0;
}

template<typename Key, typename T>
Unordered_map<Key,T>::size_type Unordered_map<Key,T>::size() const noexcept {
    return size_;
}

template<typename Key, typename T>
Unordered_map<Key,T>::size_type Unordered_map<Key,T>::bucket_count() const noexcept {
    return buckets_.size();
}

template<typename Key, typename T>
Unordered_map<Key,T>::size_type Unordered_map<Key,T>::bucket_size(size_type bucket_index) const {
    if (bucket_index >= buckets_.size()) throw std::out_of_range("Unordered_map::bucket_size: invalid bucket index");
    return buckets_[bucket_index].size();
} 

// ----- Modificacion -----
template<typename Key, typename T>
void Unordered_map<Key,T>::clear() {
    for(auto &bucket: buckets_){
        bucket.clear();
    }
    size_ = 0;
}

template<typename Key, typename T>
void Unordered_map<Key,T>::erase(const_key_reference key) {
    auto it = find(key);
    if (it != end()) erase(it);
}

// ----- Comparadores -----
template<typename Key, typename T>
bool Unordered_map<Key,T>::operator==(const Unordered_map& other) const {
    if (this == &other) return true;
    if (size_ != other.size_) return false;
    
    for (const auto& pair : *this) {
        auto it = other.find(pair.First());
        
        if (it == other.end()) return false;
        if (pair.Second()!=it->Second()) return false;
    }

    return true;
}
    
template<typename Key, typename T>
bool Unordered_map<Key,T>::operator!=(const Unordered_map& other) const {
    return !(*this == other);
}

// ----- Helpers -----
template<typename Key, typename T>
float Unordered_map<Key,T>::load_factor() const {
    if (buckets_.empty()) return 0.0f;
    return static_cast<float>(size_) / buckets_.size();
}

template<typename Key, typename T>
void Unordered_map<Key,T>::rehash(size_type new_bucket_count) {
    if (new_bucket_count <= buckets_.size()) return;
    
    bucket_array new_buckets(Reserve, new_bucket_count);
    for (size_type i = 0; i < new_bucket_count; ++i) new_buckets.emplace_back();
    
    for (auto& old_bucket : buckets_) {
        while (!old_bucket.empty()) {
            value_type element = std::move(old_bucket.front());
            old_bucket.pop_front();
            
            size_type new_index = hasher_(element.First()) % new_bucket_count;
            
            new_buckets[new_index].push_back(std::move(element));
        }
    }
    
    buckets_ = std::move(new_buckets);
}

template<typename Key, typename T>
void Unordered_map<Key,T>::swap(Unordered_map& other) noexcept {

    bucket_array temp_buckets = std::move(buckets_);
    buckets_ = std::move(other.buckets_);
    other.buckets_ = std::move(temp_buckets);
    
    size_type temp_size = size_;
    size_ = other.size_;
    other.size_ = temp_size;
    
    std::hash<Key> temp_hasher = std::move(hasher_);
    hasher_ = std::move(other.hasher_);
    other.hasher_ = std::move(temp_hasher);
}
