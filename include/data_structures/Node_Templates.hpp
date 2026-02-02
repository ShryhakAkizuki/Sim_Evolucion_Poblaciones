#pragma once

#include <memory>           // std::unique_ptr
#include <algorithm>        // Para std::copy, std::move, etc. (en implementaciones)
#include <utility>          // Para std::forward, std::move, etc.

template<typename T>
class NodeBase {
public:
    // ----- Aliases -----
    using value_type = T;
    
    using reference = T&;
    using const_reference = const T&;

    // ----- Funciones especiales -----
    NodeBase();
    NodeBase(const NodeBase& other) = delete;
    NodeBase(NodeBase&& other) noexcept;
    NodeBase& operator=(const NodeBase& other) = delete;
    NodeBase& operator=(NodeBase&& other) noexcept;
    ~NodeBase();

    explicit NodeBase(const_reference value);
    explicit NodeBase(T&& value);

    // ----- Acceso de elementos -----
    reference Value();
    const_reference Value() const;

    // ----- Modificacion -----
    void SetValue(const_reference value);
    void SetValue(T&& value);

    template<typename... Args>
    void SetValue(Args&&... args);

    // ----- Comparadores -----
    bool operator==(const NodeBase& other) const;
    bool operator!=(const NodeBase& other) const;

protected:
    // ----- Atributos -----
    value_type data_ = value_type();

};


template<typename T>
class DListNode : public NodeBase<T>{
public:
    // ----- Aliases -----
    using value_type = NodeBase<T>::value_type;
    
    using reference = NodeBase<T>::reference;
    using const_reference = NodeBase<T>::const_reference;
    
    using owner_pointer = std::unique_ptr<DListNode<T>>;
    using observer_pointer = DListNode<T>*;
    using const_observer_pointer = const DListNode<T>*;

    // ----- Funciones especiales -----
    DListNode();
    DListNode(const DListNode& other) = delete;
    DListNode(DListNode&& other) noexcept;
    DListNode& operator=(const DListNode& other) = delete;
    DListNode& operator=(DListNode&& other) noexcept;
    ~DListNode();

    explicit DListNode(const_reference value, owner_pointer next = nullptr, observer_pointer back = nullptr);
    explicit DListNode(T&& value, owner_pointer next = nullptr, observer_pointer back = nullptr);

    // ----- Acceso de elementos -----
    observer_pointer Next();
    const_observer_pointer Next() const;

    observer_pointer Back();
    const_observer_pointer Back() const;

    // ----- Modificacion -----
    void SetNext(owner_pointer next);
    void SetBack(observer_pointer back);
    
    owner_pointer release_next();


protected:
    // ----- Atributos -----
    observer_pointer back_ = nullptr;
    owner_pointer next_ = nullptr;
};

// #################### NodeBase - Class ###################

// ----- Funciones especiales -----
template<typename T>    
NodeBase<T>::NodeBase() :
data_(value_type()) {}    

template<typename T>    
NodeBase<T>::NodeBase(NodeBase&& other) noexcept :
data_(other.data_) {
    other.data_ = value_type();
}

template<typename T>    
NodeBase<T>& NodeBase<T>::operator=(NodeBase&& other) noexcept {
    if(this != &other){
        data_ = other.data_;
        other.data_ = value_type();
    }
    return *this;
}

template<typename T>    
NodeBase<T>::~NodeBase() {
    data_.~value_type();
}

template<typename T>    
NodeBase<T>::NodeBase(const_reference value) :
data_(value) {}

template<typename T>    
NodeBase<T>::NodeBase(T&& value) :
data_(std::move(value)) {}

// ----- Acceso de elementos -----
template<typename T>    
NodeBase<T>::reference NodeBase<T>::Value() {
    return data_;
}

template<typename T>    
NodeBase<T>::const_reference NodeBase<T>::Value() const {
    return data_;
}

// ----- Modificacion -----
template<typename T>    
void NodeBase<T>::SetValue(const_reference value) {
    data_ = value;
}

template<typename T>    
void NodeBase<T>::SetValue(T&& value) {
    data_ = std::move(value);
}

template<typename T>    
template<typename... Args>
void NodeBase<T>::SetValue(Args&&... args) {
    data_ = value_type(std::forward<Args>(args)...);
}

// ----- Comparadores -----
template<typename T>    
bool NodeBase<T>::operator==(const NodeBase& other) const {
    if (this == &other) return true;
    return (data_ == other.data_);
}

template<typename T>    
bool NodeBase<T>::operator!=(const NodeBase& other) const {
    return !(*this == other);
}

// #################### DListNode - Class ###################

// ----- Funciones especiales -----
template<typename T>    
DListNode<T>::DListNode() :
NodeBase<T>(), back_(nullptr), next_(nullptr) {}

template<typename T>    
DListNode<T>::DListNode(DListNode&& other) noexcept :
NodeBase<T>(std::move(other)),  back_(other.back_), next_(std::move(other.next_)) {
    other.back_ = nullptr;
    other.next_ = nullptr;
}

template<typename T>    
DListNode<T>& DListNode<T>::operator=(DListNode&& other) noexcept {
    if(this != &other){
        NodeBase<T>::operator=(std::move(other));

        back_ = nullptr;
        next_.reset();

        
        back_ = other.back_;
        next_ = std::move(other.next_);

        other.back_ = nullptr;
        other.next_ = nullptr;
    }
    return *this;
}

template<typename T>    
DListNode<T>::~DListNode() {
    back_ = nullptr;
    next_.reset();
}

template<typename T>    
DListNode<T>::DListNode(const_reference value, owner_pointer next, observer_pointer back) :
NodeBase<T>(value), back_(back), next_(std::move(next)) {}

template<typename T>    
DListNode<T>::DListNode(T&& value, owner_pointer next, observer_pointer back) :
NodeBase<T>(std::move(value)), back_(back), next_(std::move(next)) {}

// ----- Acceso de elementos -----
template<typename T>    
DListNode<T>::observer_pointer DListNode<T>::Next() {
    return next_.get();
}

template<typename T>    
DListNode<T>::const_observer_pointer DListNode<T>::Next() const {
    return next_.get();
}

template<typename T>    
DListNode<T>::observer_pointer DListNode<T>::Back() {
    return back_;
}

template<typename T>    
DListNode<T>::const_observer_pointer DListNode<T>::Back() const {
    return back_;
}

// ----- Modificacion -----

template<typename T>    
void DListNode<T>::SetNext(owner_pointer next) {
    next_.reset();
    next_ = std::move(next);
}

template<typename T>    
void DListNode<T>::SetBack(observer_pointer back) {
    back_ = nullptr;
    back_ = back;
}

template<typename T>    
DListNode<T>::owner_pointer DListNode<T>::release_next(){
    return std::move(next_);
}
