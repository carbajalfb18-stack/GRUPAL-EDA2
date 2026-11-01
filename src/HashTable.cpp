#include <iostream>
#include <string>
#include <vector>
#include "Pedido.cpp"
using namespace std;
 
// ============================================================
//  HashTable.cpp  -  depende de: Pedido, Utilidades
//  Basado en el hash del profe (HashNode, tabla, capacidad, rehash)
//  Encadenamiento (lista enlazada por bucket)
// ============================================================
 
struct HashNode {
    string    key;
    Pedido*   valor;
    HashNode* next;
 
    HashNode(string k, Pedido* v) : key(k), valor(v), next(nullptr) {}
};
 
class HashTable {
private:
    vector<HashNode*> tabla;
    int capacidad;
    int cantidad;
    int colisiones;
 
    int indice(const string& key) {
        // TODO
        return 0;
    }
 
    void rehash(int nuevaCapacidad) {
        // TODO
    }
 
    double factorCarga() {
        // TODO
        return 0;
    }
 
public:
    HashTable(int capacidadInicial) {
        // TODO
    }
 
    ~HashTable() {
        // TODO: libera los HashNode, NO los Pedido
    }
 
    bool insertar(Pedido* p) {
        // TODO
        return false;
    }
 
    Pedido* buscar(const string& id) {
        // TODO
        return nullptr;
    }
 
    bool eliminar(const string& id) {
        // TODO
        return false;
    }
 
    int getCantidad() {
        // TODO
        return 0;
    }
 
    int getColisiones() {
        // TODO
        return 0;
    }
};