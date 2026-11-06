#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <limits>
#include <cmath>
#include <memory>
#include <exception>

#include "Sistema.cpp"


using namespace std;

// ============================================================
//  main.cpp  -  depende de: Sistema
//  Desde la raiz: g++ -std=c++11 src/main.cpp -o sistema
//  Compilar SOLO main.cpp y ejecutar desde la carpeta que contiene data/.
// ============================================================

namespace {
    bool leerTexto(const string& mensaje, string& texto) {
        while (true) {
            cout << mensaje;
            if (!getline(cin, texto)) return false;
            const size_t inicio = texto.find_first_not_of(" \t\r\n");
            if (inicio != string::npos) {
                const size_t fin = texto.find_last_not_of(" \t\r\n");
                texto = texto.substr(inicio, fin - inicio + 1);
                return true;
            }
            cout << "La entrada no puede estar vacia." << endl;
        }
    }

    bool leerEntero(const string& mensaje, int minimo, int maximo, int& valor) {
        string texto;
        while (leerTexto(mensaje, texto)) {
            istringstream entrada(texto);
            entrada.imbue(locale::classic());
            if ((entrada >> valor) && (entrada >> ws).eof() &&
                valor >= minimo && valor <= maximo) return true;
            cout << "Ingrese un entero entre " << minimo << " y " << maximo << "." << endl;
        }
        return false;
    }

    bool digitos(const string& texto) {
        if (texto.empty()) return false;
        for (size_t i = 0; i < texto.length(); ++i) {
            if (texto[i] < '0' || texto[i] > '9') return false;
        }
        return true;
    }

    bool leerId(string& id) {
        while (leerTexto("ID del pedido (PD-000001): ", id)) {
            if (id.length() == 9 && id.substr(0, 3) == "PD-" &&
                digitos(id.substr(3))) return true;
            cout << "Use PD- seguido de seis digitos." << endl;
        }
        return false;
    }

    bool leerDni(string& dni) {
        while (leerTexto("DNI del cliente (8 digitos): ", dni)) {
            if (dni.length() == 8 && digitos(dni)) return true;
            cout << "El DNI debe contener exactamente ocho digitos." << endl;
        }
        return false;
    }

    bool leerMonto(double& monto) {
        string texto;
        while (leerTexto("Monto en dolares (use punto decimal): ", texto)) {
            istringstream entrada(texto);
            entrada.imbue(locale::classic());
            if ((entrada >> monto) && (entrada >> ws).eof() &&
                std::isfinite(monto) && monto >= 0) return true;
            cout << "Ingrese un monto finito mayor o igual a cero." << endl;
        }
        return false;
    }

    bool leerFecha(const string& mensaje, string& fecha) {
        while (leerTexto(mensaje, fecha)) {
            if (esFechaValida(fecha)) return true;
            cout << "Fecha invalida. Use YYYY-MM-DD, entre los anos 1900 y 2100." << endl;
        }
        return false;
    }

    bool leerHora(string& hora) {
        while (leerTexto("Hora (HH:MM:SS): ", hora)) {
            if (esHoraValida(hora)) return true;
            cout << "Hora invalida. Use HH:MM:SS, entre 00:00:00 y 23:59:59." << endl;
        }
        return false;
    }

    bool elegirEstado(string& estado) {
        cout << "1. " << ESTADO_PENDIENTE << endl
             << "2. " << ESTADO_TRANSITO << endl
             << "3. " << ESTADO_ENTREGADO << endl;
        int opcion;
        if (!leerEntero("Estado: ", 1, 3, opcion)) return false;
        if (opcion == 1) estado = ESTADO_PENDIENTE;
        else if (opcion == 2) estado = ESTADO_TRANSITO;
        else estado = ESTADO_ENTREGADO;
        // Cancelado se gestiona exclusivamente mediante la opcion de cancelacion.
        return true;
    }

    void consultarRango(Sistema& sistema) {
        string inicio, fin;
        while (leerFecha("Fecha inicial (YYYY-MM-DD): ", inicio) &&
               leerFecha("Fecha final (YYYY-MM-DD): ", fin)) {
            if (claveInicioDia(inicio) <= claveFinDia(fin)) {
                sistema.consultaRango(inicio, fin);
                return;
            }
            cout << "La fecha inicial no puede ser posterior a la final." << endl;
        }
    }
}
 
void mostrarMenu() {
    cout << "\n=== GESTION DE PEDIDOS ===" << endl
         << "1. Cargar pedidos desde CSV" << endl
         << "2. Registrar pedido manualmente" << endl
         << "3. Buscar pedido por ID" << endl
         << "4. Despachar pedidos prioritarios" << endl
         << "5. Consultar los primeros 30 pedidos en orden cronologico" << endl
         << "6. Consultar pedidos por rango de fechas" << endl
         << "7. Actualizar estado de un pedido" << endl
         << "8. Cancelar pedido" << endl
         << "9. Mostrar estadisticas" << endl
         << "0. Salir" << endl;
}
 
void ingresarPedidoManual(Sistema& s) {
    Pedido datos;
    if (!leerId(datos.idPedido) || !leerDni(datos.dniCliente)) return;

    cout << "1. Normal\n2. Express\n3. SameDay" << endl;
    int tipo;
    if (!leerEntero("Tipo de envio: ", 1, 3, tipo)) return;
    if (tipo == 1) datos.tipoEnvio = "Normal";
    else if (tipo == 2) datos.tipoEnvio = "Express";
    else datos.tipoEnvio = "SameDay";

    if (!leerEntero("Prioridad (1 a 10): ", 1, 10, datos.nivelPrioridad) ||
        !leerMonto(datos.montoDolares) ||
        !leerFecha("Fecha (YYYY-MM-DD): ", datos.fecha) ||
        !leerHora(datos.hora) || !elegirEstado(datos.estado)) return;

    datos.claveFecha = calcularClaveFecha(datos.fecha, datos.hora);
    unique_ptr<Pedido> pedido(new Pedido(datos));
    // Transferir ANTES de llamar: si registrarPedido lanza, su contrato indica
    // que el llamador no debe liberar el puntero. La excepcion llega a main,
    // donde se termina la sesion y se destruye Sistema.
    Pedido* candidato = pedido.release();
    if (s.registrarPedido(candidato)) {
        cout << "Pedido registrado." << endl;
    } else {
        pedido.reset(candidato); // false: el llamador conserva la propiedad
        cout << "No se registro el pedido: ID duplicado, datos invalidos o insercion rechazada."
             << endl;
    }
}
 
void pruebaCargaMasiva(Sistema& s)   { /* TODO: tiempo de cargarCSV */ }
void pruebaBusqueda(Sistema& s)      { /* TODO: primero, último e inventado */ }
void pruebaDespacho(Sistema& s)      { /* TODO: 10 más urgentes */ }
void pruebaRango(Sistema& s)         { /* TODO: una semana, un mes, rango vacío */ }
void pruebaInsercion(Sistema& s)     { /* TODO: insertar y verificar en hash y árbol */ }
void pruebaActualizacion(Sistema& s) { /* TODO: 10 pedidos */ }
void pruebaCancelacion(Sistema& s)   { /* TODO: 10 pedidos y verificar con el hash */ }
 
int main() {
    try {
        Sistema sistema;
        while (cin) {
            mostrarMenu();
            int opcion;
            if (!leerEntero("Seleccione una opcion: ", 0, 9, opcion)) break;

            switch (opcion) {
                case 0:
                    cout << "Hasta luego." << endl;
                    return 0;
                case 1: {
                    string ruta;
                    if (!leerTexto("Ruta CSV (ejemplo: data/pedidos_masivo.csv): ", ruta)) break;
                    const int cargados = sistema.cargarCSV(ruta);
                    cout << "Pedidos registrados desde el CSV: " << cargados << endl;
                    break;
                }
                case 2:
                    ingresarPedidoManual(sistema);
                    break;
                case 3: {
                    string id;
                    if (leerId(id)) sistema.buscarPorId(id);
                    break;
                }
                case 4: {
                    int cantidad;
                    if (leerEntero("Cantidad de pedidos a despachar: ", 1,
                                   numeric_limits<int>::max(), cantidad)) {
                        sistema.despacharLote(cantidad);
                    }
                    break;
                }
                case 5:
                    sistema.consultaOrdenada();
                    break;
                case 6:
                    consultarRango(sistema);
                    break;
                case 7: {
                    string id, estado;
                    if (leerId(id) && elegirEstado(estado)) {
                        if (sistema.actualizarEstado(id, estado)) {
                            cout << "Estado actualizado." << endl;
                        } else {
                            cout << "No se actualizo el estado." << endl;
                        }
                    }
                    break;
                }
                case 8: {
                    string id;
                    if (leerId(id)) {
                        if (sistema.cancelarPedido(id)) {
                            cout << "Pedido cancelado." << endl;
                        } else {
                            cout << "No se cancelo el pedido." << endl;
                        }
                    }
                    break;
                }
                case 9:
                    sistema.mostrarEstadisticas();
                    break;
            }
        }
        if (cin.eof()) {
            cout << "\nFin de la entrada. Cerrando el sistema." << endl;
            return 0;
        }
        cerr << "No se pudo leer la entrada. Cerrando el sistema." << endl;
        return 1;
    } catch (const exception& error) {
        cerr << "La sesion se ha cerrado por un error: " << error.what() << endl;
    } catch (...) {
        cerr << "La sesion se ha cerrado por un error inesperado." << endl;
    }
    // No continuar tras una excepcion: Sistema puede tener operaciones parciales.
    return 1;
}
