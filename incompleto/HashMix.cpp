#include <iostream>
#include <string>

using namespace std;

// Estructura para los nodos de la lista enlazada
struct NodoHash {
    string idPedido;
    Pedido* ped;
    NodoHash* siguiente; // Puntero al siguiente nodo en caso de colisi-n

    // Constructor del nodo
    NodoHash(string c, Pedido* v) {
        idPedido = c;
        ped = v;
        siguiente = nullptr;
    }
};

class TablaHash {
  NodoHash** tabla;//Puntero doble: Un arreglo de punteros a
  int capacidad;
  // Funci-n hash privada
  int funcionHash(string idPedido) {
    int suma = 0;
    for (char c : idPedido) {
      suma += c;
    }
    return suma % capacidad;//Asegura que el -ndice este en r
  }
    public:
    // Constructor
      TablaHash(int cap) {
        capacidad = cap;
        tabla = new NodoHash*[capacidad];//Crear el arreglo de
        // Inicializar todos los punteros a nullptr (vacios)
        for (int i = 0; i < capacidad; i++) {
          tabla[i] = nullptr;
        }
    }

    void insertar(string idPedido, Pedido* ped) {
        int indice = funcionHash(idPedido);
        NodoHash* nuevoNodo = new NodoHash(idPedido, pedido);

        // Si no hay colisi-n, es el primer elemento
        if (tabla[indice] == nullptr) {
            tabla[indice] = nuevoNodo;
        } else {
            // Manejo de colisi-n: Enlazar al inicio de la lista
            nuevoNodo->siguiente = tabla[indice];
            tabla[indice] = nuevoNodo;
        }
    }

    Pedido* buscar(string idPedido){
        int indice =funcionHash(idPedido);
        NodoHash* actual = tabla[indice];
        while (actual != nullptr){
            if (actual->idPedido == idPedido){
                cout <<"Pedido ID: "<<actual->ped->idPedido<<endl
                <<"DNI: "<<actual->ped->dniCliente<<endl
                <<"Tipo Envio: "<<actual->ped->tipoEnvio<<endl
                <<"Prioridad: "<<actual->ped->nivelPrioridad<<endl
                <<"Monto: "<<actual->ped->montoDolares<<endl
                <<"Fecha: "<<actual->ped->fecha<<endl
                <<"Hora: "<<actual->ped->hora<<endl
                <<"Estado: "<<actual->ped->estado<<endl;
                return actual->ped;
            }
            actual=actual->siguiente;
        }
        cout <<"No encontrado"<<endl;
        return nullptr;
    }

    void eliminar(string idPedido) {
        int indice = funcionHash(idPedido);
        NodoHash* actual = tabla[indice];
        NodoHash* anterior = nullptr;

        while (actual != nullptr) {
            if (actual->idPedido == idPedido) {
                // Si el nodo a eliminar es el primero (cabeza de la lista)
                if (anterior == nullptr) {
                    tabla[indice] = actual->siguiente;
                } else {
                    // Si el nodo est- en medio o al final
                    anterior->siguiente = actual->siguiente;
                }

                delete actual; // Liberar memoria del puntero
                cout << "idPedido " << idPedido << " eliminada." << endl;
                return;
            }

            anterior = actual;
            actual = actual->siguiente;
        }

        cout << "idPedido no encontrada para eliminar." << endl;
    }

    int getCantidad(){
        int total = 0;
        for (int i = 0; i < capacidad; i++) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                total++;
                actual = actual->siguiente;
            }
        }
        return total;
    }
    ~TablaHash() {
        for (int i = 0; i < capacidad; i++) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                NodoHash* temporal = actual;
                actual = actual->siguiente;
                delete temporal; // Liberar cada nodo
            }
        }
        delete[] tabla; // Liberar el arreglo din-mico de punteros
    } // Fin de la clase

    //FALTA HACER REHASH PARA CUANDO EXCEDA.

};


int main(){
    Pedido* orden = new Pedido()
    TablaHash miTabla(10);
    miTabla.insertar("Ana",10);
    cout <<"Edad de Ana: "<<miTabla.buscar("Ana")<<endl;
    miTabla.insertar("John",13);
    cout <<"Edad de John: "<<miTabla.buscar("John")<<endl;
    miTabla.insertar("Lopez",10);
    cout <<"Edad de Lopez: "<<miTabla.buscar("Lopez")<<endl;

    miTabla.eliminar("John");
    cout <<"Edad de John: "<<miTabla.buscar("John")<<endl;
    cout << miTabla.getCantidad()<<endl;

    return 0;
}
