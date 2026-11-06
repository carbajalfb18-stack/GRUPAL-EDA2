#include <iostream>
#include <string>
#include <fstream> // leer archivo csv
#include <cmath>
#include <sstream>
#include <iomanip>
#include <locale>
#include <memory>
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
    struct RegistroPropiedad {
        Pedido* pedido;
        RegistroPropiedad* siguiente;
        RegistroPropiedad(Pedido* p, RegistroPropiedad* sig)
            : pedido(p), siguiente(sig) {}
    };

    struct PropietarioPedidos {
        RegistroPropiedad* primero;
        PropietarioPedidos() : primero(nullptr) {}
        ~PropietarioPedidos() {
            while (primero != nullptr) {
                RegistroPropiedad* registro = primero;
                primero = registro->siguiente;
                delete registro->pedido;
                delete registro;
            }
        }

        RegistroPropiedad* buscar(Pedido* p) const {
            for (RegistroPropiedad* r = primero; r != nullptr; r = r->siguiente) {
                if (r->pedido == p) return r;
            }
            return nullptr;
        }
    };

    struct LimpiezaHeap {
        MaxHeap& heap;
        explicit LimpiezaHeap(MaxHeap& h) : heap(h) {}
        ~LimpiezaHeap() {
            // Contrato acordado: descarta y libera cancelados, no los devuelve;
            // nullptr significa que ya no quedan pedidos validos ni cancelados.
            // PENDIENTE GRUPO: extraerMax no debe lanzar durante esta limpieza.
            while (heap.extraerMax() != nullptr) {}
        }
    };

    // IMPORTANTE: los miembros se destruyen en orden inverso a su declaracion:
    // arbol -> hash -> limpiezaHeap -> heap -> propietario.
    // Todos los Pedido activos sobreviven a los nodos que los referencian.
    PropietarioPedidos propietario;
    MaxHeap       heap;
    LimpiezaHeap  limpiezaHeap;
    HashTable     hash;
    ArbolRedBlack arbol;
    int           totalPedidos;
    double        montoAcumulado;
    int           pendientes;

    // Metadatos de propiedad: estos pedidos ya no estan en el heap.
    // Los Pedido siguen perteneciendo al registro privado de Sistema.
    struct RegistroDespacho {
        Pedido* pedido;
        RegistroDespacho* siguiente;
        RegistroDespacho() : pedido(nullptr), siguiente(nullptr) {}
    };
    RegistroDespacho* despachados;

    // Copiar el sistema duplicaria propietarios de los mismos Pedido y nodos.
    Sistema(const Sistema&);
    Sistema& operator=(const Sistema&);

    bool soloDigitos(const string& texto) const {
        if (texto.empty()) return false;
        for (size_t i = 0; i < texto.length(); ++i) {
            if (texto[i] < '0' || texto[i] > '9') return false;
        }
        return true;
    }

    bool estadoValido(const string& estado) const {
        return estado == ESTADO_PENDIENTE || estado == ESTADO_TRANSITO ||
               estado == ESTADO_ENTREGADO || estado == ESTADO_CANCELADO;
    }

    bool datosValidos(const Pedido* p) const {
        if (p == nullptr) return false;
        // Formatos usados por los CSV del proyecto: PD-000001 y DNI de 8 digitos.
        if (p->idPedido.length() != 9 || p->idPedido.substr(0, 3) != "PD-" ||
            !soloDigitos(p->idPedido.substr(3))) return false;
        if (p->dniCliente.length() != 8 || !soloDigitos(p->dniCliente)) return false;
        if (p->tipoEnvio != "Normal" && p->tipoEnvio != "Express" &&
            p->tipoEnvio != "SameDay") return false;
        if (p->nivelPrioridad < 1 || p->nivelPrioridad > 10) return false;
        if (!std::isfinite(p->montoDolares) || p->montoDolares < 0) return false;
        if (!esFechaValida(p->fecha) || !esHoraValida(p->hora)) return false;
        // Un cancelado debe pasar por cancelarPedido, no ingresar a las estructuras.
        return estadoValido(p->estado) && p->estado != ESTADO_CANCELADO;
    }
 
public:
    Sistema() : heap(1024), limpiezaHeap(heap), hash(20011) {
        totalPedidos = 0;
        montoAcumulado = 0;
        pendientes = 0;
        despachados = nullptr;
    }
 
    ~Sistema() {
        // Solo liberar metadatos aqui, nunca Pedido mientras existan nodos.
        // El orden de los miembros realiza el resto de la limpieza automatica.
        while (despachados != nullptr) {
            RegistroDespacho* registro = despachados;
            despachados = registro->siguiente;
            delete registro;
        }
        // NO llamar hash.liberarPedidos(): el propietario los libera al final.
        // Los destructores de las estructuras solo liberan nodos/arreglos.
    }
 
    int cargarCSV(const string& ruta) {
        ifstream archivo;                      // Abre el archivo CSV    algo nuevo para mi
        archivo.open(ruta.c_str());           // c_str() convierte string a const char* para open()
        if (!archivo.is_open()) {
            cout << "No se pudo abrir el archivo: " << ruta << endl;
            return 0;
        }
        string linea;
        string campos[8];
        int cargados = 0;
 
        while (getline(archivo, linea)) {
            if (!linea.empty() && linea[linea.length() - 1] == '\r') {
                linea.erase(linea.length() - 1);
            }
            // El formato acordado es CSV simple, sin comillas ni encabezado.
            // Contar separadores evita que dividirLinea trunque columnas extra.
            int comas = 0;
            for (size_t i = 0; i < linea.length(); ++i) {
                if (linea[i] == ',') ++comas;
            }
            if (comas != 7) continue;
            int n = dividirLinea(linea, campos, 8);
            if (n == 8) {
                // No usar las conversiones permisivas de Utilidades para validar.
                int prioridad;
                double monto;
                istringstream entradaPrioridad(campos[3]);
                istringstream entradaMonto(campos[4]);
                entradaPrioridad.imbue(locale::classic());
                entradaMonto.imbue(locale::classic());
                if (!(entradaPrioridad >> noskipws >> prioridad) ||
                    entradaPrioridad.peek() != char_traits<char>::eof() ||
                    !(entradaMonto >> noskipws >> monto) ||
                    entradaMonto.peek() != char_traits<char>::eof()) continue;

                // Construir bajo RAII: incluso las asignaciones de string pueden
                // lanzar. La fabrica actual crearPedido no protege ese caso.
                unique_ptr<Pedido> p(new Pedido);
                p->idPedido = campos[0];
                p->dniCliente = campos[1];
                p->tipoEnvio = campos[2];
                p->nivelPrioridad = prioridad;
                p->montoDolares = monto;
                p->fecha = campos[5];
                p->hora = campos[6];
                p->estado = campos[7];
                p->claveFecha = 0;
                // registrarPedido consume el puntero si lanza; al devolver false
                // lo devuelve al llamador. Evitar dos propietarios simultaneos.
                Pedido* candidato = p.release();
                if (registrarPedido(candidato)) {
                    cargados++;
                } else {
                    p.reset(candidato); // liberar rechazo al salir de este bloque
                }
            }
        }
        archivo.close();
        return cargados;
    }
 
    bool registrarPedido(Pedido* p) {
        // false: el llamador conserva p; true: Sistema adquiere su propiedad.
        // Hash::insertar(false) no debe guardar ni liberar el puntero.
        // Precondicion: p es nuevo y no pertenece ya a otro sistema/estructura.
        // Excepciones: el llamador NO debe borrar p. Antes de publicarlo se libera
        // por RAII; despues de publicarlo Sistema lo conserva hasta el cierre.
        // Ante ese fallo se debe interrumpir el uso y destruir Sistema: NO se
        // garantiza que las tres inserciones ni los contadores sean atomicos.
        unique_ptr<Pedido> candidato(p);
        if (!datosValidos(p)) {
            candidato.release();
            return false;
        }
        if (hash.buscar(p->idPedido) != nullptr) {
            candidato.release();
            return false;
        }

        const long long claveFecha = calcularClaveFecha(p->fecha, p->hora);
        // Reservar y adoptar antes de publicar el puntero en cualquier estructura.
        // Si new falla, el guard libera p sin que ninguna estructura lo referencie.
        RegistroPropiedad* registro = new RegistroPropiedad(p, propietario.primero);
        propietario.primero = registro;
        candidato.release();
        if (!hash.insertar(p)) {
            propietario.primero = registro->siguiente;
            delete registro; // solo metadatos: el llamador conserva p
            return false;
        }
        p->claveFecha = claveFecha;
        // PENDIENTE GRUPO: heap/arbol insertan con retorno void. Hace falta
        // acordar garantias de excepcion y una forma segura de revertir; el heap
        // no ofrece eliminacion arbitraria. No intentar compensar borrando p.
        heap.insertar(p);
        arbol.insertar(p);
        totalPedidos++;
        pendientes++;
        montoAcumulado += p->montoDolares;   // suma los montos de los pedidos registrados

        return true;
    }
 
    void buscarPorId(const string& id) {
        Pedido* p = hash.buscar(id);
        if(p == nullptr){
            cout << "Pedido no encontrado" << endl;
        } else {
            mostrarPedido(p);
        }
    }
 
    void despacharLote(int n) {
        if (n <= 0) {
            cout << "La cantidad a despachar debe ser positiva" << endl;
            return;
        }
        for(int i = 0; i < n; i++) {
            // Reservar metadatos antes de extraer para no perder la pertenencia
            // al heap si falla esta reserva de memoria.
            unique_ptr<RegistroDespacho> registro(new RegistroDespacho);
            Pedido* p = heap.extraerMax();

            if(p == nullptr) {
                cout << "No hay más pedidos para despachar" << endl;
                return;
            }
            registro->pedido = p;
            registro->siguiente = despachados;
            despachados = registro.release();
            --pendientes;
            // Segun los CSV de verificacion todos los estados ingresan al heap.
            p->estado = ESTADO_TRANSITO;
            mostrarPedido(p);
      
        }
    }
 
    void consultaOrdenada() {
        arbol.inOrder(30);
    }
 
    void consultaRango(const string& fechaIni, const string& fechaFin) {
        if (!esFechaValida(fechaIni) || !esFechaValida(fechaFin)) {
            cout << "Fechas invalidas; use YYYY-MM-DD" << endl;
            return;
        }
        if (claveInicioDia(fechaIni) > claveFinDia(fechaFin)) {
            cout << "La fecha inicial no puede ser posterior a la final" << endl;
            return;
        }
        int cantidad = 0;
        double montoTotal = 0.0;

        arbol.consultaRango(claveInicioDia(fechaIni), claveFinDia(fechaFin), cantidad, montoTotal);
        cout << "Cantidad de pedidos: " << cantidad << endl;

        const ios::fmtflags formato = cout.flags();
        const streamsize precision = cout.precision();
        cout << "Monto total: " << fixed << setprecision(2) << montoTotal << endl;
        cout.flags(formato);
        cout.precision(precision);
    }
 
    bool actualizarEstado(const string& id, const string& nuevoEstado) {
        if (!estadoValido(nuevoEstado)) {
            cout << "Estado invalido" << endl;
            return false;
        }
        if (nuevoEstado == ESTADO_CANCELADO) {
            cout << "Use cancelarPedido para cancelar en todas las estructuras" << endl;
            return false;
        }
        Pedido* p = hash.buscar(id);

        if(p == nullptr) {
            cout << "Pedido no encontrado" << endl;
            return false;
        }
        if (p->estado == ESTADO_CANCELADO) {
            cout << "No se puede actualizar un pedido cancelado" << endl;
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
        if (p->estado == ESTADO_CANCELADO) {
            cout << "El pedido ya esta cancelado" << endl;
            return false;
        }
        RegistroPropiedad* propiedad = propietario.buscar(p);
        if (propiedad == nullptr) {
            cout << "El pedido no tiene un propietario registrado en Sistema" << endl;
            return false;
        }
        // Localizar si ya salio del heap; el estado no indica pertenencia,
        // porque los CSV tambien cargan pedidos EnTransito y Entregado.
        RegistroDespacho** enlace = &despachados;
        while (*enlace != nullptr && (*enlace)->pedido != p) {
            enlace = &(*enlace)->siguiente;
        }
        if (!hash.eliminar(id)) {
            cout << "No se pudo eliminar el pedido del hash" << endl;
            return false;
        }
        // eliminar del arbol solo libera el NodoRB, nunca su Pedido.
        // PENDIENTE GRUPO: eliminar(void) debe completar la retirada sin lanzar;
        // no se puede garantizar una cancelacion atomica con este contrato aun.
        arbol.eliminar(p);

        p->estado = ESTADO_CANCELADO;
        totalPedidos--;
        montoAcumulado -= p->montoDolares;  
        if (*enlace != nullptr) {
            // Ya fue despachado: conservar su propiedad hasta destruir todas las
            // estructuras. No depender aqui de que el arbol ya este implementado.
            RegistroDespacho* registro = *enlace;
            *enlace = registro->siguiente;
            delete registro;
        } else {
            // Transferencia al heap segun el contrato de cancelacion del README.
            // Evita que el propietario intente liberar un pedido ya descartado.
            propiedad->pedido = nullptr;
            --pendientes;
        }

        return true;
    }
 
    void mostrarEstadisticas() {
        cout << "Total de pedidos: " << totalPedidos << endl;
        // Opcion A del README: cancelados no cuentan aunque sigan en el heap.
        cout << "Pendientes por despachar: " << pendientes << endl;
        const ios::fmtflags formato = cout.flags();
        const streamsize precision = cout.precision();
        cout << "Monto total acumulado: " << fixed << setprecision(2)
             << montoAcumulado << endl;
        cout.flags(formato);
        cout.precision(precision);
    }
 
    int getColisiones() {
        return hash.getColisiones();
    }
};
