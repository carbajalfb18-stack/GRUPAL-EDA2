#include <iostream>
#include <string>
#include <vector>   

#include "Sistema.cpp"


using namespace std;

// ============================================================
//  main.cpp  -  depende de: Sistema
//  Compilar SOLO este archivo:  g++ main.cpp -o sistema
// ============================================================
 
void mostrarMenu() {
    // TODO: opciones 1 a 9 + pruebas + salir
}
 
void ingresarPedidoManual(Sistema& s) {
    // TODO
}
 
void pruebaCargaMasiva(Sistema& s)   { /* TODO: tiempo de cargarCSV */ }
void pruebaBusqueda(Sistema& s)      { /* TODO: primero, último e inventado */ }
void pruebaDespacho(Sistema& s)      { /* TODO: 10 más urgentes */ }
void pruebaRango(Sistema& s)         { /* TODO: una semana, un mes, rango vacío */ }
void pruebaInsercion(Sistema& s)     { /* TODO: insertar y verificar en hash y árbol */ }
void pruebaActualizacion(Sistema& s) { /* TODO: 10 pedidos */ }
void pruebaCancelacion(Sistema& s)   { /* TODO: 10 pedidos y verificar con el hash */ }
 
int main() {
    Sistema sistema;
    // TODO: bucle del menú
    return 0;
}
 