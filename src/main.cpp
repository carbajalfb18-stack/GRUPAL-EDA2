#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <limits>
#include <cmath>
#include <memory>
#include <exception>
#include <chrono>
#include <functional>
#include <vector>
#include <set>
#include <algorithm>

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
         << "10. Pruebas verificables (instancias independientes)" << endl
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
 
namespace {
    // STL solo para oraculos de prueba; no sustituye las estructuras del sistema.
    string conjuntoPruebas = "masivo";
    enum ResultadoPrueba { APROBADA, FALLIDA, BLOQUEADA };

    void exigir(bool condicion, const string& detalle) {
        if (!condicion) throw runtime_error(detalle);
    }

    // Captura temporal para comprobar interfaces publicas que devuelven void.
    // Restaura cout aun ante excepciones; los fallos incluyen la salida observada.
    string observar(const function<void()>& operacion) {
        ostringstream salida;
        salida.imbue(locale::classic());
        struct Restaurar {
            streambuf* anterior;
            streamsize precision;
            ios::fmtflags formato;
            explicit Restaurar(streambuf* nuevo)
                : anterior(cout.rdbuf(nuevo)), precision(cout.precision()), formato(cout.flags()) {
                cout.unsetf(ios::floatfield);
                cout.precision(numeric_limits<double>::max_digits10);
            }
            ~Restaurar() {
                cout.rdbuf(anterior);
                cout.precision(precision);
                cout.flags(formato);
            }
        } restaurar(salida.rdbuf());
        operacion();
        return salida.str();
    }

    template<typename T> T numeroPrueba(const string& texto) {
        istringstream entrada(texto);
        entrada.imbue(locale::classic());
        T valor;
        exigir(static_cast<bool>(entrada >> valor) && (entrada >> ws).eof(),
               "Numero invalido: " + texto);
        return valor;
    }

    string campoSalida(const string& salida, const string& etiqueta) {
        istringstream lineas(salida);
        string linea;
        while (getline(lineas, linea)) {
            if (linea.compare(0, etiqueta.size(), etiqueta) == 0) {
                string valor = linea.substr(etiqueta.size());
                size_t inicio = valor.find_first_not_of(" \t\r");
                size_t fin = valor.find_last_not_of(" \t\r");
                return inicio == string::npos ? "" : valor.substr(inicio, fin - inicio + 1);
            }
        }
        throw runtime_error("Falta '" + etiqueta + "'. Salida observada:\n" + salida);
    }

    long long centavos(double monto) { return llround(monto * 100.0); }

    struct RangoPrueba {
        string inicio, fin;
        int cantidad;
        long long monto;
    };

    struct DatosPrueba {
        string ruta, inexistente;
        vector<Pedido> pedidos;
        vector<int> prioridades;
        vector<RangoPrueba> rangos;
        long long monto;
    };

    RangoPrueba calcularRango(const DatosPrueba& datos, const string& inicio, const string& fin) {
        RangoPrueba rango = {inicio, fin, 0, 0};
        for (size_t i = 0; i < datos.pedidos.size(); ++i) {
            const Pedido& p = datos.pedidos[i];
            if (p.fecha >= inicio && p.fecha <= fin) {
                ++rango.cantidad;
                rango.monto += centavos(p.montoDolares);
            }
        }
        return rango;
    }

    DatosPrueba leerDatosPrueba() {
        DatosPrueba datos;
        datos.ruta = "data/pedidos_" + conjuntoPruebas + ".csv";
        datos.monto = 0;
        ifstream csv(datos.ruta.c_str());
        exigir(csv.is_open(), "No se pudo abrir " + datos.ruta + "; ejecutar desde la raiz");
        string linea;
        set<string> ids;
        int frecuencias[11] = {};
        while (getline(csv, linea)) {
            if (!linea.empty() && linea.back() == '\r') linea.pop_back();
            exigir(count(linea.begin(), linea.end(), ',') == 7, "CSV con columnas invalidas");
            string campos[8];
            exigir(dividirLinea(linea, campos, 8) == 8, "CSV incompleto");
            Pedido p;
            p.idPedido = campos[0]; p.dniCliente = campos[1]; p.tipoEnvio = campos[2];
            p.nivelPrioridad = numeroPrueba<int>(campos[3]);
            p.montoDolares = numeroPrueba<double>(campos[4]);
            p.fecha = campos[5]; p.hora = campos[6]; p.estado = campos[7];
            exigir(p.nivelPrioridad >= 1 && p.nivelPrioridad <= 10 &&
                   isfinite(p.montoDolares) && p.montoDolares >= 0 &&
                   esFechaValida(p.fecha) && esHoraValida(p.hora) &&
                   (p.estado == ESTADO_PENDIENTE || p.estado == ESTADO_TRANSITO ||
                    p.estado == ESTADO_ENTREGADO), "Datos CSV invalidos: " + p.idPedido);
            exigir(ids.insert(p.idPedido).second, "ID duplicado en CSV: " + p.idPedido);
            p.claveFecha = calcularClaveFecha(p.fecha, p.hora);
            datos.pedidos.push_back(p);
            datos.monto += centavos(p.montoDolares);
            ++frecuencias[p.nivelPrioridad];
        }
        exigir(datos.pedidos.size() >= 10, "Se requieren al menos diez pedidos");
        for (int prioridad = 10; prioridad >= 1; --prioridad) {
            for (int i = 0; i < frecuencias[prioridad] && datos.prioridades.size() < 10; ++i)
                datos.prioridades.push_back(prioridad);
        }
        const string rutaVerificacion = "data/verificacion_" + conjuntoPruebas + ".txt";
        ifstream verificacion(rutaVerificacion.c_str());
        exigir(verificacion.is_open(), "No se pudo abrir " + rutaVerificacion);
        ostringstream contenido;
        contenido << verificacion.rdbuf();
        const string esperado = contenido.str();
        exigir(numeroPrueba<int>(campoSalida(esperado, "Total de pedidos:")) ==
               static_cast<int>(datos.pedidos.size()), "Total CSV difiere de verificacion");
        exigir(numeroPrueba<int>(campoSalida(esperado, "Pedidos en el heap:")) ==
               static_cast<int>(datos.pedidos.size()), "Tamano esperado del Heap inconsistente");
        exigir(centavos(numeroPrueba<double>(campoSalida(esperado, "Monto total acumulado:"))) ==
               datos.monto, "Monto CSV difiere de verificacion");
        exigir(campoSalida(esperado, "Primero:") == datos.pedidos.front().idPedido &&
               campoSalida(esperado, "Ultimo:") == datos.pedidos.back().idPedido,
               "IDs extremos difieren de verificacion");
        istringstream idAusente(campoSalida(esperado, "Inventado:"));
        idAusente >> datos.inexistente;
        exigir(ids.count(datos.inexistente) == 0, "El ID inexistente aparece en el CSV");
        istringstream lineas(esperado);
        bool prioridadesLeidas = false;
        while (getline(lineas, linea)) {
            if (linea.find("--- Despacho:") == 0) {
                exigir(static_cast<bool>(getline(lineas, linea)), "Faltan prioridades esperadas");
                string campos[10];
                exigir(count(linea.begin(), linea.end(), ',') == 9 &&
                       dividirLinea(linea, campos, 10) == 10, "Top diez invalido");
                for (int i = 0; i < 10; ++i)
                    exigir(numeroPrueba<int>(campos[i]) == datos.prioridades[i],
                           "Prioridades CSV difieren de verificacion");
                prioridadesLeidas = true;
            }
            if (linea.find("cantidad = ") != string::npos) {
                size_t dosPuntos = linea.find(':');
                istringstream fechas(linea.substr(dosPuntos + 1));
                string inicio, separador, fin;
                fechas >> inicio >> separador >> fin;
                RangoPrueba rango = calcularRango(datos, inicio, fin);
                size_t cantidad = linea.find("cantidad = ") + 11;
                size_t coma = linea.find(',', cantidad);
                size_t monto = linea.find("monto = ") + 8;
                exigir(rango.cantidad == numeroPrueba<int>(linea.substr(cantidad, coma - cantidad)) &&
                       rango.monto == centavos(numeroPrueba<double>(linea.substr(monto))),
                       "Rango CSV difiere de verificacion: " + linea);
                if (linea.find("semana") != string::npos || linea.find("un mes") != string::npos ||
                    linea.find("sin resultados") != string::npos) datos.rangos.push_back(rango);
            }
        }
        exigir(prioridadesLeidas, "Falta el top diez en verificacion");
        if (conjuntoPruebas == "pequeno") {
            // El TXT pequeno no incluye mes: se deriva el mes del primer registro.
            string inicio = datos.pedidos.front().fecha.substr(0, 7) + "-01";
            string fin;
            for (int dia = 31; dia >= 28; --dia) {
                fin = inicio.substr(0, 8) + to_string(dia);
                if (esFechaValida(fin)) break;
            }
            datos.rangos.push_back(calcularRango(datos, inicio, fin));
        }
        exigir(datos.rangos.size() == 3, "Se requieren semana, mes e intervalo vacio");
        return datos;
    }

    bool arbolPendiente() {
        // Bloqueo conservador: no aprobar operaciones del arbol mientras haya TODO.
        // Tras completarlo, las comprobaciones reales siguen siendo obligatorias.
        ifstream archivo("src/RedBlackTree.cpp");
        if (!archivo.is_open()) return true;
        ostringstream codigo;
        codigo << archivo.rdbuf();
        return codigo.str().find("TODO") != string::npos;
    }

    void verificarEstadisticas(Sistema& sistema, int total, int pendientes, long long monto) {
        string salida = observar([&]() { sistema.mostrarEstadisticas(); });
        exigir(numeroPrueba<int>(campoSalida(salida, "Total de pedidos:")) == total &&
               numeroPrueba<int>(campoSalida(salida, "Pendientes por despachar:")) == pendientes &&
               centavos(numeroPrueba<double>(campoSalida(salida, "Monto total acumulado:"))) == monto,
               "Estadisticas incorrectas:\n" + salida);
    }

    string buscarObservado(Sistema& sistema, const string& id) {
        return observar([&]() { sistema.buscarPorId(id); });
    }

    void verificarBusqueda(Sistema& sistema, const Pedido& p) {
        const string salida = buscarObservado(sistema, p.idPedido);
        exigir(campoSalida(salida, "Pedido ID: ") == p.idPedido &&
               campoSalida(salida, "DNI: ") == p.dniCliente &&
               campoSalida(salida, "Tipo Envio: ") == p.tipoEnvio &&
               numeroPrueba<int>(campoSalida(salida, "Prioridad: ")) == p.nivelPrioridad &&
               centavos(numeroPrueba<double>(campoSalida(salida, "Monto: "))) == centavos(p.montoDolares) &&
               campoSalida(salida, "Fecha: ") == p.fecha &&
               campoSalida(salida, "Hora: ") == p.hora &&
               campoSalida(salida, "Estado: ") == p.estado,
               "Busqueda incorrecta:\n" + salida);
    }

    vector<Pedido> filasObservadas(const string& salida) {
        vector<Pedido> filas;
        istringstream lineas(salida);
        lineas.imbue(locale::classic());
        string linea;
        while (getline(lineas, linea)) {
            if (linea.compare(0, 3, "PD-") != 0) continue;
            Pedido p;
            istringstream campos(linea);
            campos.imbue(locale::classic());
            exigir(static_cast<bool>(campos >> p.idPedido >> p.dniCliente >> p.tipoEnvio >>
                    p.nivelPrioridad >> p.montoDolares >> p.fecha >> p.hora >> p.estado) &&
                    (campos >> ws).eof(), "Fila de salida invalida: " + linea);
            p.claveFecha = calcularClaveFecha(p.fecha, p.hora);
            filas.push_back(p);
        }
        return filas;
    }

    void verificarRango(Sistema& sistema, const RangoPrueba& rango) {
        string salida = observar([&]() { sistema.consultaRango(rango.inicio, rango.fin); });
        exigir(numeroPrueba<int>(campoSalida(salida, "Cantidad de pedidos:")) == rango.cantidad &&
               centavos(numeroPrueba<double>(campoSalida(salida, "Monto total:"))) == rango.monto,
               "Rango incorrecto " + rango.inicio + " a " + rango.fin + ":\n" + salida);
        cout << "  Rango " << rango.inicio << " a " << rango.fin << ": "
             << rango.cantidad << " pedidos; " << rango.monto << " centavos\n";
    }

    void verificarOrden(Sistema& sistema, vector<Pedido> esperados) {
        sort(esperados.begin(), esperados.end(), [](const Pedido& a, const Pedido& b) {
            return compararPedidos(&a, &b) < 0;
        });
        vector<Pedido> actual = filasObservadas(observar([&]() { sistema.consultaOrdenada(); }));
        size_t cantidad = min(static_cast<size_t>(30), esperados.size());
        exigir(actual.size() == cantidad, "Cantidad incorrecta en consulta ordenada del arbol");
        for (size_t i = 0; i < cantidad; ++i)
            exigir(actual[i].idPedido == esperados[i].idPedido &&
                   actual[i].claveFecha == esperados[i].claveFecha,
                   "Orden del arbol incorrecto en posicion " + to_string(i));
    }

    bool registrarCopia(Sistema& sistema, const Pedido& datos) {
        unique_ptr<Pedido> propietario(new Pedido(datos));
        // Contrato de Sistema: consume si lanza; devuelve propiedad si retorna false.
        Pedido* candidato = propietario.release();
        bool registrado = sistema.registrarPedido(candidato);
        if (!registrado) propietario.reset(candidato);
        return registrado;
    }

    const char* nombresPruebas[] = {"Carga masiva", "Busqueda", "Despacho", "Rangos",
                                  "Insercion", "Actualizacion", "Cancelacion"};

    ResultadoPrueba ejecutarPruebaSistema(int prueba) {
        cout << "\n[PRUEBA] " << nombresPruebas[prueba] << " (" << conjuntoPruebas << ")\n";
        try {
            DatosPrueba datos = leerDatosPrueba();
            const bool pendiente = arbolPendiente();
            if (pendiente && (prueba == 0 || prueba == 3 || prueba == 4 || prueba == 6)) {
                cout << "[BLOQUEADA] Requiere insercion, consulta o eliminacion real del arbol.\n";
                return BLOQUEADA;
            }
            // Instancia propia: ninguna prueba modifica el Sistema del menu ni otra prueba.
            // Con el arbol actual, constructor/insertar/destructor son cuerpos vacios;
            // busqueda, despacho y actualizacion verifican exclusivamente Hash/Heap.
            Sistema sistema;
            int cargados = 0;
            const auto inicio = chrono::steady_clock::now();
            if (prueba != 4) {
                observar([&]() { cargados = sistema.cargarCSV(datos.ruta); });
                exigir(cargados == static_cast<int>(datos.pedidos.size()), "Carga incompleta");
            }
            const double milisegundos = chrono::duration<double, milli>(
                chrono::steady_clock::now() - inicio).count();
            const int total = static_cast<int>(datos.pedidos.size());
            if (prueba != 4) verificarEstadisticas(sistema, total, total, datos.monto);
            if (prueba == 0) {
                cout << "  " << cargados << " registros cargados en " << milisegundos
                     << " ms (incluye captura de la salida del hash).\n";
                verificarOrden(sistema, datos.pedidos);
                string fechaMin = datos.pedidos.front().fecha, fechaMax = fechaMin;
                for (size_t i = 0; i < datos.pedidos.size(); ++i) {
                    fechaMin = min(fechaMin, datos.pedidos[i].fecha);
                    fechaMax = max(fechaMax, datos.pedidos[i].fecha);
                }
                verificarRango(sistema, calcularRango(datos, fechaMin, fechaMax));
                verificarBusqueda(sistema, datos.pedidos.front());
                verificarBusqueda(sistema, datos.pedidos.back());
            } else if (prueba == 1) {
                const Pedido extremos[] = {datos.pedidos.front(), datos.pedidos.back()};
                for (int i = 0; i < 2; ++i) {
                    const auto antes = chrono::steady_clock::now();
                    verificarBusqueda(sistema, extremos[i]);
                    cout << "  " << extremos[i].idPedido << ": encontrado, "
                         << chrono::duration<double, milli>(chrono::steady_clock::now() - antes).count()
                         << " ms (consulta y verificacion).\n";
                }
                const auto antes = chrono::steady_clock::now();
                exigir(buscarObservado(sistema, datos.inexistente) == "No encontrado\n",
                       "La busqueda de ID inexistente no informo ausencia");
                cout << "  " << datos.inexistente << ": ausente, "
                     << chrono::duration<double, milli>(chrono::steady_clock::now() - antes).count()
                     << " ms.\n";
            } else if (prueba == 2) {
                vector<Pedido> despachados = filasObservadas(observar([&]() { sistema.despacharLote(10); }));
                exigir(despachados.size() == 10, "El despacho no produjo diez pedidos");
                set<string> ids;
                for (size_t i = 0; i < despachados.size(); ++i) {
                    const Pedido& p = despachados[i];
                    exigir(ids.insert(p.idPedido).second && p.nivelPrioridad == datos.prioridades[i] &&
                           p.estado == ESTADO_TRANSITO, "Despacho duplicado, estado o prioridad incorrectos");
                    bool encontrado = false;
                    for (size_t j = 0; j < datos.pedidos.size(); ++j) {
                        if (datos.pedidos[j].idPedido == p.idPedido) {
                            encontrado = datos.pedidos[j].nivelPrioridad == p.nivelPrioridad;
                            break;
                        }
                    }
                    exigir(encontrado, "El despacho contiene un pedido ajeno al CSV");
                    verificarBusqueda(sistema, p);
                    cout << "  " << p.idPedido << ": prioridad " << p.nivelPrioridad << '\n';
                }
                verificarEstadisticas(sistema, total, total - 10, datos.monto);
            } else if (prueba == 3) {
                for (size_t i = 0; i < datos.rangos.size(); ++i) verificarRango(sistema, datos.rangos[i]);
            } else if (prueba == 4) {
                // Caso valido documentado: copia del primer CSV en un sistema vacio.
                const Pedido& p = datos.pedidos.front();
                bool registrado = false;
                observar([&]() { registrado = registrarCopia(sistema, p); });
                exigir(registrado, "Insercion individual rechazada");
                verificarBusqueda(sistema, p);
                verificarOrden(sistema, vector<Pedido>(1, p));
                RangoPrueba rango = {p.fecha, p.fecha, 1, centavos(p.montoDolares)};
                verificarRango(sistema, rango);
                verificarEstadisticas(sistema, 1, 1, centavos(p.montoDolares));
                bool duplicado = true;
                observar([&]() { duplicado = registrarCopia(sistema, p); });
                exigir(!duplicado, "No se rechazo el ID duplicado");
                verificarEstadisticas(sistema, 1, 1, centavos(p.montoDolares));
                vector<Pedido> salida = filasObservadas(observar([&]() { sistema.despacharLote(1); }));
                exigir(salida.size() == 1 && salida[0].idPedido == p.idPedido,
                       "El pedido insertado no estaba en el Heap");
            } else if (prueba == 5) {
                for (int i = 0; i < 10; ++i) {
                    Pedido esperado = datos.pedidos[i];
                    esperado.estado = esperado.estado == ESTADO_TRANSITO ? ESTADO_ENTREGADO : ESTADO_TRANSITO;
                    bool actualizado = false;
                    observar([&]() { actualizado = sistema.actualizarEstado(esperado.idPedido, esperado.estado); });
                    exigir(actualizado, "Actualizacion rechazada: " + esperado.idPedido);
                    verificarBusqueda(sistema, esperado);
                    cout << "  " << esperado.idPedido << " -> " << esperado.estado << '\n';
                }
                verificarEstadisticas(sistema, total, total, datos.monto);
            } else if (prueba == 6) {
                set<string> cancelados;
                long long monto = datos.monto;
                for (int i = 0; i < 10; ++i) {
                    const string id = datos.pedidos[i].idPedido;
                    bool cancelado = false;
                    observar([&]() { cancelado = sistema.cancelarPedido(id); });
                    exigir(cancelado, "Cancelacion rechazada: " + id);
                    exigir(buscarObservado(sistema, id) == "No encontrado\n", "Cancelado aun en hash: " + id);
                    cancelados.insert(id);
                    monto -= centavos(datos.pedidos[i].montoDolares);
                }
                vector<Pedido> restantes;
                for (size_t i = 0; i < datos.pedidos.size(); ++i)
                    if (!cancelados.count(datos.pedidos[i].idPedido)) restantes.push_back(datos.pedidos[i]);
                verificarOrden(sistema, restantes);
                // Verificar cada dia afectado, incluidos los pedidos no cancelados de ese dia.
                set<string> dias;
                for (int i = 0; i < 10; ++i) dias.insert(datos.pedidos[i].fecha);
                for (set<string>::const_iterator dia = dias.begin(); dia != dias.end(); ++dia) {
                    RangoPrueba rango = calcularRango(datos, *dia, *dia);
                    for (int i = 0; i < 10; ++i) if (datos.pedidos[i].fecha == *dia) {
                        --rango.cantidad; rango.monto -= centavos(datos.pedidos[i].montoDolares);
                    }
                    verificarRango(sistema, rango);
                }
                verificarEstadisticas(sistema, total - 10, total - 10, monto);
                vector<Pedido> salida = filasObservadas(observar([&]() { sistema.despacharLote(total); }));
                exigir(salida.size() == restantes.size(), "Cantidad incorrecta despues de cancelar");
                set<string> esperados;
                for (size_t i = 0; i < restantes.size(); ++i) esperados.insert(restantes[i].idPedido);
                for (size_t i = 0; i < salida.size(); ++i)
                    exigir(esperados.erase(salida[i].idPedido) == 1, "Despacho repetido o cancelado");
                exigir(esperados.empty(), "Se perdieron pedidos no cancelados");
                verificarEstadisticas(sistema, total - 10, 0, monto);
            }
            cout << "[APROBADA" << (pendiente ? " Hash/Heap; arbol NO validado" : "") << "] "
                 << nombresPruebas[prueba] << '\n';
            return APROBADA;
        } catch (const exception& error) {
            // El Sistema local ya se destruyo; nunca continuar usandolo tras un fallo.
            cout << "[FALLIDA] " << error.what() << '\n';
            return FALLIDA;
        }
    }

    int ejecutarTodasLasPruebas() {
        int aprobadas = 0, fallidas = 0, bloqueadas = 0;
        for (int i = 0; i < 7; ++i) {
            ResultadoPrueba resultado = ejecutarPruebaSistema(i);
            if (resultado == APROBADA) ++aprobadas;
            else if (resultado == FALLIDA) ++fallidas;
            else ++bloqueadas;
        }
        cout << "\nResumen: " << aprobadas << " aprobadas (alcance indicado), " << fallidas
             << " fallidas, " << bloqueadas << " bloqueadas.\n";
        return fallidas ? 1 : bloqueadas ? 2 : 0;
    }
}

// Se conserva la firma; el parametro no se modifica. Cada prueba crea su Sistema.
void pruebaCargaMasiva(Sistema&)   { ejecutarPruebaSistema(0); }
void pruebaBusqueda(Sistema&)      { ejecutarPruebaSistema(1); }
void pruebaDespacho(Sistema&)      { ejecutarPruebaSistema(2); }
void pruebaRango(Sistema&)         { ejecutarPruebaSistema(3); }
void pruebaInsercion(Sistema&)     { ejecutarPruebaSistema(4); }
void pruebaActualizacion(Sistema&) { ejecutarPruebaSistema(5); }
void pruebaCancelacion(Sistema&)   { ejecutarPruebaSistema(6); }

void menuPruebas(Sistema& sistema) {
    cout << "Datos: 1. pequeno  2. masivo\n";
    int datos;
    if (!leerEntero("Conjunto: ", 1, 2, datos)) return;
    conjuntoPruebas = datos == 1 ? "pequeno" : "masivo";
    cout << "1. Carga  2. Busqueda  3. Despacho  4. Rangos\n"
         << "5. Insercion  6. Actualizacion  7. Cancelacion  8. Todas  0. Volver\n";
    int opcion;
    if (!leerEntero("Prueba: ", 0, 8, opcion)) return;
    typedef void (*PruebaMenu)(Sistema&);
    const PruebaMenu pruebas[] = {pruebaCargaMasiva, pruebaBusqueda, pruebaDespacho,
        pruebaRango, pruebaInsercion, pruebaActualizacion, pruebaCancelacion};
    if (opcion == 8) ejecutarTodasLasPruebas();
    else if (opcion > 0) pruebas[opcion - 1](sistema);
}
 
int main(int argc, char* argv[]) {
    try {
        // Modo reproducible: sistema.exe --pruebas [pequeno|masivo].
        // Codigos: 0 todo aprobado; 1 fallo; 2 dependencias pendientes.
        if (argc > 1) {
            if (string(argv[1]) != "--pruebas" || argc > 3 ||
                (argc == 3 && string(argv[2]) != "pequeno" && string(argv[2]) != "masivo")) {
                cerr << "Uso: sistema.exe [--pruebas [pequeno|masivo]]\n";
                return 1;
            }
            if (argc == 3) conjuntoPruebas = argv[2];
            return ejecutarTodasLasPruebas();
        }
        Sistema sistema;
        while (cin) {
            mostrarMenu();
            int opcion;
            if (!leerEntero("Seleccione una opcion: ", 0, 10, opcion)) break;

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
                case 10:
                    menuPruebas(sistema);
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
