import matplotlib.pyplot as plt
import os

def cargar_rutas(nombre_archivo):
    rutas = []
    with open(nombre_archivo, 'r') as f:
        ruta_actual = []
        for linea in f:
            if linea.startswith("Ruta"):
                if ruta_actual:
                    rutas.append(ruta_actual)
                    ruta_actual = []
            elif linea.strip():
                partes = linea.strip().split()
                x, y = float(partes[1]), float(partes[2])
                ruta_actual.append((x, y))
        if ruta_actual:
            rutas.append(ruta_actual)
    return rutas

def plot_rutas(rutas, titulo):
    plt.figure(figsize=(8, 8))
    for ruta in rutas:
        xs, ys = zip(*ruta)
        plt.plot(xs, ys, '-o', alpha=0.6)
    plt.title(titulo)
    plt.grid(True)
    plt.axis('equal')
    plt.show()

# Menú para elegir archivo
archivos = [f for f in os.listdir() if f.startswith("rutas_") and f.endswith(".txt")]
if not archivos:
    print("No se encontraron archivos de rutas.")
    exit(1)

print("Elige un archivo de rutas para visualizar:")
for i, f in enumerate(archivos):
    print(f"{i + 1}. {f}")

opcion = int(input("Opción: ")) - 1
archivo_elegido = archivos[opcion]

rutas = cargar_rutas(archivo_elegido)
plot_rutas(rutas, titulo=f"Visualización de {archivo_elegido}")
