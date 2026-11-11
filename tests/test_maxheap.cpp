// Desde la raiz del repositorio, en PowerShell:
// g++ -std=c++11 -Wall -Wextra -pedantic -O0 -g tests/test_maxheap.cpp -o tests/test_maxheap.exe
// .\tests\test_maxheap.exe
// Compilar SOLO este archivo: incluye MaxHeap.cpp, Pedido.cpp y Utilidades.cpp.
// No agregar src/main.cpp ni los otros .cpp como unidades de compilacion.
// Opcional, si el compilador admite sanitizadores de memoria:
// g++ -std=c++11 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_maxheap.cpp -o tests/test_maxheap_asan.exe
// .\tests\test_maxheap_asan.exe
// La deteccion de fugas depende del soporte del sanitizador en la plataforma.

#include <exception>
#include <iomanip>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "../src/MaxHeap.cpp"

static_assert(!std::is_copy_constructible<MaxHeap>::value,
              "MaxHeap no debe permitir copia superficial");
static_assert(!std::is_copy_assignable<MaxHeap>::value,
              "MaxHeap no debe permitir asignacion superficial");

namespace {
    void comprobar(bool condicion, const char* mensaje) {
        // No usar assert: las comprobaciones siguen activas con -DNDEBUG.
        if (!condicion) throw std::runtime_error(mensaje);
    }

    std::unique_ptr<Pedido> nuevoPedido(int numero, int prioridad,
                                      const std::string& estado = ESTADO_PENDIENTE) {
        std::ostringstream id;
        id << "PD-" << std::setw(6) << std::setfill('0') << numero;
        const std::string tipo = prioridad <= 3 ? "Normal"
                               : prioridad <= 7 ? "Express" : "SameDay";
        return std::unique_ptr<Pedido>(crearPedido(id.str(), "45879123", tipo,
            prioridad, 100.50, "2026-06-01", "08:30:00", estado));
    }

    // Las colecciones STL son solo auxiliares de prueba; el Heap usa su arreglo.
    struct Escenario {
        // El heap se destruye antes que los propietarios de pedidos validos.
        std::vector<std::unique_ptr<Pedido> > propietarios;
        MaxHeap heap;

        explicit Escenario(int capacidad) : heap(capacidad) {}

        ~Escenario() {
            // Drenar tambien si una comprobacion lanza: el Heap libera cancelados
            // transferidos; los propietarios liberan validos, extraidos o no.
            while (heap.extraerMax() != nullptr) {}
        }

        Pedido* insertar(int prioridad, const std::string& estado = ESTADO_PENDIENTE) {
            propietarios.push_back(nuevoPedido(
                static_cast<int>(propietarios.size()) + 1, prioridad, estado));
            Pedido* p = propietarios.back().get();
            heap.insertar(p);
            return p;
        }

        void cancelar(Pedido* p) {
            for (std::size_t i = 0; i < propietarios.size(); ++i) {
                if (propietarios[i].get() == p) {
                    // Se usa solo sobre pedidos que siguen dentro del Heap.
                    // Cambiar estado antes de transferir: string puede lanzar.
                    p->estado = ESTADO_CANCELADO;
                    propietarios[i].release();
                    return;
                }
            }
            throw std::runtime_error("Pedido sin propietario o ya transferido");
        }
    };

    void comprobarVacio(MaxHeap& heap) {
        comprobar(heap.estaVacio(), "El Heap debe estar vacio");
        comprobar(heap.getTamano() == 0, "El tamano vacio debe ser cero");
        comprobar(heap.verMax() == nullptr, "verMax vacio debe devolver nullptr");
        comprobar(heap.extraerMax() == nullptr, "Extraer vacio debe devolver nullptr");
    }

    void comprobarExtracciones(Escenario& e, std::set<Pedido*> esperados,
                               const std::vector<int>& prioridades) {
        comprobar(esperados.size() == prioridades.size(), "Expectativas inconsistentes");
        for (std::size_t i = 0; i < prioridades.size(); ++i) {
            Pedido* p = e.heap.extraerMax();
            comprobar(p != nullptr, "Falta un pedido valido");
            // Comprobar identidad antes de desreferenciar un posible puntero ajeno.
            comprobar(esperados.erase(p) == 1, "Puntero ajeno o extraido dos veces");
            comprobar(p->nivelPrioridad == prioridades[i], "Orden de prioridades incorrecto");
            comprobar(p->estado != ESTADO_CANCELADO, "Se devolvio un cancelado");
        }
        comprobar(esperados.empty(), "Se perdieron pedidos");
        // Esta extraccion final tambien drena cancelados debajo del ultimo valido.
        comprobar(e.heap.extraerMax() == nullptr, "Se devolvieron pedidos adicionales");
        comprobarVacio(e.heap);
    }

    void vacioYNullptr() {
        Escenario e(1);
        comprobarVacio(e.heap);
        e.heap.insertar(nullptr);
        comprobarVacio(e.heap);
        Pedido* p = e.insertar(4);
        e.heap.insertar(nullptr);
        comprobar(e.heap.getTamano() == 1, "nullptr cambio el tamano");
        comprobar(e.heap.verMax() == p, "nullptr cambio la raiz");
    }

    void unPedido() {
        Escenario e(1);
        Pedido* p = e.insertar(7);
        comprobar(!e.heap.estaVacio(), "Insercion no cambio estaVacio");
        comprobar(e.heap.getTamano() == 1, "Tamano tras insertar incorrecto");
        comprobar(e.heap.verMax() == p, "verMax no conserva el puntero");
        comprobar(e.heap.verMax() == p, "verMax tiene efectos secundarios");
        comprobar(e.heap.getTamano() == 1, "verMax modifica el tamano");
        comprobar(e.heap.extraerMax() == p, "Extraccion no conserva el puntero");
        comprobar(p->idPedido == "PD-000001", "El pedido valido fue alterado");
        comprobarVacio(e.heap);
    }

    void probarOrden(const std::vector<int>& entrada) {
        Escenario e(2);
        std::set<Pedido*> esperados;
        int cantidades[11] = {};
        for (std::size_t i = 0; i < entrada.size(); ++i) {
            esperados.insert(e.insertar(entrada[i]));
            ++cantidades[entrada[i]];
            comprobar(e.heap.getTamano() == static_cast<int>(i + 1), "Tamano de insercion incorrecto");
        }
        std::vector<int> salida;
        for (int prioridad = 10; prioridad >= 1; --prioridad) {
            for (int i = 0; i < cantidades[prioridad]; ++i) salida.push_back(prioridad);
        }
        comprobarExtracciones(e, esperados, salida);
    }

    void ascendentes() { probarOrden({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}); }
    void descendentes() { probarOrden({10, 9, 8, 7, 6, 5, 4, 3, 2, 1}); }
    void mezcladas() { probarOrden({3, 10, 1, 7, 4, 9, 2, 8, 6, 5}); }
    // Solo se exige recuperar todos los punteros: no hay contrato FIFO ni por ID.
    void empates() { probarOrden({5, 5, 5, 5, 5, 5, 5}); }

    void crecimiento() {
        // Desde capacidad 2 cruza tambien 1024, 2048 y 4096.
        std::vector<int> entrada;
        for (int i = 0; i < 4097; ++i) entrada.push_back((i * 7) % 10 + 1);
        probarOrden(entrada);
    }

    void capacidadesNoPositivas() {
        const int capacidades[] = {0, -10};
        for (int i = 0; i < 2; ++i) {
            Escenario e(capacidades[i]);
            comprobarVacio(e.heap);
            Pedido* a = e.insertar(2);
            Pedido* b = e.insertar(8);
            comprobarExtracciones(e, {a, b}, {8, 2});
        }
    }

    void intercaladas() {
        Escenario e(1);
        Pedido* a = e.insertar(3);
        Pedido* b = e.insertar(8);
        comprobar(e.heap.extraerMax() == b, "Primer despacho intercalado incorrecto");
        comprobar(e.heap.getTamano() == 1, "Tamano tras extraer incorrecto");
        Pedido* c = e.insertar(10);
        Pedido* d = e.insertar(1);
        comprobar(e.heap.extraerMax() == c, "Nueva prioridad maxima ignorada");
        comprobar(e.heap.extraerMax() == a, "Se perdio el pedido anterior");
        Pedido* f = e.insertar(7);
        comprobarExtracciones(e, {d, f}, {7, 1});
        Pedido* g = e.insertar(4);
        comprobarExtracciones(e, {g}, {4});
    }

    void cancelados() {
        Escenario e(2);
        Pedido* raiz = e.insertar(10);
        Pedido* siguiente = e.insertar(9);
        Pedido* a = e.insertar(8);
        Pedido* interno = e.insertar(7);
        Pedido* b = e.insertar(4);
        Pedido* hoja = e.insertar(1);
        e.cancelar(raiz);
        e.cancelar(siguiente);
        e.cancelar(interno);
        e.cancelar(hoja);
        comprobar(e.heap.verMax() == raiz, "verMax debe mostrar la raiz cancelada");
        comprobar(e.heap.verMax()->estado == ESTADO_CANCELADO, "Estado no compartido");
        comprobar(e.heap.getTamano() == 6, "La consulta elimino cancelados");
        comprobar(!e.heap.estaVacio(), "Hay elementos fisicos en el Heap");
        comprobarExtracciones(e, {a, b}, {8, 4});
        // No volver a leer ni liberar los punteros transferidos al Heap.
    }

    void todosCancelados() {
        Escenario e(1);
        for (int i = 1; i <= 10; ++i) e.cancelar(e.insertar(i));
        comprobar(e.heap.getTamano() == 10, "Tamano fisico de cancelados incorrecto");
        comprobar(!e.heap.estaVacio(), "Cancelados aun ocupan el arreglo");
        comprobar(e.heap.extraerMax() == nullptr, "Se devolvio un cancelado");
        comprobarVacio(e.heap);
        // Reutilizar el Heap despues de descartar todos los cancelados.
        Pedido* p = e.insertar(6);
        comprobarExtracciones(e, {p}, {6});
    }

    void otrosEstados() {
        Escenario e(1);
        Pedido* a = e.insertar(9, ESTADO_ENTREGADO);
        Pedido* b = e.insertar(7, ESTADO_TRANSITO);
        Pedido* c = e.insertar(2);
        comprobarExtracciones(e, {a, b, c}, {9, 7, 2});
        comprobar(a->estado == ESTADO_ENTREGADO, "El Heap cambio el estado Entregado");
        comprobar(b->estado == ESTADO_TRANSITO, "El Heap cambio el estado EnTransito");
    }

    void destructorConservaPedidos() {
        std::unique_ptr<Pedido> a = nuevoPedido(1, 4);
        std::unique_ptr<Pedido> b = nuevoPedido(2, 9);
        {
            MaxHeap heap(1);
            heap.insertar(a.get());
            heap.insertar(b.get());
            comprobar(heap.extraerMax() == b.get(), "Puntero extraido distinto");
            // Dejar a dentro para comprobar el destructor sin drenarlo.
        }
        comprobar(a->nivelPrioridad == 4 && a->idPedido == "PD-000001",
                  "El destructor altero o libero el pedido almacenado");
        comprobar(b->nivelPrioridad == 9 && b->idPedido == "PD-000002",
                  "El Heap altero o libero el pedido extraido");
        // Los dos unique_ptr son los unicos propietarios y los liberan aqui.
    }

    void limpiezaConCanceladosPendientes() {
        Escenario e(1);
        e.insertar(8);
        Pedido* p = e.insertar(3);
        e.cancelar(p);
        // El destructor de Escenario drena, incluso cancelados bajo un valido.
        // Ejecutar con sanitizadores para detectar fugas o dobles liberaciones.
    }
}

int main() {
    struct Prueba { const char* nombre; void (*ejecutar)(); };
    const Prueba pruebas[] = {
        {"Heap vacio y rechazo de nullptr", vacioYNullptr},
        {"Un pedido y consulta sin efectos secundarios", unPedido},
        {"Prioridades ascendentes", ascendentes},
        {"Prioridades descendentes", descendentes},
        {"Prioridades mezcladas", mezcladas},
        {"Empates sin imponer desempate", empates},
        {"Crecimiento con 4097 pedidos", crecimiento},
        {"Capacidades iniciales cero y negativa", capacidadesNoPositivas},
        {"Inserciones y extracciones intercaladas", intercaladas},
        {"Cancelados en raiz, interior y hoja", cancelados},
        {"Todos cancelados y reutilizacion", todosCancelados},
        {"EnTransito y Entregado siguen siendo extraibles", otrosEstados},
        {"Destructor conserva pedidos activos y extraidos", destructorConservaPedidos},
        {"Limpieza de cancelados pendientes", limpiezaConCanceladosPendientes}
    };
    int fallos = 0;
    const std::size_t total = sizeof(pruebas) / sizeof(pruebas[0]);
    for (std::size_t i = 0; i < total; ++i) {
        try {
            pruebas[i].ejecutar();
            std::cout << "[OK] " << pruebas[i].nombre << '\n';
        } catch (const std::exception& error) {
            ++fallos;
            std::cerr << "[FALLO] " << pruebas[i].nombre << ": " << error.what() << '\n';
        } catch (...) {
            ++fallos;
            std::cerr << "[FALLO] " << pruebas[i].nombre << ": excepcion desconocida\n";
        }
    }
    std::cout << "Resultado: " << total - fallos << '/' << total << " pruebas correctas\n";
    return fallos == 0 ? 0 : 1;
}
