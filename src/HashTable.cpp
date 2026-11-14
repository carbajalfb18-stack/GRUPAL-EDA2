#include <iostream>
#include <string>

#include "Utilidades.cpp"
#include "Pedido.cpp"
#pragma once  // se quita cuando juntemos todo en un solo main

using namespace std;
 
// ============================================================
//  HashTable.cpp
//  Busca pedidos por su id. Si dos ids caen en la misma posición
//  (colisión), se guardan uno detrás de otro en una cadena.
// ============================================================
 
struct HashNode {
    string key;
    Pedido* valor;
    HashNode* next;

    HashNode(string c, Pedido* v) {
        key = c;
        valor = v;
        next = nullptr;
    }
};

class HashTable {
  HashNode** tabla;
  int capacidad;
  int colisiones;
  // convierte el id en una posición de la tabla
  int funcionHash(string idPedido) {
    int suma = 0;
    for (char c : idPedido) {
      suma += c;
    }
    return suma % capacidad;
  }
    public:
      HashTable(int cap) {
        capacidad = cap;
        colisiones = 0;
        tabla = new HashNode*[capacidad];
        for (int i = 0; i < capacidad; i++) {
          tabla[i] = nullptr;
        }
    }

    bool insertar(string idPedido, Pedido* ped) {
        int indice = funcionHash(idPedido);

        // si el id ya existe, no se guarda
        HashNode* actual = tabla[indice];
        while (actual != nullptr) {
            if (actual->key == idPedido) {
                return false;
            }
            actual = actual->next;
        }

        HashNode* nuevoNodo = new HashNode(idPedido, ped);

        if (tabla[indice] == nullptr) {
            tabla[indice] = nuevoNodo;
        } else {
            // ya había pedidos en esa posición (colisión): lo pongo al inicio de la cadena
            colisiones++;
            nuevoNodo->next = tabla[indice];
            tabla[indice] = nuevoNodo;
        }
        return true;
    }

    Pedido* buscar(string idPedido){
        int indice =funcionHash(idPedido);
        HashNode* actual = tabla[indice];
        while (actual != nullptr){
            if (actual->key == idPedido){
                cout <<"Pedido ID: "<<actual->valor->idPedido<<endl
                <<"DNI: "<<actual->valor->dniCliente<<endl
                <<"Tipo Envio: "<<actual->valor->tipoEnvio<<endl
                <<"Prioridad: "<<actual->valor->nivelPrioridad<<endl
                <<"Monto: "<<actual->valor->montoDolares<<endl
                <<"Fecha: "<<actual->valor->fecha<<endl
                <<"Hora: "<<actual->valor->hora<<endl
                <<"Estado: "<<actual->valor->estado<<endl;
                return actual->valor;
            }
            actual=actual->next;
        }
        cout <<"No encontrado"<<endl;
        return nullptr;
    }

    void eliminar(string idPedido) {
        int indice = funcionHash(idPedido);
        HashNode* actual = tabla[indice];
        HashNode* anterior = nullptr;

        while (actual != nullptr) {
            if (actual->key == idPedido) {
                if (anterior == nullptr) {
                    tabla[indice] = actual->next;
                } else {
                    anterior->next = actual->next;
                }

                delete actual;
                cout << "idPedido " << idPedido << " eliminada." << endl;
                return;
            }

            anterior = actual;
            actual = actual->next;
        }

        cout << "idPedido no encontrada para eliminar." << endl;
    }

    int getColisiones(){
        return colisiones;
    }

    void liberarPedidos(){
        for (int i = 0; i < capacidad; i++) {
            HashNode* actual = tabla[i];
            while (actual != nullptr) {
                delete actual->valor;
                actual = actual->next;
            }
        }
    }

    int getCantidad(){
        int total = 0;
        for (int i = 0; i < capacidad; i++) {
            HashNode* actual = tabla[i];
            while (actual != nullptr) {
                total++;
                actual = actual->next;
            }
        }
        return total;
    }
    ~HashTable() {
        for (int i = 0; i < capacidad; i++) {
            HashNode* actual = tabla[i];
            while (actual != nullptr) {
                HashNode* temporal = actual;
                actual = actual->next;
                delete temporal;
            }
        }
        delete[] tabla;
    }

    void rehash(int nuevaCapacidad) {
        if (nuevaCapacidad <= capacidad) return;  // solo sirve para agrandar la tabla

        HashNode** tablaVieja = tabla;
        int capacidadVieja = capacidad;

        capacidad = nuevaCapacidad;
        tabla = new HashNode*[capacidad];
        for (int i = 0; i < capacidad; i++) {
            tabla[i] = nullptr;
        }

        for (int i = 0; i < capacidadVieja; i++) {
            HashNode* actual = tablaVieja[i];
            while (actual != nullptr) {
                HashNode* siguiente = actual->next;  // lo guardo porque voy a mover este nodo

                int nuevoIndice = funcionHash(actual->key);

                actual->next = tabla[nuevoIndice];
                tabla[nuevoIndice] = actual;

                actual = siguiente;
            }
        }

        // borro solo el arreglo viejo; los nodos ya se movieron a la tabla nueva
        delete[] tablaVieja;
    }

};