#include <iostream>
#include <string>
#include "Utilidades.cpp"

using namespace std;
 
// ============================================================
//  Pedido.cpp  -  depende de: Utilidades
//  Todas las estructuras guardan Pedido* (un solo objeto por pedido).
//  NO modificar este struct sin avisar a los 3.
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
    // TODO
    return nullptr;
}
 
Pedido* crearPedidoDesdeCampos(string campos[]) {
    // TODO
    return nullptr;
}
 
void mostrarPedido(const Pedido* p) {
    // TODO
}
 
int compararPedidos(const Pedido* a, const Pedido* b) {
    // TODO
    return 0;
}
 
bool mayorPrioridad(const Pedido* a, const Pedido* b) {
    // TODO
    return false;
}