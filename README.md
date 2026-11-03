# Proyecto EDA II: Sistema Masivo de Gestión Logística y Despacho de Pedidos

Estructura general del proyecto (sin archivos `.h`). El detalle de funciones de cada archivo está en `2_ESTRUCTURA_INTERNA.md`.

---

## 1. Árbol de archivos

```
ProyectoEDA2/
├── src/
│   ├── Utilidades.cpp       # constantes, fechas, split, hashString
│   ├── Pedido.cpp           # struct Pedido + comparadores
│   ├── HashTable.cpp        # HashNode + HashTable
│   ├── MaxHeap.cpp          # MaxHeap (arreglo de Pedido*)
│   ├── RedBlackTree.cpp     # NodoRB + ArbolRedBlack
│   ├── Sistema.cpp          # une las 3 estructuras + cargarCSV
│   └── main.cpp             # menú + pruebas
├── data/
│   ├── pedidos_pequeno.csv       # 20 a 50 pedidos
│   ├── pedidos_masivo.csv        # 10,000 o más pedidos
│   ├── verificacion_pequeno.txt  # resultados esperados (para comparar)
│   └── verificacion_masivo.txt
├── generar_datos.py         # genera los CSV y los archivos de verificación
└── informe/
    └── informe_IEEE.pdf
```

Entrega final: un único `.zip` con informe PDF, código fuente y los dos CSV.

---

## 2. Orden de dependencias

Cada archivo solo puede usar lo que está **antes** en esta lista:

| # | Archivo | Depende de |
|---|---------|-----------|
| 1 | `Utilidades.cpp` | nada |
| 2 | `Pedido.cpp` | Utilidades |
| 3 | `HashTable.cpp` | Pedido, Utilidades |
| 4 | `MaxHeap.cpp` | Pedido |
| 5 | `RedBlackTree.cpp` | Pedido |
| 6 | `Sistema.cpp` | Utilidades, Pedido, HashTable, MaxHeap, RedBlackTree |
| 7 | `main.cpp` | Sistema |

Hash, Heap y Árbol **no se conocen entre sí**. Solo `Sistema` los une.

---

## 3. Cómo se arma el programa

### Fase 1: mientras trabajan (archivos separados)

Así están los archivos ahora:

- Cada `.cpp` lleva `#pragma once` (para que no se incluya dos veces).
- Cada `.cpp` hace `#include "Otro.cpp"` de los archivos de los que depende (según la tabla de la sección 2).
- `main.cpp` hace `#include "Sistema.cpp"`, que arrastra todo lo demás.
- **Solo se compila `main.cpp`:** `g++ main.cpp -o sistema`. Si se agregan los demás `.cpp` a la compilación, sale error de "definición múltiple".
- Cada uno prueba su parte con un `main` temporal que incluya su archivo.

### Fase 2: entrega (un solo `main.cpp`)

Se pega todo en un único archivo, en este orden:

```
1. #include <iostream>, <string> + using namespace std;
2. Utilidades
3. Pedido
4. HashTable
5. MaxHeap
6. RedBlackTree
7. Sistema
8. main()
```

Y se borra:

- Todos los `#include "...cpp"`.
- Todos los `#pragma once`.
- Los `#include <...>` y `using namespace std;` repetidos (queda uno solo arriba).
- `#include <vector>` solo se deja si el hash lo sigue usando (ver sección 7).

---

## 4. Regla de oro: todos guardan `Pedido*`

Existe **un solo objeto `Pedido`** en memoria por pedido. Hash, Heap y Árbol guardan el **mismo puntero**.

Consecuencia: si se actualiza el estado desde el hash, el árbol y el heap ven el cambio automáticamente.

---

## 5. Quién hace `delete` de qué

| Elemento | Quién lo libera |
|----------|-----------------|
| Nodos del hash (`HashNode`) | destructor de `HashTable` |
| Nodos del árbol (`NodoRB`) y `TNULL` | destructor de `ArbolRedBlack` |
| Arreglo interno del heap (`arr`) | destructor de `MaxHeap` |
| Objetos `Pedido` | **solo `Sistema`** (o el heap al descartar un cancelado) |

Los destructores de las 3 estructuras **nunca** hacen `delete` de los `Pedido`. Así se evita el doble `delete`.

---

## 6. Flujo de cada operación (qué estructuras toca)

| Operación | Hash | Heap | Árbol |
|-----------|:----:|:----:|:-----:|
| Carga masiva / registro nuevo | verifica e inserta | inserta | inserta |
| Búsqueda por ID | busca | | |
| Despachar lote | | `extraerMax` | |
| Consulta ordenada (30) | | | `inOrder` |
| Consulta por rango | | | `consultaRango` |
| Actualizar estado | busca el puntero | | |
| Cancelar pedido | elimina | marca `Cancelado` | elimina |
| Estadísticas | | `getTamano` | |

### Cancelación (punto crítico)

1. Buscar el pedido en el hash (obtiene el `Pedido*`).
2. Eliminarlo del hash.
3. Eliminarlo del árbol (usa `claveFecha` + `idPedido`).
4. Poner `estado = "Cancelado"` (el heap lo marca así, no se borra).
5. **No hacer `delete` aún**: el heap todavía apunta a él.
6. Cuando `extraerMax` encuentre un `Cancelado`, lo descarta y ahí sí hace `delete`.
7. Actualizar `totalPedidos` y `montoAcumulado` en `Sistema`.

### Ojo con las estadísticas

El enunciado define "pendientes por despachar" como el tamaño del heap. Pero un pedido cancelado sigue dentro del heap hasta que `extraerMax` lo descarte, así que `getTamano()` lo contaría de más. Hay que decidirlo entre los 3 antes de programar `mostrarEstadisticas`:

- **Opción A:** `Sistema` lleva un contador de pendientes que baja al cancelar o despachar (y se muestra ese, no `getTamano()`).
- **Opción B:** el heap lleva su propio contador de cancelados y `getTamano()` lo descuenta.

---

## 7. Acuerdos que los 3 deben respetar

- **Clave del hash:** `idPedido` (string, ejemplo `PD-000001`).
- **Clave del heap:** `nivelPrioridad` (mayor sale primero).
- **Clave del árbol:** `claveFecha` (ejemplo `20260601083000`, la calcula `calcularClaveFecha`); si empatan, desempata por `idPedido` (`compararPedidos`).
- **Estados (constantes en `Utilidades.cpp`):** `Pendiente`, `EnTransito`, `Entregado`, `Cancelado`. *(El enunciado sugiere Iniciado, Enviado, En camino, Entregado, Cancelado: confirmar si se cambia.)*
- **Rango de fechas:** el usuario da `2026-06-01`; `claveInicioDia` lo convierte a `...000000` y `claveFinDia` a `...235959`.
- **Contadores** (`totalPedidos`, `montoAcumulado`): solo se tocan dentro de `Sistema`, nunca dentro de las estructuras.
- **Tiempos:** se miden solo en `Sistema`/`main`, nunca dentro de las estructuras.
- **`vector` (pendiente de decidir):** el enunciado no lo prohíbe, pero pide estructuras "creadas desde cero con punteros". Lo más seguro es que la tabla del hash sea `HashNode**` (con `new` y `delete[]`) y no usar `vector`.

---

## 8. Estado y reparto

| Archivo | Estado | Responsable |
|---------|--------|-------------|
| `Utilidades.cpp` | implementado | |
| `Pedido.cpp` | esqueleto | |
| `HashTable.cpp` | **vacío** | Integrante 1 |
| `MaxHeap.cpp` | esqueleto | Integrante 2 |
| `RedBlackTree.cpp` | esqueleto | Integrante 3 |
| `Sistema.cpp`, `main.cpp`, informe | esqueleto | Todos |

Nadie modifica `Pedido.cpp` ni `Utilidades.cpp` sin avisar.

---

## 9. Dónde va el CSV

El CSV no va dentro del código: es un archivo aparte y se abre desde `main` con `sistema.cargarCSV("data/pedidos_masivo.csv");`.

- El programa debe ejecutarse **desde la carpeta que contiene `data/`**, o la ruta no se encuentra.
- Si `cargarCSV` retorna 0, casi seguro no abrió el archivo (ruta o carpeta equivocada).
- Mejor no usar rutas absolutas (`C:/Users/...`): fallan en otra computadora.
- Los CSV se generan con `python generar_datos.py` (crea la carpeta `data/` junto al script).

---

## 10. Pruebas obligatorias (para `main.cpp`)

1. Carga masiva con tiempo (Hash + Árbol + Heap).
2. Búsqueda: primero del archivo, último y uno inventado, con milisegundos.
3. Despacho de los 10 más urgentes y verificar prioridades.
4. Rango: una semana, un mes y uno sin resultados; verificar el monto (comparar con `verificacion_masivo.txt`).
5. Inserción individual y verificar en hash y árbol.
6. Actualizar estado de 10 pedidos.
7. Cancelar 10 pedidos y verificar que el hash ya no los encuentra.
