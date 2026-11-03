"""
generar_datos.py

Genera los archivos de prueba del proyecto EDA II:
    data/pedidos_pequeno.csv   (verificacion manual)
    data/pedidos_masivo.csv    (prueba de estres)
    data/verificacion_pequeno.txt
    data/verificacion_masivo.txt   (respuestas esperadas para comparar con tu programa)

Formato de cada linea (sin encabezado):
    idPedido,dniCliente,tipoEnvio,nivelPrioridad,montoDolares,fecha,hora,estado
    PD-000001,45879123,Normal,1,45.50,2026-06-01,08:30:00,Pendiente

Uso:
    python generar_datos.py
    python generar_datos.py 50000        (cantidad del archivo masivo)
"""

import random
import sys
import os
from datetime import date, timedelta

# ------------------------- CONFIGURACION -------------------------
CANTIDAD_PEQUENO = 30          # entre 20 y 50
CANTIDAD_MASIVO = 10000        # al menos 10,000

# La carpeta 'data' se crea junto a este script (no en el directorio desde donde se ejecuta)
try:
    CARPETA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
except NameError:
    CARPETA = os.path.join(os.path.expanduser("~"), "Desktop", "data")

# Pedidos del archivo pequeno: se reparten en estos dias (para verificar a mano)
PEQUENO_INICIO = date(2026, 6, 1)
PEQUENO_DIAS = 10

# Pedidos del archivo masivo: se reparten en estos dias
MASIVO_INICIO = date(2026, 6, 1)
MASIVO_DIAS = 153              # junio a octubre de 2026

# (tipoEnvio, prioridad minima, prioridad maxima, peso, monto minimo, monto maximo)
TIPOS = [
    ("Normal",  1,  3, 60,   5,  300),
    ("Express", 4,  7, 30,  50, 1500),
    ("SameDay", 8, 10, 10,  20,  500),
]

# (estado, peso)  -> la mayoria quedan Pendiente para que el heap tenga datos
ESTADOS = [("Pendiente", 70), ("EnTransito", 15), ("Entregado", 15)]

# Pedidos fijos del enunciado (se usan al inicio del archivo pequeno)
EJEMPLOS_ENUNCIADO = [
    ("PD-000001", "45879123", "Normal",  1, 4550,   "2026-06-01", "08:30:00", "Pendiente"),
    ("PD-000002", "77842159", "Express", 5, 120000, "2026-06-01", "09:15:20", "Pendiente"),
    ("PD-000003", "14578963", "SameDay", 10, 8599,  "2026-06-02", "14:20:15", "EnTransito"),
]
# (los montos van en centavos dentro del script: 4550 = 45.50)


# ------------------------- FUNCIONES -------------------------
def formatear_monto(centavos):
    """4550 -> '45.50'"""
    return str(centavos // 100) + "." + str(centavos % 100).zfill(2)


def hora_aleatoria():
    seg = random.randint(0, 24 * 3600 - 1)
    return str(seg // 3600).zfill(2) + ":" + str(seg % 3600 // 60).zfill(2) + ":" + str(seg % 60).zfill(2)


def generar_pedido(numero, inicio, dias):
    """Retorna una tupla con los 8 campos. El monto va en centavos."""
    nombres = [t[0] for t in TIPOS]
    pesos = [t[3] for t in TIPOS]
    tipo = random.choices(TIPOS, weights=pesos)[0]

    id_pedido = "PD-" + str(numero).zfill(6)
    dni = str(random.randint(10000000, 79999999))
    prioridad = random.randint(tipo[1], tipo[2])
    centavos = random.randint(tipo[4] * 100, tipo[5] * 100)
    fecha = (inicio + timedelta(days=random.randint(0, dias - 1))).isoformat()
    hora = hora_aleatoria()
    estado = random.choices([e[0] for e in ESTADOS], weights=[e[1] for e in ESTADOS])[0]
    return (id_pedido, dni, tipo[0], prioridad, centavos, fecha, hora, estado)


def generar_lista(cantidad, inicio, dias, fijos=None):
    filas = []
    if fijos:
        filas.extend(fijos)
    for n in range(len(filas) + 1, cantidad + 1):
        filas.append(generar_pedido(n, inicio, dias))

    # Fuerza algunos pedidos con la MISMA fecha y hora (prueba el desempate del arbol)
    if cantidad >= 20:
        for destino, origen in [(cantidad - 1, 4), (cantidad - 3, 9), (cantidad - 5, 9)]:
            f = list(filas[destino])
            f[5] = filas[origen][5]
            f[6] = filas[origen][6]
            filas[destino] = tuple(f)
    return filas


def escribir_csv(ruta, filas):
    with open(ruta, "w", encoding="utf-8", newline="\n") as f:
        for r in filas:
            f.write(",".join([r[0], r[1], r[2], str(r[3]), formatear_monto(r[4]), r[5], r[6], r[7]]) + "\n")


def consulta_rango(filas, ini, fin):
    """Cantidad y monto (en centavos) de los pedidos con fecha entre ini y fin (inclusive)."""
    cant = 0
    total = 0
    for r in filas:
        if ini <= r[5] <= fin:
            cant += 1
            total += r[4]
    return cant, total


def escribir_verificacion(ruta, nombre, filas, rangos, seed):
    total_centavos = sum(r[4] for r in filas)
    mayores = sorted([r[3] for r in filas], reverse=True)[:10]
    with open(ruta, "w", encoding="utf-8", newline="\n") as f:
        f.write("VERIFICACION de " + nombre + "  (semilla: " + str(seed) + ")\n\n")
        f.write("--- Estadisticas esperadas tras la carga ---\n")
        f.write("Total de pedidos:       " + str(len(filas)) + "\n")
        f.write("Pedidos en el heap:     " + str(len(filas)) + "\n")
        f.write("Monto total acumulado:  " + formatear_monto(total_centavos) + "\n\n")
        f.write("--- Busqueda por ID ---\n")
        f.write("Primero:   " + filas[0][0] + "\n")
        f.write("Ultimo:    " + filas[-1][0] + "\n")
        f.write("Inventado: PD-999999 (no debe existir)\n\n")
        f.write("--- Despacho: las 10 prioridades mas altas (en orden) ---\n")
        f.write(", ".join(str(p) for p in mayores) + "\n\n")
        f.write("--- Consultas por rango (inclusive) ---\n")
        for etiqueta, ini, fin in rangos:
            cant, total = consulta_rango(filas, ini, fin)
            f.write(etiqueta + ": " + ini + " a " + fin + "  ->  cantidad = " + str(cant)
                    + ",  monto = " + formatear_monto(total) + "\n")
    return total_centavos


# ------------------------- PROGRAMA PRINCIPAL -------------------------
def main():
    cantidad_masivo = CANTIDAD_MASIVO
    if len(sys.argv) > 1:
        cantidad_masivo = int(sys.argv[1])
    if cantidad_masivo < 10000:
        print("Aviso: el enunciado pide al menos 10,000 pedidos en el masivo.")

    seed = random.randint(1, 10**9)
    random.seed(seed)

    os.makedirs(CARPETA, exist_ok=True)

    # ---- Archivo pequeno ----
    pequeno = generar_lista(CANTIDAD_PEQUENO, PEQUENO_INICIO, PEQUENO_DIAS, fijos=EJEMPLOS_ENUNCIADO)
    escribir_csv(os.path.join(CARPETA, "pedidos_pequeno.csv"), pequeno)
    rangos_peq = [
        ("Rango 1 (3 dias)", "2026-06-01", "2026-06-03"),
        ("Rango 2 (1 semana)", "2026-06-01", "2026-06-07"),
        ("Rango sin resultados", "2027-01-01", "2027-01-31"),
    ]
    escribir_verificacion(os.path.join(CARPETA, "verificacion_pequeno.txt"),
                          "pedidos_pequeno.csv", pequeno, rangos_peq, seed)

    # ---- Archivo masivo ----
    masivo = generar_lista(cantidad_masivo, MASIVO_INICIO, MASIVO_DIAS)
    escribir_csv(os.path.join(CARPETA, "pedidos_masivo.csv"), masivo)
    rangos_mas = [
        ("Rango de una semana", "2026-07-06", "2026-07-12"),
        ("Rango de un mes", "2026-08-01", "2026-08-31"),
        ("Rango de alta densidad (un dia)", "2026-09-15", "2026-09-15"),
        ("Rango sin resultados", "2027-01-01", "2027-01-31"),
    ]
    escribir_verificacion(os.path.join(CARPETA, "verificacion_masivo.txt"),
                          "pedidos_masivo.csv", masivo, rangos_mas, seed)

    print("Listo. Archivos creados en la carpeta '" + CARPETA + "':")
    print("  pedidos_pequeno.csv       (" + str(len(pequeno)) + " pedidos)")
    print("  pedidos_masivo.csv        (" + str(len(masivo)) + " pedidos)")
    print("  verificacion_pequeno.txt / verificacion_masivo.txt")
    print("Semilla usada: " + str(seed))


main()