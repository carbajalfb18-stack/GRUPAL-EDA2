
// #include <vector>
using namespace std;

#include <iostream>
#include <string>

#include "Utilidades.cpp"
#include "Pedido.cpp"
#pragma once                   /* para que no se repitan cuando terminemos ponemos todo junto y se quita */

using namespace std;
 
// ============================================================
//  HashTable.cpp  -  depende de: Pedido, Utilidades
//  Basado en el hash del profe (HashNode, tabla, capacidad, rehash)
//  Encadenamiento (lista enlazada por bucket)
// ============================================================
 

// Estructura para los nodos de la lista enlazada
struct HashNode {
    string key;
    Pedido* valor;
    HashNode* next; // Puntero al siguiente nodo en caso de colisi-n

    // Constructor del nodo
    HashNode(string c, Pedido* v) {
        key = c;
        valor = v;
        next = nullptr;
    }
};

class HashTable {
  HashNode** tabla;//Puntero doble: Un arreglo de punteros a
  int capacidad;
  // Funci-n hash privada
  int funcionHash(string idPedido) {
    int suma = 0;
    for (char c : idPedido) {
      suma += c;
    }
    return suma % capacidad;//Asegura que el -ndice este en r
  }
    public:
    // Constructor
      HashTable(int cap) {
        capacidad = cap;
        tabla = new HashNode*[capacidad];//Crear el arreglo de
        // Inicializar todos los punteros a nullptr (vacios)
        for (int i = 0; i < capacidad; i++) {
          tabla[i] = nullptr;
        }
    }

    bool insertar(string idPedido, Pedido* ped) {
        int indice = funcionHash(idPedido);

        // Si el id ya existe en este bucket, se rechaza
        HashNode* actual = tabla[indice];
        while (actual != nullptr) {
            if (actual->key == idPedido) {
                return false;
            }
            actual = actual->next;
        }

        HashNode* nuevoNodo = new HashNode(idPedido, ped);

        // Si no hay colision, es el primer elemento
        if (tabla[indice] == nullptr) {
            tabla[indice] = nuevoNodo;
        } else {
            // Manejo de colision: Enlazar al inicio de la lista
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
                // Si el nodo a eliminar es el primero (cabeza de la lista)
                if (anterior == nullptr) {
                    tabla[indice] = actual->next;
                } else {
                    // Si el nodo est- en medio o al final
                    anterior->next = actual->next;
                }

                delete actual; // Liberar memoria del puntero
                cout << "idPedido " << idPedido << " eliminada." << endl;
                return;
            }

            anterior = actual;
            actual = actual->next;
        }

        cout << "idPedido no encontrada para eliminar." << endl;
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
                delete temporal; // Liberar cada nodo
            }
        }
        delete[] tabla; // Liberar el arreglo din-mico de punteros
    }

    void rehash(int nuevaCapacidad) {
        if (nuevaCapacidad <= capacidad) return; // Solo redimensionar si la nueva capacidad es mayor

        // Guardar la referencia a la tabla actual y su capacidad
        HashNode** tablaVieja = tabla;
        int capacidadVieja = capacidad;

        // Actualizar la capacidad e inicializar la nueva tabla con nullptr
        capacidad = nuevaCapacidad;
        tabla = new HashNode*[capacidad];
        for (int i = 0; i < capacidad; i++) {
            tabla[i] = nullptr;
        }

        // Reinsertar todos los nodos existentes en la nueva tabla
        for (int i = 0; i < capacidadVieja; i++) {
            HashNode* actual = tablaVieja[i];
            while (actual != nullptr) {
                HashNode* siguiente = actual->next; // Guardar puntero al siguiente elemento

                // Recalcular el índice con la nueva capacidad
                int nuevoIndice = funcionHash(actual->key);

                // Insertar al inicio de la lista en la nueva tabla
                actual->next = tabla[nuevoIndice];
                tabla[nuevoIndice] = actual;

                actual = siguiente;
            }
        }

        // Liberar el arreglo de punteros antiguo (sin borrar los nodos ya resituados)
        delete[] tablaVieja;
    }
    
    // Libera los Pedido guardados (los nodos los libera el destructor)
    void liberarPedidos() {
        for (int i = 0; i < capacidad; i++) {
            HashNode* actual = tabla[i];
            while (actual != nullptr) {
                delete actual->valor;
                actual->valor = nullptr;
                actual = actual->next;
            }
        }
    }

    // Colisiones = elementos que quedaron detras del primero de su bucket
    int getColisiones() {
        int colisiones = 0;
        for (int i = 0; i < capacidad; i++) {
            HashNode* actual = tabla[i];
            if (actual == nullptr) continue;
            actual = actual->next;
            while (actual != nullptr) {
                colisiones++;
                actual = actual->next;
            }
        }
        return colisiones;
    }

};