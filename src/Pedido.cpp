#include <iostream>
#include <string>

#pragma once  /* para que no se repitan*/
#include "Utilidades.cpp"

using namespace std;
 
// ============================================================
//  Pedido.cpp  -  depende de: Utilidades
//  Todas las estructuras guardan Pedido* (un solo objeto por pedido).
//  NO modificar este struct sin avisar.
// ============================================================
 
struct Pedido {
    string    idPedido;
    string    dniCliente;
    string    tipoEnvio;
    int       nivelPrioridad;
    double    montoDolares;
    string    fecha;
    string    hora;
    string    estado;
    long long claveFecha;
};
 
Pedido* crearPedido(string id, string dni, string tipo, int prioridad, double monto, string fecha, string hora, string estado) {
    Pedido* p = new Pedido;
    p->idPedido        = id;
    p->dniCliente      = dni;
    p->tipoEnvio       = tipo;
    p->nivelPrioridad  = prioridad;
    p->montoDolares    = monto;
    p->fecha           = fecha;
    p->hora            = hora;
    p->estado          = estado;
    p->claveFecha      = calcularClaveFecha(fecha, hora);
    return p;
}
 
Pedido* crearPedidoDesdeCampos(string campos[]) {
    int prioridad = textoAEntero(campos[3]);
    double monto  = textoADecimal(campos[4]);
    return crearPedido(campos[0], campos[1], campos[2], prioridad, monto,
                       campos[5], campos[6], campos[7]);
}
 
void mostrarPedido(const Pedido* p) {
    cout << p->idPedido << " " << p->dniCliente << " " << p->tipoEnvio << " "
         << p->nivelPrioridad << " " << p->montoDolares << " "
         << p->fecha << " " << p->hora << " " << p->estado
         << endl;   
}
 
int compararPedidos(const Pedido* a, const Pedido* b) {
    if (a->claveFecha < b->claveFecha) return -1;
    if (a->claveFecha > b->claveFecha) return 1;
    if (a->idPedido < b->idPedido) return -1;
    if (a->idPedido > b->idPedido) return 1;
    return 0;
}
 
bool mayorPrioridad(const Pedido* a, const Pedido* b) {
    return a->nivelPrioridad > b->nivelPrioridad;
}