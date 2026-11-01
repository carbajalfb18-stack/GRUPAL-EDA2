#include <iostream>
#include <string>
using namespace std;

#include "Utilidades.cpp"
#include "Pedido.cpp"
#include "HashTable.cpp"
#include "MaxHeap.cpp"
#include "RedBlackTree.cpp"
#pragma once 
 
// ============================================================
//  Sistema.cpp  -  depende de: HashTable, MaxHeap, RedBlackTree, Utilidades
//  Es el ÚNICO que conoce las 3 estructuras.
//  Dueño de los Pedido (único que hace delete, junto con el heap al
//  descartar cancelados) y de los contadores.
// ============================================================
 
class Sistema {
private:
    HashTable     hash;
    MaxHeap       heap;
    ArbolRedBlack arbol;
    int           totalPedidos;
    double        montoAcumulado;
 
public:
    Sistema() : hash(20011), heap(1024) {
        // TODO: inicializar totalPedidos y montoAcumulado en 0
    }
 
    ~Sistema() {
        // TODO: liberar los Pedido que sigan en el hash
        //       (los cancelados los libera el heap al extraerlos)
    }
 
    int cargarCSV(const string& ruta) {
        // TODO
        return 0;
    }
 
    bool registrarPedido(Pedido* p) {
        // TODO
        return false;
    }
 
    void buscarPorId(const string& id) {
        // TODO
    }
 
    void despacharLote(int n) {
        // TODO
    }
 
    void consultaOrdenada() {
        // TODO
    }
 
    void consultaRango(const string& fechaIni, const string& fechaFin) {
        // TODO
    }
 
    bool actualizarEstado(const string& id, const string& nuevoEstado) {
        // TODO
        return false;
    }
 
    bool cancelarPedido(const string& id) {
        // TODO
        return false;
    }
 
    void mostrarEstadisticas() {
        // TODO
    }
 
    int getColisiones() {
        // TODO
        return 0;
    }
};