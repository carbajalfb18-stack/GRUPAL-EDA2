#include <iostream>
#include <string>
#include <vector>

#include "Utilidades.cpp"
#include "Pedido.cpp"
#pragma once          /* para que no se repitan cuando terminemos ponemos todo junto y se quita */

using namespace std;
 
// ============================================================
//  HashTable.cpp  -  depende de: Pedido, Utilidades
//  Basado en el hash del profe (HashNode, tabla, capacidad, rehash)
//  Encadenamiento (lista enlazada por bucket)
// ============================================================
 
#include <iostream>
#include <string>
#include "Pedido.cpp"

using namespace std;

// Estructura para los nodos de la lista enlazada
struct NodoHash {
    string idPedido;
    Pedido* ped;
    NodoHash* siguiente; // Puntero al siguiente nodo en caso de colisi-n

    // Constructor del nodo
    NodoHash(string c, Pedido* v) {
        idPedido = c;
        ped = v;
        siguiente = nullptr;
    }
};

class TablaHash {
  NodoHash** tabla;//Puntero doble: Un arreglo de punteros a
  int capacidad;
  int colisiones; // Inserciones acumuladas que encontraron un bucket ocupado.
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
      TablaHash(int cap) {
        capacidad = cap;
        colisiones = 0;
        tabla = new NodoHash*[capacidad];//Crear el arreglo de
        // Inicializar todos los punteros a nullptr (vacios)
        for (int i = 0; i < capacidad; i++) {
          tabla[i] = nullptr;
        }
    }

    void insertar(string idPedido, Pedido* ped) {
        int indice = funcionHash(idPedido);
        NodoHash* nuevoNodo = new NodoHash(idPedido, ped);

        // Si no hay colisi-n, es el primer elemento
        if (tabla[indice] == nullptr) {
            tabla[indice] = nuevoNodo;
        } else {
            ++colisiones;
            // Manejo de colisi-n: Enlazar al inicio de la lista
            nuevoNodo->siguiente = tabla[indice];
            tabla[indice] = nuevoNodo;
        }
    }

    Pedido* buscar(string idPedido){
        int indice =funcionHash(idPedido);
        NodoHash* actual = tabla[indice];
        while (actual != nullptr){
            if (actual->idPedido == idPedido){
                cout <<"Pedido ID: "<<actual->ped->idPedido<<endl
                <<"DNI: "<<actual->ped->dniCliente<<endl
                <<"Tipo Envio: "<<actual->ped->tipoEnvio<<endl
                <<"Prioridad: "<<actual->ped->nivelPrioridad<<endl
                <<"Monto: "<<actual->ped->montoDolares<<endl
                <<"Fecha: "<<actual->ped->fecha<<endl
                <<"Hora: "<<actual->ped->hora<<endl
                <<"Estado: "<<actual->ped->estado<<endl;
                return actual->ped;
            }
            actual=actual->siguiente;
        }
        cout <<"No encontrado"<<endl;
        return nullptr;
    }

    void eliminar(string idPedido) {
        int indice = funcionHash(idPedido);
        NodoHash* actual = tabla[indice];
        NodoHash* anterior = nullptr;

        while (actual != nullptr) {
            if (actual->idPedido == idPedido) {
                // Si el nodo a eliminar es el primero (cabeza de la lista)
                if (anterior == nullptr) {
                    tabla[indice] = actual->siguiente;
                } else {
                    // Si el nodo est- en medio o al final
                    anterior->siguiente = actual->siguiente;
                }

                delete actual; // Liberar memoria del puntero
                cout << "idPedido " << idPedido << " eliminada." << endl;
                return;
            }

            anterior = actual;
            actual = actual->siguiente;
        }

        cout << "idPedido no encontrada para eliminar." << endl;
    }

    int getColisiones() {
        return colisiones;
    }

    int getCantidad(){
        int total = 0;
        for (int i = 0; i < capacidad; i++) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                total++;
                actual = actual->siguiente;
            }
        }
        return total;
    }
    ~TablaHash() {
        for (int i = 0; i < capacidad; i++) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                NodoHash* temporal = actual;
                actual = actual->siguiente;
                delete temporal; // Liberar cada nodo
            }
        }
        delete[] tabla; // Liberar el arreglo din-mico de punteros
    }

    void rehash(int nuevaCapacidad) {
        if (nuevaCapacidad <= capacidad) return; // Solo redimensionar si la nueva capacidad es mayor

        // Guardar la referencia a la tabla actual y su capacidad
        NodoHash** tablaVieja = tabla;
        int capacidadVieja = capacidad;

        // Actualizar la capacidad e inicializar la nueva tabla con nullptr
        capacidad = nuevaCapacidad;
        tabla = new NodoHash*[capacidad];
        for (int i = 0; i < capacidad; i++) {
            tabla[i] = nullptr;
        }

        // Reinsertar todos los nodos existentes en la nueva tabla
        for (int i = 0; i < capacidadVieja; i++) {
            NodoHash* actual = tablaVieja[i];
            while (actual != nullptr) {
                NodoHash* siguiente = actual->siguiente; // Guardar puntero al siguiente elemento

                // Recalcular el índice con la nueva capacidad
                int nuevoIndice = funcionHash(actual->idPedido);

                // Insertar al inicio de la lista en la nueva tabla
                actual->siguiente = tabla[nuevoIndice];
                tabla[nuevoIndice] = actual;

                actual = siguiente;
            }
        }

        // Liberar el arreglo de punteros antiguo (sin borrar los nodos ya resituados)
        delete[] tablaVieja;
    }

};
