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
│   └── main.cpp             # menú + pruebas de tiempos
├── data/
│   ├── pedidos_pequeno.csv  # 20 a 50 pedidos
│   └── pedidos_masivo.csv   # 10,000 o más pedidos
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
| 6 | `Sistema.cpp` | HashTable, MaxHeap, RedBlackTree, Utilidades |
| 7 | `main.cpp` | Sistema |

Hash, Heap y Árbol **no se conocen entre sí**. Solo `Sistema` los une.

---

## 3. Cómo se arma el programa final (un solo main)

Cada integrante trabaja en su propio archivo, y al final **todo se pega en un único `main.cpp`** en este orden:

```
1. #include <iostream>, <string>, <vector> + using namespace std;
2. Utilidades
3. Pedido
4. HashTable
5. MaxHeap
6. RedBlackTree
7. Sistema
8. main()
```

Reglas para que el pegado salga sin errores:

- Cada bloque solo usa lo que está **antes** en el orden.
- Los `#include` y `using namespace std;` repetidos al inicio de cada archivo se borran al pegar (dejar uno solo arriba).
- Los archivos sueltos no llevan `#include` de otros archivos ni guards.
- Cada uno prueba su parte pegando antes lo que necesita (como mínimo `Utilidades` y `Pedido`).

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

---

## 7. Acuerdos que los 3 deben respetar

- **Clave del hash:** `idPedido` (string, ejemplo `PD-000001`).
- **Clave del heap:** `nivelPrioridad` (mayor sale primero).
- **Clave del árbol:** `claveFecha` (ejemplo `20260601083000`); si empatan, desempata por `idPedido`.
- **Estados (lista única, constantes en `Utilidades.cpp`):** `Pendiente`, `EnTransito`, `Entregado`, `Cancelado`. *(Confirmar entre los 3 antes de programar; el enunciado sugiere Iniciado, Enviado, En camino, Entregado, Cancelado.)*
- **Rango de fechas:** el usuario da `2026-06-01`; convertir a clave inicial con `000000` y final con `235959`.
- **Contadores** (`totalPedidos`, `montoAcumulado`): solo se tocan dentro de `Sistema`, nunca dentro de las estructuras.
- **Tiempos:** se miden solo en `Sistema`/`main`, nunca dentro de las estructuras.

---

## 8. Reparto sugerido

| Integrante | Archivo principal | Archivos de apoyo |
|-----------|-------------------|-------------------|
| 1 | `HashTable.cpp` | `Utilidades.cpp` (`hashString`) |
| 2 | `MaxHeap.cpp` | `Pedido.cpp` |
| 3 | `RedBlackTree.cpp` | `Utilidades.cpp` (fechas) |
| Todos | `Sistema.cpp`, `main.cpp`, informe | |

Cada uno prueba su estructura con un `main` temporal **fuera** de `src/`. Nadie modifica `Pedido.cpp` ni `Utilidades.cpp` sin avisar.

---

## 9. Pruebas obligatorias (para `main.cpp`)

1. Carga masiva con tiempo (Hash + Árbol + Heap).
2. Búsqueda: primero del archivo, último y uno inventado, con milisegundos.
3. Despacho de los 10 más urgentes y verificar prioridades.
4. Rango: una semana, un mes y uno sin resultados; verificar el monto.
5. Inserción individual y verificar en hash y árbol.
6. Actualizar estado de 10 pedidos.
7. Cancelar 10 pedidos y verificar que el hash ya no los encuentra.
