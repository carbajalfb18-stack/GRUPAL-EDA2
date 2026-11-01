#include <iostream>
#include <string>

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
 
    int padre(int i)   { /* TODO */ return 0; }
    int hijoIzq(int i) { /* TODO */ return 0; }
    int hijoDer(int i) { /* TODO */ return 0; }
 
    void siftUpMax(int i) {
        // TODO
    }
 
    void siftDownMax(int i) {
        // TODO
    }
 
    void redimensionar() {
        // TODO
    }
 
public:
    MaxHeap(int capacidadInicial) {
        // TODO
    }
 
    ~MaxHeap() {
        // TODO: delete[] arr, NO los Pedido
    }
 
    void insertar(Pedido* p) {
        // TODO
    }
 
    Pedido* extraerMax() {
        // TODO
        return nullptr;
    }
 
    Pedido* verMax() {
        // TODO
        return nullptr;
    }
 
    int getTamano() {
        // TODO
        return 0;
    }
 
    bool estaVacio() {
        // TODO
        return true;
    }
};