#include <iostream>

using namespace std;

enum Color { ROJO, NEGRO };

struct NodoRB {
    int dato;
    Color color;
    NodoRB *izq, *der, *padre;

    NodoRB(int d) : dato(d), color(ROJO), izq(nullptr), der(nullptr), padre(nullptr) {}
};

class ArbolRedBlack {
private:
    NodoRB* raiz;
    NodoRB* TNULL; // Centinela (hojas negras)

    // Ej 1-5: Navegación
    Color obtenerColor(NodoRB* n) {
        return (n == nullptr || n == TNULL) ? NEGRO : n->color;
    }

    NodoRB* obtenerPadre(NodoRB* n) {
        return (n != nullptr) ? n->padre : nullptr;
    }

    NodoRB* obtenerAbuelo(NodoRB* n) {
        NodoRB* p = obtenerPadre(n);
        return (p != nullptr) ? obtenerPadre(p) : nullptr;
    }

    NodoRB* obtenerHermano(NodoRB* n) {
        NodoRB* p = obtenerPadre(n);
        if (p == nullptr) return nullptr;
        return (n == p->izq) ? p->der : p->izq;
    }

    NodoRB* obtenerTio(NodoRB* n) {
        NodoRB* p = obtenerPadre(n);
        return obtenerHermano(p);
    }

    // Ej 6-7: Min y Max
    NodoRB* minimo(NodoRB* n) {
        while (n->izq != TNULL) n = n->izq;
        return n;
    }

    NodoRB* maximo(NodoRB* n) {
        while (n->der != TNULL) n = n->der;
        return n;
    }

    // Ej 8: Rotación Izquierda
    void rotacionIzquierda(NodoRB* x) {
        NodoRB* y = x->der;
        x->der = y->izq;
        if (y->izq != TNULL) y->izq->padre = x;
        y->padre = x->padre;
        if (x->padre == nullptr) raiz = y;
        else if (x == x->padre->izq) x->padre->izq = y;
        else x->padre->der = y;
        y->izq = x;
        x->padre = y;
    }

    // Ej 9: Rotación Derecha
    void rotacionDerecha(NodoRB* x) {
        NodoRB* y = x->izq;
        x->izq = y->der;
        if (y->der != TNULL) y->der->padre = x;
        y->padre = x->padre;
        if (x->padre == nullptr) raiz = y;
        else if (x == x->padre->der) x->padre->der = y;
        else x->padre->izq = y;
        y->der = x;
        x->padre = y;
    }

    // Ej 10: Transplantar
    void transplantar(NodoRB* u, NodoRB* v) {
        if (u->padre == nullptr) raiz = v;
        else if (u == u->padre->izq) u->padre->izq = v;
        else u->padre->der = v;
        v->padre = u->padre;
    }

    // Ej 11: Inserción BST normal
    NodoRB* insertarBST(int llave) {
        NodoRB* nodo = new NodoRB(llave);
        nodo->izq = TNULL;
        nodo->der = TNULL;
        NodoRB* y = nullptr;
        NodoRB* x = raiz;

        while (x != TNULL) {
            y = x;
            if (nodo->dato < x->dato) x = x->izq;
            else x = x->der;
        }

        nodo->padre = y;
        if (y == nullptr) raiz = nodo;
        else if (nodo->dato < y->dato) y->izq = nodo;
        else y->der = nodo;

        return nodo;
    }

    // Ej 12: Reparar Inserción
    void repararInsercion(NodoRB* k) {
        NodoRB* u;
        //cout<<"debug";
        while (k->padre != nullptr && k->padre->color == ROJO) {
            if (k->padre == k->padre->padre->der) {
                u = k->padre->padre->izq; // Tío
                if (u->color == ROJO) {
                    u->color = NEGRO;
                    k->padre->color = NEGRO;
                    k->padre->padre->color = ROJO;
                    k = k->padre->padre;
                } else {
                    if (k == k->padre->izq) {
                        k = k->padre;
                        rotacionDerecha(k);
                    }
                    k->padre->color = NEGRO;
                    k->padre->padre->color = ROJO;
                    rotacionIzquierda(k->padre->padre);
                }

            } else { // Simétrico
                u = k->padre->padre->der; // Tío
                if (u->color == ROJO) {
                    u->color = NEGRO;
                    k->padre->color = NEGRO;
                    k->padre->padre->color = ROJO;
                    k = k->padre->padre;
                } else {
                    if (k == k->padre->der) {
                        k = k->padre;
                        rotacionIzquierda(k);
                    }
                    k->padre->color = NEGRO;
                    k->padre->padre->color = ROJO;
                    rotacionDerecha(k->padre->padre);
                }cout<<"debug";
            }

            if (k == raiz) break;

        }
        //cout<<"debug";

        raiz->color = NEGRO;
    }

    // Ej 14: Reparar Eliminación
    void repararEliminacion(NodoRB* x) {
        NodoRB* s;
        while (x != raiz && x->color == NEGRO) {
            if (x == x->padre->izq) {
                s = x->padre->der;
                if (s->color == ROJO) {
                    s->color = NEGRO;
                    x->padre->color = ROJO;
                    rotacionIzquierda(x->padre);
                    s = x->padre->der;
                }

                if (s->izq->color == NEGRO && s->der->color == NEGRO) {
                    s->color = ROJO;
                    x = x->padre;
                } else {
                    if (s->der->color == NEGRO) {
                        s->izq->color = NEGRO;
                        s->color = ROJO;
                        rotacionDerecha(s);
                        s = x->padre->der;
                    }
                    s->color = x->padre->color;
                    x->padre->color = NEGRO;
                    s->der->color = NEGRO;
                    rotacionIzquierda(x->padre);
                    x = raiz;
                }
            } else { // Simétrico
                s = x->padre->izq;
                if (s->color == ROJO) {
                    s->color = NEGRO;
                    x->padre->color = ROJO;
                    rotacionDerecha(x->padre);
                    s = x->padre->izq;
                }

                if (s->izq->color == NEGRO && s->der->color == NEGRO) {
                    s->color = ROJO;
                    x = x->padre;
                } else {
                    if (s->izq->color == NEGRO) {
                        s->der->color = NEGRO;
                        s->color = ROJO;
                        rotacionIzquierda(s);
                        s = x->padre->izq;
                    }
                    s->color = x->padre->color;
                    x->padre->color = NEGRO;
                    s->izq->color = NEGRO;
                    rotacionDerecha(x->padre);
                    x = raiz;
                }
            }
        }
        x->color = NEGRO;
    }

    // Ej 15: Eliminar (Núcleo Lógico)
    void eliminarNodoHelper(NodoRB* nodo, int llave) {
        NodoRB* z = TNULL;
        NodoRB* x;
        NodoRB* y;

        while (nodo != TNULL) {
            if (nodo->dato == llave) z = nodo;
            if (nodo->dato <= llave) nodo = nodo->der;
            else nodo = nodo->izq;
        }

        if (z == TNULL) return;

        y = z;
        Color y_color_original = y->color;

        if (z->izq == TNULL) {
            x = z->der;
            transplantar(z, z->der);
        } else if (z->der == TNULL) {
            x = z->izq;
            transplantar(z, z->izq);
        } else {
            y = minimo(z->der);
            y_color_original = y->color;
            x = y->der;
            if (y->padre == z) {
                x->padre = y;
            } else {
                transplantar(y, y->der);
                y->der = z->der;
                y->der->padre = y;
            }
            transplantar(z, y);
            y->izq = z->izq;
            y->izq->padre = y;
            y->color = z->color;
        }
        delete z;
        if (y_color_original == NEGRO) repararEliminacion(x);
    }

    // Ej 18: Inorden helper
    void inordenHelper(NodoRB* n) {
        if (n != TNULL) {
            inordenHelper(n->izq);
            cout << n->dato << " ";
            inordenHelper(n->der);
        }
    }

    // Ej 19: Contar nodos
    int contarNodos(NodoRB* n) {
        if (n == TNULL) return 0;
        return 1 + contarNodos(n->izq) + contarNodos(n->der);
    }

    // Ej 20: Contar Rojos
    int contarRojos(NodoRB* n) {
        if (n == TNULL) return 0;
        int cont = (n->color == ROJO) ? 1 : 0;
        return cont + contarRojos(n->izq) + contarRojos(n->der);
    }

    // Ej 21: Contar Negros
    int contarNegros(NodoRB* n) {
        if (n == TNULL) return 0;
        int cont = (n->color == NEGRO) ? 1 : 0;
        return cont + contarNegros(n->izq) + contarNegros(n->der);
    }

    // Ej 22: Altura Negra y Validación
    int validarAlturaNegra(NodoRB* n) {
        if (n == TNULL) return 1;

        if (n->color == ROJO && (n->izq->color == ROJO || n->der->color == ROJO)) {
            return -1;
        }

        int izqH = validarAlturaNegra(n->izq);
        int derH = validarAlturaNegra(n->der);

        if (izqH == -1 || derH == -1 || izqH != derH) return -1;

        return izqH + (n->color == NEGRO ? 1 : 0);
    }

    // Ej 24: Sumar valores
    int sumarValores(NodoRB* n) {
        if (n == TNULL) return 0;
        return n->dato + sumarValores(n->izq) + sumarValores(n->der);
    }

    // Ej 25: Destruir
    void destruirArbol(NodoRB* n) {
        if (n != TNULL) {
            destruirArbol(n->izq);
            destruirArbol(n->der);
            delete n;
        }
    }

public:
    ArbolRedBlack() {
        TNULL = new NodoRB(0);
        TNULL->color = NEGRO;
        raiz = TNULL;
    }

    ~ArbolRedBlack() {
        destruirArbol(raiz);
        delete TNULL;
    }

    // Ej 13
    void insertar(int llave) {
        NodoRB* nuevo = insertarBST(llave);
        repararInsercion(nuevo);
    }

    // Ej 16
    void eliminar(int llave) {
        eliminarNodoHelper(raiz, llave);
    }

    // Ej 17
    NodoRB* buscar(int llave) {
        NodoRB* tmp = raiz;
        while (tmp != TNULL) {
            if (tmp->dato == llave) return tmp;
            if (tmp->dato > llave) tmp = tmp->izq;
            else tmp = tmp->der;
        }
        return TNULL; // No encontrado
    }

    void imprimirInorden() {
        inordenHelper(raiz);
        cout << endl;
    }

    // Ej 23
    bool esValidoRB() {
        if (raiz == TNULL) return true;
        if (raiz->color != NEGRO) return false;
        return validarAlturaNegra(raiz) != -1;
    }
};

int main()
{

    ArbolRedBlack* arbol = new ArbolRedBlack();
    arbol->insertar(4);
    arbol->insertar(10);
    arbol->insertar(9);
    arbol->insertar(45);
    arbol->insertar(22);
    cout << "Hello world!" << endl;
    arbol->imprimirInorden();


    return 0;
}
