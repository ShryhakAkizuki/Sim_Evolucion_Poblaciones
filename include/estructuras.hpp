#ifndef ESTRUCTURAS_HPP
#define ESTRUCTURAS_HPP

template<typename T>
struct Nodo_simple{
    T datos;
    Nodo_simple<T>* siguiente;
};

template<typename T>
class Lista_sencilla{
private:
    Nodo_simple<T>* head;
    int size;

public:
    Lista_sencilla(): head(nullptr), size(0){}
    ~Lista_sencilla(){
        Nodo_simple<T>* current = head;
        while(current != nullptr){
            Nodo_simple<T>* temp = current;
            current = current -> siguiente;
            delete temp;
        }
    }

    void push_front(const T &valor){
        Nodo_simple<T>* nuevo = new Nodo_simple<T>;
        nuevo->datos = valor;
        nuevo->siguiente = head;
        
        head = nuevo;
        size++;
    }

    Nodo_simple<T>* getHead() const{
        return head;
    }

    bool find(const T &valor){
        Nodo_simple<T>* current = head;
        while(current != nullptr){
            if(current->datos == valor){
                return true;
            }else{
                current = current -> siguiente;
            }
        }
        return false;
    }

    int getSize() const{
        return head;
    }
};

#endif
