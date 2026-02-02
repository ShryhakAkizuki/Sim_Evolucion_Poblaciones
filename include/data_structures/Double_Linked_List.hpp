#pragma once

#include <cstddef>          // Para std::size_t, std::ptrdiff_t
#include <initializer_list> // Para std::initializer_list
#include <span>             // Para std::span (C++20)
#include <stdexcept>        // Para std::out_of_range (en at())
#include <algorithm>        // Para std::copy, std::move, etc. (en implementaciones)
#include <utility>          // Para std::forward, std::move, etc.
#include <iterator>         // Para std::distance, categorías de iteradores
#include <concepts>

#include "data_structures/Node_Templates.hpp"

template<typename T>
class Double_Linked_List{
public:
    // ----- Aliases -----
    using value_type = DListNode<T>::value_type;
    
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using reference = DListNode<T>::reference;
    using const_reference = DListNode<T>::const_reference;
    
    using owner_pointer = DListNode<T>::owner_pointer;
    using observer_pointer = DListNode<T>::observer_pointer;
    using const_observer_pointer = DListNode<T>::const_observer_pointer;

    using pointer = T*;
    using const_pointer     = const T*;

    // ----- Iteradores -----
    class iterator;
    class const_iterator;
    
    // ----- Funciones especiales -----
    Double_Linked_List();
    Double_Linked_List(const Double_Linked_List& other) = delete;
    Double_Linked_List(Double_Linked_List&& other) noexcept;
    Double_Linked_List& operator=(const Double_Linked_List& other) = delete;
    Double_Linked_List& operator=(Double_Linked_List&& other) noexcept;
    ~Double_Linked_List();

    Double_Linked_List(size_type size, const_reference value);
    Double_Linked_List(std::initializer_list<T> init);
    explicit Double_Linked_List(std::span<T> s);

    template<std::input_iterator It>
    Double_Linked_List(It first, It last);

    // ----- Acceso de elementos -----
    reference at(size_type index);
    const_reference at(size_type index) const;

    reference front();
    reference back();
    const_reference front() const;
    const_reference back() const;
    
    // ----- Iteradores -----
    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    iterator insert(const_iterator pos, const_reference value);
    iterator insert(const_iterator pos, T&& value);

    template<std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last);

    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    iterator find(const_reference value);
    const_iterator find(const_reference value) const;
    
    // ----- Capacidad -----
    bool empty() const noexcept;
    size_type size() const noexcept;

    // ----- Modificacion -----
    void clear();

    void push_back(const_reference value);
    void push_back(T&& value);

    template<typename... Args>
    reference emplace_back(Args&&... args);

    void push_front(const_reference value);
    void push_front(T&& value);

    template<typename... Args>
    reference emplace_front(Args&&... args);
    
    void insert(size_type index, const_reference value);
    void insert(size_type index, T&& value);

    template<typename... Args>
    reference emplace(size_type index, Args&&... args);
    
    void pop_back();
    void pop_front();
    void erase(size_type index);

    // ----- Comparadores -----
    bool operator==(const Double_Linked_List& other) const;
    bool operator!=(const Double_Linked_List& other) const;

    // ----- Helpers -----
    void swap(Double_Linked_List& other) noexcept;

    private:
    // ----- Atributos -----
    observer_pointer tail_ = nullptr;
    owner_pointer head_ = nullptr;

    size_type size_ = 0;
};

template<typename T>
class Double_Linked_List<T>::iterator {
public:
    // ----- Aliases -----
    using iterator_category = std::bidirectional_iterator_tag;
    
    using value_type = T;
    using difference_type = std::ptrdiff_t;

    using reference = T&;
    using const_reference =  const T&;
    
    using pointer = T*;
    using const_pointer = const T*;

    using const_list_pointer = const Double_Linked_List<T>*;

    friend class const_iterator;
    friend class Double_Linked_List;

    // ----- Funciones especiales -----
    iterator();
    explicit iterator(observer_pointer node, const_list_pointer list);
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
    observer_pointer current_ = nullptr;
    const_list_pointer list_ = nullptr;
    
    // ----- Funciones especiales -----
    explicit iterator(const const_iterator& other);
    iterator& operator=(const const_iterator& other);

};

template<typename T>
class Double_Linked_List<T>::const_iterator {
public:
    // ----- Aliases -----
    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = T;
    using difference_type = std::ptrdiff_t;

    using reference = T&;
    using const_reference =  const T&;
    
    using pointer = T*;
    using const_pointer = const T*;

    using const_list_pointer = const Double_Linked_List<T>*;

    friend class iterator;
    friend class Double_Linked_List;

    // ----- Funciones especiales -----
    const_iterator();
    explicit const_iterator(const_observer_pointer node, const_list_pointer list);
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
    const_observer_pointer current_ = nullptr;
    const_list_pointer list_ = nullptr;
};

// #################### iterator ###################
// ----- Funciones especiales -----
template<typename T>
Double_Linked_List<T>::iterator::iterator() :
current_(nullptr), list_(nullptr) {}

template<typename T>
Double_Linked_List<T>::iterator::iterator(observer_pointer node, const_list_pointer list) :
current_(node), list_(list) {}

template<typename T>
Double_Linked_List<T>::iterator::iterator(const iterator& other) :
current_(other.current_), list_(other.list_) {}

template<typename T>
Double_Linked_List<T>::iterator::iterator(const const_iterator& other) :
current_(const_cast<observer_pointer>(other.current_)), list_(other.list_) {}

template<typename T>
Double_Linked_List<T>::iterator& Double_Linked_List<T>::iterator::operator=(const iterator& other){
    current_ = other.current_;
    list_ = other.list_;
    return *this;
}

template<typename T>
Double_Linked_List<T>::iterator& Double_Linked_List<T>::iterator::operator=(const const_iterator& other) {
    current_ = const_cast<observer_pointer>(other.current_);
    list_ = other.list_;
    return *this;
}

// ----- Operadores especiales -----
template<typename T>
Double_Linked_List<T>::iterator::reference Double_Linked_List<T>::iterator::operator*() const {
    if(current_==nullptr) throw std::runtime_error("Double_Linked_List::iterator::operator*: Dereferencing end iterator");
    return current_->Value();
}   

template<typename T>
Double_Linked_List<T>::iterator::pointer Double_Linked_List<T>::iterator::operator->() const {
    if(current_==nullptr) throw std::runtime_error("Double_Linked_List::iterator::operator->: Dereferencing end iterator");
    return &current_->Value();
}   

template<typename T>
Double_Linked_List<T>::iterator& Double_Linked_List<T>::iterator::operator++() {
    if(current_!=nullptr) current_ = current_->Next();
    return *this;
}   

template<typename T>
Double_Linked_List<T>::iterator Double_Linked_List<T>::iterator::operator++(int) {
    iterator temp = *this;
    ++(*this);
    return temp;
}  
   
template<typename T>
Double_Linked_List<T>::iterator& Double_Linked_List<T>::iterator::operator--() {
    if(current_ == nullptr) current_ = list_->tail_;
    else current_ = current_->Back();
    return *this;
}   

template<typename T>
Double_Linked_List<T>::iterator Double_Linked_List<T>::iterator::operator--(int) {
    iterator temp = *this;
    --(*this);
    return temp;
}  

// ----- Comparadores -----
template<typename T>
bool Double_Linked_List<T>::iterator::operator==(const iterator& other) const {
    return current_ == other.current_;
}  

template<typename T>
bool Double_Linked_List<T>::iterator::operator!=(const iterator& other) const {
    return current_ != other.current_;
}  

// #################### const_iterator ###################
// ----- Funciones especiales -----
template<typename T>
Double_Linked_List<T>::const_iterator::const_iterator() :
current_(nullptr), list_(nullptr) {}

template<typename T>
Double_Linked_List<T>::const_iterator::const_iterator(const_observer_pointer node, const_list_pointer list) :
current_(node), list_(list) {}

template<typename T>
Double_Linked_List<T>::const_iterator::const_iterator(const iterator& it) :
current_(it.current_), list_(it.list_) {}

template<typename T>
Double_Linked_List<T>::const_iterator::const_iterator(const const_iterator& other) :
current_(other.current_), list_(other.list_) {}

template<typename T>
Double_Linked_List<T>::const_iterator& Double_Linked_List<T>::const_iterator::operator=(const const_iterator& other) {
    current_ = other.current_;
    list_ = other.list_;
    return *this;
}

template<typename T>
Double_Linked_List<T>::const_iterator& Double_Linked_List<T>::const_iterator::operator=(const iterator& other) {
    current_ = other.current_;
    list_ = other.list_;
    return *this;
}

// ----- Operadores especiales -----
template<typename T>
Double_Linked_List<T>::const_iterator::const_reference Double_Linked_List<T>::const_iterator::operator*() const {
    if(current_==nullptr) throw std::runtime_error("Double_Linked_List::const_iterator::operator*: Dereferencing end iterator");
    return current_->Value();
}   

template<typename T>
Double_Linked_List<T>::const_iterator::const_pointer Double_Linked_List<T>::const_iterator::operator->() const {
    if(current_==nullptr) throw std::runtime_error("Double_Linked_List::const_iterator::operator->: Dereferencing end iterator");
    return &current_->Value();
}   

template<typename T>
Double_Linked_List<T>::const_iterator& Double_Linked_List<T>::const_iterator::operator++() {
    if(current_!=nullptr) current_ = current_->Next();
    return *this;
}   

template<typename T>
Double_Linked_List<T>::const_iterator Double_Linked_List<T>::const_iterator::operator++(int) {
    const_iterator temp = *this;
    ++(*this);
    return temp;
}  
   
template<typename T>
Double_Linked_List<T>::const_iterator& Double_Linked_List<T>::const_iterator::operator--() {
    if(current_ == nullptr) current_ = list_->tail_;
    else current_ = current_->Back();
    return *this;
}   

template<typename T>
Double_Linked_List<T>::const_iterator Double_Linked_List<T>::const_iterator::operator--(int) {
    const_iterator temp = *this;
    --(*this);
    return temp;
}  

// ----- Comparadores -----
template<typename T>
bool Double_Linked_List<T>::const_iterator::operator==(const const_iterator& other) const {
    return current_ == other.current_;
}  

template<typename T>
bool Double_Linked_List<T>::const_iterator::operator!=(const const_iterator& other) const {
    return current_ != other.current_;
}  

// #################### Double_Linked_List ###################
// ----- Funciones especiales -----
template<typename T>
Double_Linked_List<T>::Double_Linked_List() : 
tail_(nullptr), head_(nullptr), size_(0) {};

template<typename T>
Double_Linked_List<T>::Double_Linked_List(Double_Linked_List&& other) noexcept :
tail_(other.tail_), head_(std::move(other.head_)), size_(other.size_) {
    other.tail_ = nullptr;
    other.head_ = nullptr;
    other.size_ = 0;
}

template<typename T>
Double_Linked_List<T>& Double_Linked_List<T>::operator=(Double_Linked_List&& other) noexcept {
    if(this != &other){
        tail_ = nullptr;
        head_.reset();
        size_ = 0;

        tail_ = other.tail_;
        head_ = std::move(other.head_);
        size_ = other.size_;

        other.tail_ = nullptr;
        other.head_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template<typename T>
Double_Linked_List<T>::~Double_Linked_List() {
    clear();
}

template<typename T>
Double_Linked_List<T>::Double_Linked_List(size_type size, const_reference value) :
size_(0) {
    if(size == 0){
        head_ = nullptr;
        tail_ = nullptr;
        return;
    }

    for(size_type i = 0; i < size; ++i) push_back(value);
}

template<typename T>
Double_Linked_List<T>::Double_Linked_List(std::initializer_list<T> init) :
size_(0) {
    if (init.size() == 0) {
        head_ = nullptr;
        tail_ = nullptr;
        return;
    }

    for(auto it = init.begin(); it != init.end(); ++it) push_back(*it);
}

template<typename T>
Double_Linked_List<T>::Double_Linked_List(std::span<T> s) :
size_(0) {
    if (s.empty()) {
        head_ = nullptr;
        tail_ = nullptr;
        return;
    }

    for(auto it = s.begin(); it != s.end(); ++it) push_back(*it);

}

template<typename T>
template<std::input_iterator It>
Double_Linked_List<T>::Double_Linked_List(It first, It last) : 
size_(0) {
    if (first == last) {
        head_ = nullptr;
        tail_ = nullptr;
        return;
    }

    for(It current = first; current != last; ++current) push_back(*current);
}

// ----- Acceso de elementos -----
template<typename T>
Double_Linked_List<T>::reference Double_Linked_List<T>::at(size_type index) {
    if(index >= size_) throw std::out_of_range("Double_Linked_List::at: index out of range");
    iterator it;

    if(index <= size_/2) {
        it = begin();
        for(size_type i = 0; i < index; ++i) ++it;
    } else {
        it = iterator(tail_, this);
        for(size_type i = size_-1; i > index; --i) --it;
    }
    return *it;

}

template<typename T>
Double_Linked_List<T>::const_reference Double_Linked_List<T>::at(size_type index) const {
    if(index >= size_) throw std::out_of_range("Double_Linked_List::at: index out of range");
    const_iterator it;

    if(index <= size_/2) {
        it = begin();
        for(size_type i = 0; i < index; ++i) ++it;
    } else {
        it = const_iterator(tail_, this);
        for(size_type i = size_-1; i > index; --i) --it;
    }
    return *it;
}

template<typename T>
Double_Linked_List<T>::reference Double_Linked_List<T>::front() {
    if(size_ == 0) throw std::out_of_range("Double_Linked_List::front: Array without elements");
    return head_->Value();
}

template<typename T>
Double_Linked_List<T>::reference Double_Linked_List<T>::back() {
    if(size_ == 0) throw std::out_of_range("Double_Linked_List::back: Array without elements");
    return tail_->Value();
}

template<typename T>
Double_Linked_List<T>::const_reference Double_Linked_List<T>::front() const {
    if(size_ == 0) throw std::out_of_range("Double_Linked_List::front: Array without elements");
    return head_->Value();
}

template<typename T>
Double_Linked_List<T>::const_reference Double_Linked_List<T>::back() const {
    if(size_ == 0) throw std::out_of_range("Double_Linked_List::back: Array without elements");
    return tail_->Value();
}

// ----- Iteradores -----
template<typename T>
Double_Linked_List<T>::iterator Double_Linked_List<T>::begin() {
    return iterator(head_.get(), this);
}

template<typename T>
Double_Linked_List<T>::iterator Double_Linked_List<T>::end() {
    return iterator(nullptr, this);
}

template<typename T>
Double_Linked_List<T>::const_iterator Double_Linked_List<T>::begin() const {
    return const_iterator(head_.get(), this);
}

template<typename T>
Double_Linked_List<T>::const_iterator Double_Linked_List<T>::end() const {
    return const_iterator(nullptr, this);
}

template<typename T>
Double_Linked_List<T>::const_iterator Double_Linked_List<T>::cbegin() const {
    return const_iterator(head_.get(), this);
}

template<typename T>
Double_Linked_List<T>::const_iterator Double_Linked_List<T>::cend() const {
    return const_iterator(nullptr, this);
}

template<typename T>
Double_Linked_List<T>::iterator Double_Linked_List<T>::insert(const_iterator pos, const_reference value) {
    if(pos == cbegin()) {
        owner_pointer new_node = std::make_unique<DListNode<value_type>>(value, std::move(head_));
        observer_pointer next_node = new_node->Next();

        if(next_node != nullptr) next_node->SetBack(new_node.get());
        else tail_ = new_node.get();
        
        head_ = std::move(new_node);
        ++size_;
        return iterator(head_.get(), this);

    } else if(pos == cend()) {
        if(!empty()){
            tail_->SetNext(std::make_unique<DListNode<value_type>>(value, nullptr, tail_));
            tail_ = tail_->Next();
        } else {
            head_ = std::make_unique<DListNode<value_type>>(value);
            tail_ = head_.get();
        }

        ++size_;   
        return iterator(tail_, this);

    } else {
        observer_pointer prev_node = iterator(pos).current_->Back();
        owner_pointer next_node = prev_node->release_next();

        owner_pointer new_node = std::make_unique<DListNode<T>>(value, std::move(next_node), prev_node);
        
        new_node->Next()->SetBack(new_node.get());
        prev_node->SetNext(std::move(new_node));

        ++size_;
        return iterator(prev_node->Next(), this);
    }
}

template<typename T>
Double_Linked_List<T>::iterator Double_Linked_List<T>::insert(const_iterator pos, T&& value) {
    if(pos == cbegin()) {
        owner_pointer new_node = std::make_unique<DListNode<value_type>>(std::move(value), std::move(head_));
        observer_pointer next_node = new_node->Next();

        if(next_node != nullptr) next_node->SetBack(new_node.get());
        else tail_ = new_node.get();
        
        head_ = std::move(new_node);
        ++size_;
        return iterator(head_.get(), this);

    } else if(pos == cend()) {
        if(!empty()){
            tail_->SetNext(std::make_unique<DListNode<value_type>>(std::move(value), nullptr, tail_));
            tail_ = tail_->Next();
        } else {
            head_ = std::make_unique<DListNode<value_type>>(std::move(value));
            tail_ = head_.get();
        }

        ++size_;   
        return iterator(tail_, this);

    } else {
        observer_pointer prev_node = iterator(pos).current_->Back();
        owner_pointer next_node = prev_node->release_next();

        owner_pointer new_node = std::make_unique<DListNode<value_type>>(std::move(value), std::move(next_node), prev_node);
        
        new_node->Next()->SetBack(new_node.get());
        prev_node->SetNext(std::move(new_node));

        ++size_;
        return iterator(prev_node->Next(), this);
    }
}

template<typename T>
template<std::input_iterator InputIt>
Double_Linked_List<T>::iterator Double_Linked_List<T>::insert(const_iterator pos, InputIt first, InputIt last) {
    iterator first_insert = iterator(pos);

    if (first == last) return first_insert;

    first_insert = insert(pos, *first);
    iterator it(first_insert);

    for(InputIt current = ++first; current != last; ++current) {
        ++it;
        it = insert(const_iterator(it), *current);
    }

    return first_insert;
}

template<typename T>
template<class... Args>
Double_Linked_List<T>::iterator Double_Linked_List<T>::emplace(const_iterator pos, Args&&... args) {
    if(pos == cbegin()) {
        owner_pointer new_node = std::make_unique<DListNode<value_type>>(value_type(std::forward<Args>(args)...), std::move(head_));
        observer_pointer next_node = new_node->Next();

        if(next_node != nullptr) next_node->SetBack(new_node.get());
        else tail_ = new_node.get();
        
        head_ = std::move(new_node);
        ++size_;
        return begin();

    } else if(pos == cend()) {
        if(!empty()){
            tail_->SetNext(std::make_unique<DListNode<value_type>>(value_type(std::forward<Args>(args)...), nullptr, tail_));
            tail_ = tail_->Next();
        } else {
            head_ = std::make_unique<DListNode<value_type>>(value_type(std::forward<Args>(args)...));
            tail_ = head_.get();
        }

        ++size_;   
        return iterator(tail_, this);

    } else {
        observer_pointer prev_node = iterator(pos).current_->Back();
        owner_pointer next_node = prev_node->release_next();

        owner_pointer new_node = std::make_unique<DListNode<value_type>>(value_type(std::forward<Args>(args)...), std::move(next_node), prev_node);
        
        new_node->Next()->SetBack(new_node.get());
        prev_node->SetNext(std::move(new_node));

        ++size_;
        return iterator(prev_node->Next(), this);
    }  
}

template<typename T>
Double_Linked_List<T>::iterator Double_Linked_List<T>::erase(const_iterator pos) {
    if(pos == cend() || empty()) return end();
    
    if(pos == cbegin()) {
        head_ = std::move(head_->release_next());

        if(head_ == nullptr) tail_ = nullptr; 
        else head_->SetBack(nullptr);
    
        --size_;
        return begin();
    } else{
        observer_pointer prev_node = iterator(pos).current_->Back();

        prev_node->SetNext(std::move(iterator(pos).current_->release_next()));

        if(prev_node->Next() != nullptr) prev_node->Next()->SetBack(prev_node);
        else tail_ = prev_node;

        --size_;
        return iterator(prev_node->Next(), this);
    }
}

template<typename T>
Double_Linked_List<T>::iterator Double_Linked_List<T>::erase(const_iterator first, const_iterator last) {
    iterator it(first);
    
    if (first == last) it; 

    while(it!=iterator(last)) it = erase(const_iterator(it));

    return it;
}

template<typename T>            
Double_Linked_List<T>::iterator Double_Linked_List<T>::find(const_reference value) {
    for(iterator it = begin(); it != end(); ++it) if (*it == value) return it;
    return end();
}

template<typename T>            
Double_Linked_List<T>::const_iterator Double_Linked_List<T>::find(const_reference value) const {
    for(const_iterator it = cbegin(); it != cend(); ++it) if (*it == value) return it;
    return cend();
}

// ----- Capacidad -----

template<typename T>
bool Double_Linked_List<T>::empty() const noexcept {
    return (size_ == 0 && head_ == nullptr);
}

template<typename T>
Double_Linked_List<T>::size_type Double_Linked_List<T>::size() const noexcept {
    return size_;
}

// ----- Modificacion -----
template<typename T>
void Double_Linked_List<T>::clear() {
    erase(cbegin(), cend());
}

template<typename T>
void Double_Linked_List<T>::push_back(const_reference value) {
    if(empty()) {
        head_ = std::make_unique<DListNode<value_type>>(value);
        tail_ = head_.get();
    } else {
        tail_->SetNext(std::make_unique<DListNode<value_type>>(value, nullptr, tail_));
        tail_ = tail_->Next();
    }
    ++size_;
}

template<typename T>
void Double_Linked_List<T>::push_back(T&& value) {
    if(empty()) {
        head_ = std::make_unique<DListNode<value_type>>(std::move(value));
        tail_ = head_.get();
    } else {
        tail_->SetNext(std::make_unique<DListNode<value_type>>(std::move(value), nullptr, tail_));
        tail_ = tail_->Next();
    }
    ++size_;
}

template<typename T>
template<typename... Args>
Double_Linked_List<T>::reference Double_Linked_List<T>::emplace_back(Args&&... args) {
    if(empty()) {
        head_ = std::make_unique<DListNode<value_type>>(value_type(std::forward<Args>(args)...));
        tail_ = head_.get();
    } else {
        tail_->SetNext(std::make_unique<DListNode<value_type>>(value_type(std::forward<Args>(args)...), nullptr, tail_));
        tail_ = tail_->Next();
    }
    ++size_;

    return tail_->Value();
}

template<typename T>
void Double_Linked_List<T>::push_front(const_reference value) {
    if(empty()) {
        head_ = std::make_unique<DListNode<value_type>>(value);
        tail_ = head_.get();
    } else {
        owner_pointer new_node = std::make_unique<DListNode<value_type>>(value, std::move(head_));
        new_node->Next()->SetBack(new_node.get());
        head_ = std::move(new_node);
    }
    ++size_;
}

template<typename T>
void Double_Linked_List<T>::push_front(T&& value) {
    if(empty()) {
        head_ = std::make_unique<DListNode<value_type>>(std::move(value));
        tail_ = head_.get();
    } else {
        owner_pointer new_node = std::make_unique<DListNode<value_type>>(std::move(value), std::move(head_));
        new_node->Next()->SetBack(new_node.get());
        head_ = std::move(new_node);
    }
    ++size_; 
}

template<typename T>
template<typename... Args>
Double_Linked_List<T>::reference Double_Linked_List<T>::emplace_front(Args&&... args) {
    if(empty()) {
        head_ = std::make_unique<DListNode<value_type>>(value_type(std::forward<Args>(args)...));
        tail_ = head_.get();
    } else {
        owner_pointer new_node = std::make_unique<DListNode<value_type>>(value_type(std::forward<Args>(args)...), std::move(head_));
        new_node->Next()->SetBack(new_node.get());
        head_ = std::move(new_node);
    }
    ++size_; 

    return head_->Value();
}

template<typename T>
void Double_Linked_List<T>::insert(size_type index, const_reference value) {
    if (index > size_) throw std::out_of_range("Double_Linked_List::insert: position out of range");
    const_iterator it;

    if(index <= size_/2) {
        it = cbegin();
        for(size_type i = 0; i < index; ++i) ++it;
    } else {
        it = cend();
        for(size_type i = size_; i > index; --i) --it;
    }
    insert(it, value);
}

template<typename T>
void Double_Linked_List<T>::insert(size_type index, T&& value) {
    if (index > size_) throw std::out_of_range("Double_Linked_List::insert: position out of range");
    const_iterator it;

    if(index <= size_/2) {
        it = cbegin();
        for(size_type i = 0; i < index; ++i) ++it;
    } else {
        it = cend();
        for(size_type i = size_; i > index; --i) --it;
    }
    insert(it, std::move(value));
}

template<typename T>
template<typename... Args>
Double_Linked_List<T>::reference Double_Linked_List<T>::emplace(size_type index, Args&&... args) {
    if (index > size_) throw std::out_of_range("Double_Linked_List::emplace: position out of range");
    const_iterator it;

    if(index <= size_/2) {
        it = cbegin();
        for(size_type i = 0; i < index; ++i) ++it;
    } else {
        it = cend();
        for(size_type i = size_; i > index; --i) --it;
    }
    emplace(it, value_type(std::forward<Args>(args)...));
}

template<typename T>
void Double_Linked_List<T>::pop_back() {
    erase(const_iterator(tail_, this));
}

template<typename T>
void Double_Linked_List<T>::pop_front() {
    erase(cbegin());
}

template<typename T>
void Double_Linked_List<T>::erase(size_type index) {
    if (index > size_) throw std::out_of_range("Double_Linked_List::erase: position out of range");
    const_iterator it;

    if(index <= size_/2) {
        it = cbegin();
        for(size_type i = 0; i < index; ++i) ++it;
    } else {
        it = cend();
        for(size_type i = size_; i > index; --i) --it;
    }
    erase(it);
}

// ----- Comparadores -----
template<typename T>
bool Double_Linked_List<T>::operator==(const Double_Linked_List& other) const {
    if (this == &other) return true;
    if (size_ != other.size_) return false;
    if (head_.get() == other.head_.get()) return true;
    
    const_iterator it_1 = begin();
    const_iterator it_2 = other.begin();

    for(size_type i = 0; i < size_; i++){
        if((*it_1) != (*it_2)) return false;
        ++it_1;
        ++it_2;
    }
    return true;
}

template<typename T>
bool Double_Linked_List<T>::operator!=(const Double_Linked_List& other) const {
    return !(*this == other);
}

// ----- Helpers -----
template<typename T>
void Double_Linked_List<T>::swap(Double_Linked_List& other) noexcept {
    observer_pointer temp_tail = tail_;
    tail_ = other.tail_;
    other.tail_ = temp_tail;
    
    owner_pointer temp_head(std::move(head_));
    head_ = std::move(other.head_);
    other.head_ = std::move(temp_head);

    size_type temp_size = size_;
    size_ = other.size_;
    other.size_ = temp_size;
}
