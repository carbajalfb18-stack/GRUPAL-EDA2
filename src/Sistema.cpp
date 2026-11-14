#include <iostream>
#include <string>
#include <fstream> // para leer el archivo csv
using namespace std;

#include "Utilidades.cpp"
#include "Pedido.cpp"
#include "HashTable.cpp"
#include "MaxHeap.cpp"
#include "RedBlackTree.cpp"
#pragma once 
 
// ============================================================
//  Sistema.cpp
//  Junta el hash, el heap y el árbol. Es el dueño de los pedidos:
//  solo aquí (y el heap, al sacar los cancelados) se hace delete.
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
        totalPedidos = 0;
        montoAcumulado = 0;
    }
 
    ~Sistema() {
        hash.liberarPedidos();
        // los cancelados los libera el heap cuando los saca
    }
 
    int cargarCSV(const string& ruta) {
        ifstream archivo;  // para leer el archivo línea por línea
        archivo.open(ruta.c_str());  // c_str() solo pasa la ruta al formato que pide open()
        if (!archivo.is_open()) {
            cout << "No se pudo abrir el archivo: " << ruta << endl;
            return 0;
        }
        string linea;
        string campos[8];
        int cargados = 0;
 
        while (getline(archivo, linea)) {
            int n = dividirLinea(linea, campos, 8);
            if (n == 8) {
                Pedido* p = crearPedidoDesdeCampos(campos);
                if (registrarPedido(p)) {
                    cargados++;
                }
            }
        }
        archivo.close();
        return cargados;
    }
 
    bool registrarPedido(Pedido* p) {
        
        if(!esFechaValida(p->fecha)) {
            delete p;  
            return false;
        }

        if(!hash.insertar(p->idPedido, p)) {
            delete p;
            return false;
        }
        heap.insertar(p);
        arbol.insertar(p);
        totalPedidos++;
        montoAcumulado += p->montoDolares;

        return true;
    }
 
    void buscarPorId(const string& id) {
        Pedido* p = hash.buscar(id);
        if(p == nullptr){
            cout << "Pedido no encontrado" << endl;
        } else {
            cout << "Pedido ID: " << p->idPedido << endl
                 << "DNI: " << p->dniCliente << endl
                 << "Tipo Envio: " << p->tipoEnvio << endl
                 << "Prioridad: " << p->nivelPrioridad << endl
                 << "Monto: " << p->montoDolares << endl
                 << "Fecha: " << p->fecha << endl
                 << "Hora: " << p->hora << endl
                 << "Estado: " << p->estado << endl;
        }
    }
 
    void despacharLote(int n) {
        for(int i = 0; i < n; i++) {
            Pedido* p = heap.extraerMax();

            if(p == nullptr) {
                cout << "No hay más pedidos para despachar" << endl;
                return;
            }
            
            p->estado = ESTADO_TRANSITO;
            mostrarPedido(p);
      
        }
    }
 
    void consultaOrdenada() {
        arbol.inOrder(30);  // muestra solo los primeros 30 (con 0 saldrían todos)
    }
 
    void consultaRango(const string& fechaIni, const string& fechaFin) {
        int cantidad = 0;
        double montoTotal = 0.0;

        calcularRango(fechaIni, fechaFin, cantidad, montoTotal);
        cout << "Cantidad de pedidos: " << cantidad << endl;

        cout << "Monto total: " << montoTotal << endl;
    }
 
    bool actualizarEstado(const string& id, const string& nuevoEstado) {
        Pedido* p = hash.buscar(id);

        if(p == nullptr) {
            cout << "Pedido no encontrado" << endl;
            return false;
        }
        p->estado = nuevoEstado;
        return true;
    }
 
    bool cancelarPedido(const string& id) {
        Pedido* p = hash.buscar(id);
        if(p == nullptr) {
            cout << "Pedido no encontrado" << endl;
            return false;
        }
        hash.eliminar(id);
        arbol.eliminar(p);

        p->estado = ESTADO_CANCELADO;
        totalPedidos--;
        montoAcumulado -= p->montoDolares;  

        return true;
    }
 
    void mostrarEstadisticas() {
        cout << "Total de pedidos: " << totalPedidos << endl;
        cout << "Pendientes por despachar: " << heap.getTamano() << endl;
        cout << "Monto total acumulado: " << montoAcumulado << endl;
    }
 
    // Versiones "silenciosas": no imprimen nada, sirven para las pruebas de tiempo
    Pedido* obtenerPedido(const string& id) {
        return hash.buscar(id);
    }

    void calcularRango(const string& fechaIni, const string& fechaFin, int& cantidad, double& montoTotal) {
        cantidad = 0;
        montoTotal = 0.0;
        arbol.consultaRango(claveInicioDia(fechaIni), claveFinDia(fechaFin), cantidad, montoTotal);
    }

    int despacharSinMostrar(int n) {
        int despachados = 0;
        while (despachados < n) {
            Pedido* p = heap.extraerMax();
            if (p == nullptr) {
                return despachados;
            }
            p->estado = ESTADO_TRANSITO;
            despachados++;
        }
        return despachados;
    }

    int getColisiones() {
        return hash.getColisiones();
    }
};