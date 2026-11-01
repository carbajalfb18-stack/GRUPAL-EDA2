#include <iostream>
#include <string>
#include "Pedido.cpp"
using namespace std;
 
// ============================================================
//  RedBlackTree.cpp  -  depende de: Pedido
//  Basado en el ArbolRedBlack del profe (25 ejercicios)
//  Orden: compararPedidos (claveFecha, y si empatan idPedido)
//  Numeración "Ej N" = número del ejercicio en la hoja del profe
// ============================================================
 
enum Color { ROJO, NEGRO };
 
struct NodoRB {
    Pedido* dato;
    Color   color;
    NodoRB *izq, *der, *padre;
    NodoRB(Pedido* d) : dato(d), color(ROJO), izq(nullptr), der(nullptr), padre(nullptr) {}
};
 
class ArbolRedBlack {
private:
    NodoRB* raiz;
    NodoRB* TNULL;
 
    Color   obtenerColor(NodoRB* n)  { /* TODO */ return NEGRO; }
    NodoRB* obtenerPadre(NodoRB* n)  { /* TODO */ return nullptr; }
    NodoRB* obtenerAbuelo(NodoRB* n) { /* TODO */ return nullptr; }
    NodoRB* obtenerHermano(NodoRB* n){ /* TODO */ return nullptr; }
    NodoRB* obtenerTio(NodoRB* n)    { /* TODO */ return nullptr; }
 
    NodoRB* minimo(NodoRB* n) { /* TODO */ return nullptr; }
    NodoRB* maximo(NodoRB* n) { /* TODO */ return nullptr; }
 
    void rotacionIzquierda(NodoRB* x)         { /* TODO */ }
    void rotacionDerecha(NodoRB* x)           { /* TODO */ }
    void transplantar(NodoRB* u, NodoRB* v)   { /* TODO */ }
 
    NodoRB* insertarBST(Pedido* p)            { /* TODO */ return nullptr; }
    void    repararInsercion(NodoRB* k)       { /* TODO */ }
 
    void repararEliminacion(NodoRB* x)        { /* TODO */ }
    void eliminarNodoHelper(NodoRB* nodo, Pedido* p) { /* TODO */ }
 
    void inordenHelper(NodoRB* n, int limite, int& contador) { /* TODO */ }
    int  contarNodos(NodoRB* n)               { /* TODO */ return 0; }
    int  contarRojos(NodoRB* n)               { /* TODO */ return 0; }
    int  contarNegros(NodoRB* n)              { /* TODO */ return 0; }
 
    int  validarAlturaNegra(NodoRB* n)        { /* TODO */ return 0; }
 
    void destruirArbol(NodoRB* n)             { /* TODO */ }
 
    void rangoHelper(NodoRB* n, long long ini, long long fin, int& cantidad, double& monto) { /* TODO */ }
 
public:
    ArbolRedBlack() {
        // TODO: crear TNULL (color NEGRO) y raiz = TNULL
    }
 
    ~ArbolRedBlack() {
        // TODO: destruirArbol(raiz) y delete TNULL
    }
 
    void insertar(Pedido* p) {
        // TODO
    }
 
    void eliminar(Pedido* p) {
        // TODO
    }
 
    NodoRB* buscar(Pedido* p) {
        // TODO
        return nullptr;
    }
 
    bool esValidoRB() {
        // TODO
        return false;
    }
 
    void inOrder(int limite) {
        // TODO
    }
 
    void consultaRango(long long ini, long long fin, int& cantidad, double& montoTotal) {
        // TODO
    }
 
    int getCantidad() {
        // TODO
        return 0;
    }
};