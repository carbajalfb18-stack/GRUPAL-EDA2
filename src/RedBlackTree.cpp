#pragma once
#include <iostream>
#include <string>
#include "Pedido.cpp"

using namespace std;

enum Color { ROJO, NEGRO };

struct NodoRB {
    Pedido* dato;
    Color color;
    NodoRB *izq, *der, *padre;

    NodoRB(Pedido* d)
        : dato(d), color(ROJO),
          izq(nullptr), der(nullptr), padre(nullptr) {}
};

class ArbolRedBlack {
private:
    NodoRB* raiz;
    NodoRB* TNULL;

    ArbolRedBlack(const ArbolRedBlack&) = delete;
    ArbolRedBlack& operator=(const ArbolRedBlack&) = delete;

    Color obtenerColor(NodoRB* n) {
        return (n == nullptr || n == TNULL) ? NEGRO : n->color;
    }

    NodoRB* obtenerPadre(NodoRB* n) {
        return (n == nullptr || n == TNULL) ? TNULL : n->padre;
    }

    NodoRB* obtenerAbuelo(NodoRB* n) {
        return obtenerPadre(obtenerPadre(n));
    }

    NodoRB* obtenerHermano(NodoRB* n) {
        NodoRB* p = obtenerPadre(n);
        if (p == TNULL) return TNULL;
        return (n == p->izq) ? p->der : p->izq;
    }

    NodoRB* obtenerTio(NodoRB* n) {
        return obtenerHermano(obtenerPadre(n));
    }

    NodoRB* minimo(NodoRB* n) {
        if (n == nullptr) return TNULL;
        while (n != TNULL && n->izq != TNULL)
            n = n->izq;
        return n;
    }

    NodoRB* maximo(NodoRB* n) {
        if (n == nullptr) return TNULL;
        while (n != TNULL && n->der != TNULL)
            n = n->der;
        return n;
    }

    void rotacionIzquierda(NodoRB* x) {
        if (x == nullptr || x == TNULL || x->der == TNULL)
            return;

        NodoRB* y = x->der;
        x->der = y->izq;

        if (y->izq != TNULL)
            y->izq->padre = x;

        y->padre = x->padre;

        if (x->padre == TNULL)
            raiz = y;
        else if (x == x->padre->izq)
            x->padre->izq = y;
        else
            x->padre->der = y;

        y->izq = x;
        x->padre = y;
    }

    void rotacionDerecha(NodoRB* x) {
        if (x == nullptr || x == TNULL || x->izq == TNULL)
            return;

        NodoRB* y = x->izq;
        x->izq = y->der;

        if (y->der != TNULL)
            y->der->padre = x;

        y->padre = x->padre;

        if (x->padre == TNULL)
            raiz = y;
        else if (x == x->padre->der)
            x->padre->der = y;
        else
            x->padre->izq = y;

        y->der = x;
        x->padre = y;
    }

    void transplantar(NodoRB* u, NodoRB* v) {
        if (u->padre == TNULL)
            raiz = v;
        else if (u == u->padre->izq)
            u->padre->izq = v;
        else
            u->padre->der = v;

        v->padre = u->padre;
    }

    NodoRB* insertarBST(Pedido* p) {
        if (p == nullptr) return nullptr;

        NodoRB* padre = TNULL;
        NodoRB* actual = raiz;
        int orden = 0;

        while (actual != TNULL) {
            padre = actual;
            orden = compararPedidos(p, actual->dato);

            if (orden == 0)
                return nullptr;

            if (orden < 0)
                actual = actual->izq;
            else
                actual = actual->der;
        }

        NodoRB* nuevo = new NodoRB(p);
        nuevo->padre = padre;
        nuevo->izq = TNULL;
        nuevo->der = TNULL;

        if (padre == TNULL)
            raiz = nuevo;
        else if (orden < 0)
            padre->izq = nuevo;
        else
            padre->der = nuevo;

        return nuevo;
    }

    void repararInsercion(NodoRB* k) {
        while (obtenerColor(obtenerPadre(k)) == ROJO) {
            NodoRB* abuelo = obtenerAbuelo(k);
            NodoRB* tio = obtenerTio(k);

            if (obtenerColor(tio) == ROJO) {
                k->padre->color = NEGRO;
                tio->color = NEGRO;
                abuelo->color = ROJO;
                k = abuelo;
            }
            else if (k->padre == abuelo->izq) {
                if (k == k->padre->der) {
                    k = k->padre;
                    rotacionIzquierda(k);
                }

                k->padre->color = NEGRO;
                k->padre->padre->color = ROJO;
                rotacionDerecha(k->padre->padre);
            }
            else {
                if (k == k->padre->izq) {
                    k = k->padre;
                    rotacionDerecha(k);
                }

                k->padre->color = NEGRO;
                k->padre->padre->color = ROJO;
                rotacionIzquierda(k->padre->padre);
            }
        }

        raiz->color = NEGRO;
        raiz->padre = TNULL;
    }

    void repararEliminacion(NodoRB* x) {
        while (x != raiz && obtenerColor(x) == NEGRO) {
            if (x == x->padre->izq) {
                NodoRB* hermano = x->padre->der;

                if (obtenerColor(hermano) == ROJO) {
                    hermano->color = NEGRO;
                    x->padre->color = ROJO;
                    rotacionIzquierda(x->padre);
                    hermano = x->padre->der;
                }

                if (hermano == TNULL) {
                    x = x->padre;
                    continue;
                }

                if (obtenerColor(hermano->izq) == NEGRO &&
                    obtenerColor(hermano->der) == NEGRO) {
                    hermano->color = ROJO;
                    x = x->padre;
                }
                else {
                    if (obtenerColor(hermano->der) == NEGRO) {
                        hermano->izq->color = NEGRO;
                        hermano->color = ROJO;
                        rotacionDerecha(hermano);
                        hermano = x->padre->der;
                    }

                    hermano->color = x->padre->color;
                    x->padre->color = NEGRO;
                    hermano->der->color = NEGRO;
                    rotacionIzquierda(x->padre);
                    x = raiz;
                }
            }
            else {
                NodoRB* hermano = x->padre->izq;

                if (obtenerColor(hermano) == ROJO) {
                    hermano->color = NEGRO;
                    x->padre->color = ROJO;
                    rotacionDerecha(x->padre);
                    hermano = x->padre->izq;
                }

                if (hermano == TNULL) {
                    x = x->padre;
                    continue;
                }

                if (obtenerColor(hermano->der) == NEGRO &&
                    obtenerColor(hermano->izq) == NEGRO) {
                    hermano->color = ROJO;
                    x = x->padre;
                }
                else {
                    if (obtenerColor(hermano->izq) == NEGRO) {
                        hermano->der->color = NEGRO;
                        hermano->color = ROJO;
                        rotacionIzquierda(hermano);
                        hermano = x->padre->izq;
                    }

                    hermano->color = x->padre->color;
                    x->padre->color = NEGRO;
                    hermano->izq->color = NEGRO;
                    rotacionDerecha(x->padre);
                    x = raiz;
                }
            }
        }

        x->color = NEGRO;
    }

    void eliminarNodoHelper(NodoRB* nodo, Pedido* p) {
        if (p == nullptr) return;

        while (nodo != TNULL) {
            int orden = compararPedidos(p, nodo->dato);

            if (orden == 0)
                break;

            nodo = (orden < 0) ? nodo->izq : nodo->der;
        }

        if (nodo == TNULL) return;

        NodoRB* sucesor = nodo;
        Color colorOriginal = sucesor->color;
        NodoRB* reemplazo;

        if (nodo->izq == TNULL) {
            reemplazo = nodo->der;
            transplantar(nodo, nodo->der);
        }
        else if (nodo->der == TNULL) {
            reemplazo = nodo->izq;
            transplantar(nodo, nodo->izq);
        }
        else {
            sucesor = minimo(nodo->der);
            colorOriginal = sucesor->color;
            reemplazo = sucesor->der;

            if (sucesor->padre == nodo) {
                reemplazo->padre = sucesor;
            }
            else {
                transplantar(sucesor, sucesor->der);
                sucesor->der = nodo->der;
                sucesor->der->padre = sucesor;
            }

            transplantar(nodo, sucesor);
            sucesor->izq = nodo->izq;
            sucesor->izq->padre = sucesor;
            sucesor->color = nodo->color;
        }

        delete nodo;

        if (colorOriginal == NEGRO)
            repararEliminacion(reemplazo);

        if (raiz != TNULL)
            raiz->padre = TNULL;

        TNULL->padre = TNULL;
        TNULL->color = NEGRO;
    }

    void inordenHelper(NodoRB* n, int limite, int& contador) {
        if (n == TNULL || (limite > 0 && contador >= limite))
            return;

        inordenHelper(n->izq, limite, contador);

        if (limite > 0 && contador >= limite)
            return;

        mostrarPedido(n->dato);
        contador++;

        inordenHelper(n->der, limite, contador);
    }

    int contarNodos(NodoRB* n) {
        if (n == TNULL) return 0;

        return 1 + contarNodos(n->izq) + contarNodos(n->der);
    }

    int contarRojos(NodoRB* n) {
        if (n == TNULL) return 0;

        int actual = (n->color == ROJO) ? 1 : 0;
        return actual + contarRojos(n->izq) + contarRojos(n->der);
    }

    int contarNegros(NodoRB* n) {
        if (n == TNULL) return 0;

        int actual = (n->color == NEGRO) ? 1 : 0;
        return actual + contarNegros(n->izq) + contarNegros(n->der);
    }

    int validarAlturaNegra(NodoRB* n) {
        if (n == TNULL) return 1;

        if (n == nullptr)
            return 0;

        if (n->color == ROJO &&
            (obtenerColor(n->izq) == ROJO ||
             obtenerColor(n->der) == ROJO))
            return 0;

        int izquierda = validarAlturaNegra(n->izq);
        int derecha = validarAlturaNegra(n->der);

        if (izquierda == 0 || izquierda != derecha)
            return 0;

        return izquierda + (n->color == NEGRO ? 1 : 0);
    }

    bool validarEstructura(
        NodoRB* n,
        NodoRB* padre,
        const Pedido* inferior,
        const Pedido* superior
    ) {
        if (n == TNULL) return true;

        if (n == nullptr || n->dato == nullptr ||
            n->padre != padre ||
            n->izq == nullptr || n->der == nullptr)
            return false;

        if (inferior != nullptr &&
            compararPedidos(n->dato, inferior) <= 0)
            return false;

        if (superior != nullptr &&
            compararPedidos(n->dato, superior) >= 0)
            return false;

        return validarEstructura(
                   n->izq, n, inferior, n->dato
               ) &&
               validarEstructura(
                   n->der, n, n->dato, superior
               );
    }

    void destruirArbol(NodoRB* n) {
        if (n == TNULL) return;

        destruirArbol(n->izq);
        destruirArbol(n->der);
        delete n;
    }

    void rangoHelper(
        NodoRB* n,
        long long ini,
        long long fin,
        int& cantidad,
        double& monto
    ) {
        if (n == TNULL) return;

        long long clave = n->dato->claveFecha;

        if (clave >= ini)
            rangoHelper(n->izq, ini, fin, cantidad, monto);

        if (clave >= ini && clave <= fin) {
            cantidad++;
            monto += n->dato->montoDolares;
        }

        if (clave <= fin)
            rangoHelper(n->der, ini, fin, cantidad, monto);
    }

public:
    ArbolRedBlack() {
        TNULL = new NodoRB(nullptr);
        TNULL->color = NEGRO;
        TNULL->izq = TNULL;
        TNULL->der = TNULL;
        TNULL->padre = TNULL;
        raiz = TNULL;
    }

    ~ArbolRedBlack() {
        destruirArbol(raiz);
        delete TNULL;
    }

    void insertar(Pedido* p) {
        NodoRB* nuevo = insertarBST(p);

        if (nuevo != nullptr)
            repararInsercion(nuevo);
    }

    void eliminar(Pedido* p) {
        eliminarNodoHelper(raiz, p);
    }

    NodoRB* buscar(Pedido* p) {
        if (p == nullptr) return nullptr;

        NodoRB* actual = raiz;

        while (actual != TNULL) {
            int orden = compararPedidos(p, actual->dato);

            if (orden == 0)
                return actual;

            actual = (orden < 0) ? actual->izq : actual->der;
        }

        return nullptr;
    }

    bool esValidoRB() {
        if (TNULL == nullptr ||
            TNULL->color != NEGRO ||
            TNULL->dato != nullptr ||
            TNULL->izq != TNULL ||
            TNULL->der != TNULL ||
            TNULL->padre != TNULL ||
            raiz == nullptr)
            return false;

        if (raiz == TNULL)
            return true;

        if (raiz->padre != TNULL || raiz->color != NEGRO)
            return false;

        return validarEstructura(
                   raiz, TNULL, nullptr, nullptr
               ) &&
               validarAlturaNegra(raiz) > 0;
    }

    void inOrder(int limite) {
        if (limite < 0) return;

        int contador = 0;
        inordenHelper(raiz, limite, contador);
    }

    void consultaRango(
        long long ini,
        long long fin,
        int& cantidad,
        double& montoTotal
    ) {
        if (ini > fin) return;

        rangoHelper(raiz, ini, fin, cantidad, montoTotal);
    }

    int getCantidad() {
        return contarNodos(raiz);
    }
};