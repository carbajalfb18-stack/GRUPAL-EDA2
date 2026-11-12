#include <iostream>
#include <string>
#include <cstddef>
#include <limits>
#include <stdexcept>

#include "Pedido.cpp"
#pragma once

using namespace std;
 
// ============================================================
//  MaxHeap.cpp  -  depende de: Pedido
//  Basado en el heap con ARREGLO del profe (arr, cantidadNodos)
//  Max-Heap por nivelPrioridad (usa mayorPrioridad de Pedido.cpp)
// ============================================================
 
class MaxHeap {
private:
    Pedido** arr;
    int cantidadNodos;
    int capacidad;
 
    MaxHeap(const MaxHeap&) = delete;
    MaxHeap& operator=(const MaxHeap&) = delete;

    int padre(int i)   { return (i - 1) / 2; }
    int hijoIzq(int i) { return 2 * i + 1; }
    int hijoDer(int i) { return 2 * i + 2; }
 
    void siftUpMax(int i) {
        while (i > 0) {
            int superior = padre(i);
            if (!mayorPrioridad(arr[i], arr[superior])) break;
            Pedido* temporal = arr[i];
            arr[i] = arr[superior];
            arr[superior] = temporal;
            i = superior;
        }
    }
 
    void siftDownMax(int i) {
        // Solo calcular hijos para nodos internos: evita indices desbordados.
        while (i < cantidadNodos / 2) {
            int mayor = hijoIzq(i);
            int derecho = hijoDer(i);
            if (derecho < cantidadNodos && mayorPrioridad(arr[derecho], arr[mayor])) {
                mayor = derecho;
            }
            if (!mayorPrioridad(arr[mayor], arr[i])) break;
            Pedido* temporal = arr[i];
            arr[i] = arr[mayor];
            arr[mayor] = temporal;
            i = mayor;
        }
    }
 
    void redimensionar() {
        const size_t limiteBytes = numeric_limits<size_t>::max() / sizeof(Pedido*);
        const int limite = limiteBytes < static_cast<size_t>(numeric_limits<int>::max())
                         ? static_cast<int>(limiteBytes) : numeric_limits<int>::max();
        if (capacidad >= limite) {
            throw length_error("MaxHeap: capacidad maxima alcanzada");
        }
        int nuevaCapacidad = capacidad > limite / 2 ? limite : capacidad * 2;
        Pedido** nuevo = new Pedido*[nuevaCapacidad];
        for (int i = 0; i < cantidadNodos; ++i) {
            nuevo[i] = arr[i];
        }
        delete[] arr;
        arr = nuevo;
        capacidad = nuevaCapacidad;
    }
 
public:
    MaxHeap(int capacidadInicial) {
        cantidadNodos = 0;
        capacidad = capacidadInicial > 0 ? capacidadInicial : 1;
        if (static_cast<size_t>(capacidad) > numeric_limits<size_t>::max() / sizeof(Pedido*)) {
            throw length_error("MaxHeap: capacidad inicial demasiado grande");
        }
        arr = new Pedido*[capacidad];
    }
 
    ~MaxHeap() {
        delete[] arr;
    }
 
    void insertar(Pedido* p) {
        if (p == nullptr) return;
        // Cada Pedido debe insertarse una sola vez mientras permanezca en el heap.
        if (cantidadNodos == capacidad) {
            redimensionar();
        }
        arr[cantidadNodos++] = p;
        siftUpMax(cantidadNodos - 1);
    }
 
    Pedido* extraerMax() {
        while (cantidadNodos > 0) {
            Pedido* maximo = arr[0];
            --cantidadNodos;
            if (cantidadNodos > 0) {
                arr[0] = arr[cantidadNodos];
            }
            arr[cantidadNodos] = nullptr;
            if (cantidadNodos > 0) siftDownMax(0);

            if (maximo->estado != ESTADO_CANCELADO) return maximo;
            delete maximo;
        }
        return nullptr;
    }
 
    Pedido* verMax() {
        // Consulta fisica sin efectos secundarios: la raiz podria estar cancelada.
        return cantidadNodos > 0 ? arr[0] : nullptr;
    }
 
    int getTamano() {
        // Incluye cancelados que todavia no fueron descartados por extraerMax.
        return cantidadNodos;
    }
 
    bool estaVacio() {
        return cantidadNodos == 0;
    }
};
