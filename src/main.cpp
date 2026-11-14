#include <iostream>
#include <string>
#include <chrono>

#include "Sistema.cpp"

using namespace std;

// ============================================================
//  main.cpp
//  Menú del programa y pruebas de sustentación.
// ============================================================

// Si los archivos están en otra carpeta, se cambia aquí
const string RUTA_PEQUENO = "data/pedidos_pequeno.csv";
const string RUTA_MASIVO  = "data/pedidos_masivo.csv";

// Aquí se guardan los tiempos de las pruebas para armar la tabla del informe
int    cargadosMasivo = 0;
double tCarga = 0;
double tBusqueda = 0;
double tInsercion = 0;
double tDespacho = 0;
double tOrdenada = 0;
double tRango = 0;
double tActualizacion = 0;
double tEliminacion = 0;

// ---------------------- cosas de ayuda ----------------------

// Un "Momento" es un instante del reloj; restando dos Momentos sale el tiempo que pasó
typedef chrono::steady_clock::time_point Momento;

Momento ahora() {
    return chrono::steady_clock::now();
}

double aMs(Momento inicio, Momento fin) {
    return chrono::duration<double, milli>(fin - inicio).count();
}

void imprimirMs(double ms) {
    // los tiempos muy chiquitos necesitan mas decimales para no verse como 0
    if (ms < 1) cout.precision(5);
    else cout.precision(3);
    cout << ms << " ms";
    cout.precision(2);
}

string siNo(bool valor) {
    if (valor) return "si";
    return "no";
}

string leerPalabra(const string& pregunta) {
    string texto = "";
    cout << pregunta;
    cin >> texto;
    return texto;
}

// Devuelve el número que escribió el usuario, o -1 si escribió otra cosa
int leerEntero(const string& pregunta) {
    string texto = "";
    cout << pregunta;
    if (!(cin >> texto)) return 0;

    int largo = texto.length();
    int i = 0;
    while (i < largo) {
        if (texto[i] < '0' || texto[i] > '9') return -1;
        i++;
    }
    return textoAEntero(texto);
}

// 5 -> "PD-000005"
string idConNumero(int n) {
    string digitos = "";
    int i = 0;
    while (i < 6) {
        digitos = char('0' + n % 10) + digitos;
        n = n / 10;
        i++;
    }
    return "PD-" + digitos;
}

// ---------------------- menú ----------------------

void mostrarMenu() {
    cout << "========== SISTEMA DE PEDIDOS ==========" << endl;
    cout << " 1. Cargar pedidos desde un archivo" << endl;
    cout << " 2. Registrar un pedido nuevo" << endl;
    cout << " 3. Buscar un pedido por ID" << endl;
    cout << " 4. Despachar siguiente lote" << endl;
    cout << " 5. Ver pedidos ordenados por fecha y hora (primeros 30)" << endl;
    cout << " 6. Consultar por rango de fechas" << endl;
    cout << " 7. Actualizar el estado de un pedido" << endl;
    cout << " 8. Cancelar un pedido" << endl;
    cout << " 9. Estadisticas generales" << endl;
    cout << "10. Pruebas de sustentacion (con tiempos)" << endl;
    cout << " 0. Salir" << endl;
    cout << "========================================" << endl;
}

void cargarArchivo(Sistema& s) {
    cout << "1) " << RUTA_PEQUENO << endl;
    cout << "2) " << RUTA_MASIVO << endl;
    cout << "3) Escribir otra ruta" << endl;
    int eleccion = leerEntero("Que archivo quieres cargar? ");

    string ruta = "";
    if (eleccion == 1) {
        ruta = RUTA_PEQUENO;
    } else if (eleccion == 2) {
        ruta = RUTA_MASIVO;
    } else if (eleccion == 3) {
        ruta = leerPalabra("Ruta del archivo: ");
    } else {
        cout << "Opcion no valida" << endl;
        return;
    }

    Momento inicio = ahora();
    int cargados = s.cargarCSV(ruta);
    Momento fin = ahora();

    cout << "Pedidos cargados: " << cargados << " en ";
    imprimirMs(aMs(inicio, fin));
    cout << endl;
    if (cargados == 0) {
        cout << "(Si ya habias cargado ese archivo, los ids repetidos se rechazan)" << endl;
    }
}

void ingresarPedidoManual(Sistema& s) {
    string id = leerPalabra("ID del pedido (ej: PD-900001): ");
    string dni = leerPalabra("DNI del cliente: ");
    string tipo = leerPalabra("Tipo de envio (Normal, Express, SameDay): ");

    int prioridad = leerEntero("Nivel de prioridad (1 a 10): ");
    if (prioridad < 1 || prioridad > 10) {
        cout << "Prioridad no valida" << endl;
        return;
    }

    double monto = textoADecimal(leerPalabra("Monto en dolares (ej: 45.50): "));
    if (monto <= 0) {
        cout << "Monto no valido" << endl;
        return;
    }

    string fecha = leerPalabra("Fecha (AAAA-MM-DD): ");
    if (!esFechaValida(fecha)) {
        cout << "Fecha no valida" << endl;
        return;
    }

    string hora = leerPalabra("Hora (HH:MM:SS): ");
    if (!esHoraValida(hora)) {
        cout << "Hora no valida" << endl;
        return;
    }

    Pedido* nuevo = crearPedido(id, dni, tipo, prioridad, monto, fecha, hora, ESTADO_PENDIENTE);
    if (s.registrarPedido(nuevo)) {
        cout << "Pedido registrado" << endl;
    } else {
        cout << "No se registro: ese ID ya existe" << endl;
    }
}

void buscarPedido(Sistema& s) {
    string id = leerPalabra("ID del pedido: ");
    s.buscarPorId(id);
}

void despacharPedidos(Sistema& s) {
    int n = leerEntero("Cuantos pedidos quieres despachar? ");
    if (n <= 0) {
        cout << "Numero no valido" << endl;
        return;
    }
    s.despacharLote(n);
}

void consultarRango(Sistema& s) {
    string desde = leerPalabra("Fecha inicial (AAAA-MM-DD): ");
    string hasta = leerPalabra("Fecha final (AAAA-MM-DD): ");

    if (!esFechaValida(desde) || !esFechaValida(hasta)) {
        cout << "Alguna fecha no es valida" << endl;
        return;
    }
    if (claveInicioDia(desde) > claveFinDia(hasta)) {
        cout << "La fecha inicial es mayor que la final" << endl;
        return;
    }
    s.consultaRango(desde, hasta);
}

void cambiarEstado(Sistema& s) {
    string id = leerPalabra("ID del pedido: ");
    cout << "1) " << ESTADO_PENDIENTE << endl;
    cout << "2) " << ESTADO_TRANSITO << endl;
    cout << "3) " << ESTADO_ENTREGADO << endl;
    int eleccion = leerEntero("Nuevo estado (para cancelar usa la opcion 8): ");

    string nuevo = "";
    if (eleccion == 1) nuevo = ESTADO_PENDIENTE;
    else if (eleccion == 2) nuevo = ESTADO_TRANSITO;
    else if (eleccion == 3) nuevo = ESTADO_ENTREGADO;
    else {
        cout << "Opcion no valida" << endl;
        return;
    }

    if (s.actualizarEstado(id, nuevo)) {
        cout << "Estado actualizado a " << nuevo << endl;
    }
}

void cancelarUnPedido(Sistema& s) {
    string id = leerPalabra("ID del pedido a cancelar: ");
    if (s.cancelarPedido(id)) {
        cout << "Pedido cancelado" << endl;
    }
}

// ---------------------- pruebas de sustentación ----------------------

void pruebaCargaMasiva(Sistema& s) {
    cout << "=== PRUEBA 1: carga masiva ===" << endl;
    Momento inicio = ahora();
    cargadosMasivo = s.cargarCSV(RUTA_MASIVO);
    Momento fin = ahora();
    tCarga = aMs(inicio, fin);

    cout << "Pedidos cargados: " << cargadosMasivo << endl;
    cout << "Tiempo de insertar todo en hash, arbol y heap: ";
    imprimirMs(tCarga);
    cout << endl;
    s.mostrarEstadisticas();
    cout << endl;
}

void pruebaBusqueda(Sistema& s) {
    cout << "=== PRUEBA 2: busqueda por ID (hash) ===" << endl;
    string ids[3] = { "PD-000001", idConNumero(cargadosMasivo), "PD-INEXISTENTE" };
    string nombres[3] = { "Primero del archivo", "Ultimo del archivo", "Inventado" };
    const int REPETICIONES = 10000;
    double suma = 0;

    int i = 0;
    while (i < 3) {
        cout << "--- " << nombres[i] << " (" << ids[i] << ") ---" << endl;
        s.buscarPorId(ids[i]);

        // Una sola busqueda dura muy poco, asi que se repite para poder medirla
        int encontrados = 0;
        Momento inicio = ahora();
        int k = 0;
        while (k < REPETICIONES) {
            if (s.obtenerPedido(ids[i]) != nullptr) encontrados++;
            k++;
        }
        Momento fin = ahora();

        double total = aMs(inicio, fin);
        cout << REPETICIONES << " busquedas: ";
        imprimirMs(total);
        cout << "  |  cada una: ";
        imprimirMs(total / REPETICIONES);
        cout << endl;
        suma = suma + total / REPETICIONES;
        i++;
    }
    tBusqueda = suma / 3;
    cout << endl;
}

void pruebaDespacho(Sistema& s) {
    cout << "=== PRUEBA 3: despacho (heap) ===" << endl;
    cout << "Los 10 mas urgentes (la prioridad es el 4to dato de cada linea):" << endl;
    s.despacharLote(10);

    cout << "Despachando 100 pedidos mas (sin mostrarlos, para medir el tiempo)..." << endl;
    Momento inicio = ahora();
    int despachados = s.despacharSinMostrar(100);
    Momento fin = ahora();
    tDespacho = aMs(inicio, fin);

    cout << "Despachados: " << despachados << " en ";
    imprimirMs(tDespacho);
    cout << endl << endl;
}

void pruebaRango(Sistema& s) {
    cout << "=== PRUEBA 4: consultas por rango de fechas (arbol) ===" << endl;
    string nombres[4] = { "Una semana", "Un mes", "Un solo dia (mucha densidad)", "Sin resultados" };
    string desde[4] = { "2026-07-06", "2026-08-01", "2026-09-15", "2027-01-01" };
    string hasta[4] = { "2026-07-12", "2026-08-31", "2026-09-15", "2027-01-31" };
    const int REPETICIONES = 1000;

    int i = 0;
    while (i < 4) {
        cout << "--- " << nombres[i] << ": " << desde[i] << " a " << hasta[i] << " ---" << endl;
        s.consultaRango(desde[i], hasta[i]);

        int cantidad = 0;
        double monto = 0;
        Momento inicio = ahora();
        int k = 0;
        while (k < REPETICIONES) {
            s.calcularRango(desde[i], hasta[i], cantidad, monto);
            k++;
        }
        Momento fin = ahora();

        double cadaUna = aMs(inicio, fin) / REPETICIONES;
        cout << "Tiempo de cada consulta: ";
        imprimirMs(cadaUna);
        cout << endl;
        if (i == 0) tRango = cadaUna;
        i++;
    }
    cout << "(Compara cantidad y monto con data/verificacion_masivo.txt)" << endl << endl;
}

void pruebaConsultaOrdenada(Sistema& s) {
    cout << "=== Consulta ordenada por fecha y hora (primeros 30) ===" << endl;
    Momento inicio = ahora();
    s.consultaOrdenada();
    Momento fin = ahora();
    tOrdenada = aMs(inicio, fin);

    cout << "Tiempo (incluye mostrar en pantalla): ";
    imprimirMs(tOrdenada);
    cout << endl << endl;
}

void pruebaInsercion(Sistema& s) {
    cout << "=== PRUEBA 5: insertar un pedido despues de la carga ===" << endl;
    int cantidadAntes = 0;
    double montoAntes = 0;
    s.calcularRango("2026-09-20", "2026-09-20", cantidadAntes, montoAntes);

    Pedido* nuevo = crearPedido("PD-900001", "12345678", "Express", 9, 150.75,
                                "2026-09-20", "12:00:00", ESTADO_PENDIENTE);
    Momento inicio = ahora();
    bool insertado = s.registrarPedido(nuevo);
    Momento fin = ahora();
    tInsercion = aMs(inicio, fin);

    cout << "Insertado: " << siNo(insertado) << " en ";
    imprimirMs(tInsercion);
    cout << endl;

    cout << "Aparece en la busqueda por ID: " << siNo(s.obtenerPedido("PD-900001") != nullptr) << endl;

    int cantidadDespues = 0;
    double montoDespues = 0;
    s.calcularRango("2026-09-20", "2026-09-20", cantidadDespues, montoDespues);
    cout << "Pedidos del 2026-09-20 en el arbol: antes " << cantidadAntes
         << ", ahora " << cantidadDespues << " (debe subir en 1)" << endl;
    cout << "Monto de ese dia: antes " << montoAntes
         << ", ahora " << montoDespues << " (debe subir 150.75)" << endl;

    Pedido* repetido = crearPedido("PD-900001", "12345678", "Normal", 1, 10.00,
                                   "2026-09-20", "13:00:00", ESTADO_PENDIENTE);
    cout << "Intentar repetir el mismo ID: se registra? " << siNo(s.registrarPedido(repetido))
         << " (debe ser no)" << endl << endl;
}

void pruebaActualizacion(Sistema& s) {
    cout << "=== PRUEBA 6: actualizar el estado de 10 pedidos ===" << endl;
    int actualizados = 0;
    int i = 0;
    Momento inicio = ahora();
    while (i < 10) {
        if (s.actualizarEstado(idConNumero(101 + i), ESTADO_ENTREGADO)) actualizados++;
        i++;
    }
    Momento fin = ahora();
    tActualizacion = aMs(inicio, fin);

    int correctos = 0;
    i = 0;
    while (i < 10) {
        Pedido* p = s.obtenerPedido(idConNumero(101 + i));
        if (p != nullptr && p->estado == ESTADO_ENTREGADO) correctos++;
        i++;
    }
    cout << "Actualizados: " << actualizados << " en ";
    imprimirMs(tActualizacion);
    cout << endl;
    cout << "Verificados con estado " << ESTADO_ENTREGADO << ": " << correctos << " de 10" << endl << endl;
}

void pruebaCancelacion(Sistema& s) {
    cout << "=== PRUEBA 7: cancelar 10 pedidos ===" << endl;
    int cancelados = 0;
    int i = 0;
    Momento inicio = ahora();
    while (i < 10) {
        if (s.cancelarPedido(idConNumero(201 + i))) cancelados++;
        i++;
    }
    Momento fin = ahora();
    tEliminacion = aMs(inicio, fin);

    int yaNoExisten = 0;
    i = 0;
    while (i < 10) {
        if (s.obtenerPedido(idConNumero(201 + i)) == nullptr) yaNoExisten++;
        i++;
    }
    cout << "Cancelados: " << cancelados << " en ";
    imprimirMs(tEliminacion);
    cout << endl;
    cout << "El hash ya no los encuentra: " << yaNoExisten << " de 10" << endl;
    s.mostrarEstadisticas();
    cout << endl;
}

void mostrarTablaTiempos() {
    cout << "=== TABLA DE TIEMPOS (para el informe) ===" << endl;
    cout << "Operacion                          | Datos            | Tiempo" << endl;
    cout << "Carga masiva                       | " << cargadosMasivo << " pedidos   | "; imprimirMs(tCarga); cout << endl;
    cout << "Busqueda por ID (promedio de una)  | " << cargadosMasivo << " pedidos   | "; imprimirMs(tBusqueda); cout << endl;
    cout << "Insercion individual               | " << cargadosMasivo << " pedidos   | "; imprimirMs(tInsercion); cout << endl;
    cout << "Despacho de 100 pedidos urgentes   | 100 pedidos      | "; imprimirMs(tDespacho); cout << endl;
    cout << "Consulta ordenada (30)             | " << cargadosMasivo << " pedidos   | "; imprimirMs(tOrdenada); cout << endl;
    cout << "Consulta por rango (una semana)    | " << cargadosMasivo << " pedidos   | "; imprimirMs(tRango); cout << endl;
    cout << "Actualizacion                      | 10 pedidos       | "; imprimirMs(tActualizacion); cout << endl;
    cout << "Eliminacion                        | 10 pedidos       | "; imprimirMs(tEliminacion); cout << endl;
}

void ejecutarPruebas() {
    Sistema s;   // uno nuevo, para no mezclar con lo que ya se cargó

    pruebaCargaMasiva(s);
    if (cargadosMasivo == 0) {
        cout << "No se pudo cargar el archivo masivo, no se puede seguir." << endl;
        return;
    }
    pruebaBusqueda(s);
    pruebaDespacho(s);
    pruebaRango(s);
    pruebaConsultaOrdenada(s);
    pruebaInsercion(s);
    pruebaActualizacion(s);
    pruebaCancelacion(s);
    mostrarTablaTiempos();
    cout << "Colisiones en el hash: " << s.getColisiones() << endl;
}

// ---------------------- programa principal ----------------------

int main() {
    cout.setf(ios::fixed);
    cout.precision(2);

    Sistema sistema;
    bool seguir = true;

    while (seguir) {
        mostrarMenu();
        int opcion = leerEntero("Elige una opcion: ");
        cout << endl;

        if (opcion == 1) cargarArchivo(sistema);
        else if (opcion == 2) ingresarPedidoManual(sistema);
        else if (opcion == 3) buscarPedido(sistema);
        else if (opcion == 4) despacharPedidos(sistema);
        else if (opcion == 5) sistema.consultaOrdenada();
        else if (opcion == 6) consultarRango(sistema);
        else if (opcion == 7) cambiarEstado(sistema);
        else if (opcion == 8) cancelarUnPedido(sistema);
        else if (opcion == 9) sistema.mostrarEstadisticas();
        else if (opcion == 10) ejecutarPruebas();
        else if (opcion == 0) seguir = false;
        else cout << "Opcion no valida" << endl;

        cout << endl;
    }

    cout << "Hasta luego" << endl;
    return 0;
}
