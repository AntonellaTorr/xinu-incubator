#!/usr/bin/env python3
# Lo mismo que hacer "telnet 192.168.4.1 80": abre el socket, manda bytes de a uno y recibe bytes de a uno.
import socket
import time

IP, PORT = "192.168.4.1", 80          # conectarse antes a la red AVR_wifi_ESP
PAUSA = 0.3                           # segundos de espera antes de cada byte (el AVR queda ocupado un rato)

AYUDA = """
Escribi los numeros que queres pedir (se mandan de a uno, en orden):
  1    temperatura, solo parte entera        12   temperatura con decimales
  3    humedad, solo parte entera            34   humedad con decimales
  5    distancia al agua (cm)
  Se pueden combinar, por ej: 12345 pide todo. q para salir.
"""

s = socket.create_connection((IP, PORT), timeout=5)
s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)   # cada send() sale como un paquete de 1 byte
s.sendall(b"0")                       # el primer byte abre la sesion...
print("AVR dice:", s.recv(1))         # ...y el AVR contesta 'A'


def pedir(c):
    """Manda 1 byte (por ej. '1') y devuelve el byte recibido como numero."""
    time.sleep(PAUSA)
    s.sendall(c.encode())
    return s.recv(1)[0]


def con_signo(b):                     # el byte viene 0..255; la temperatura puede ser negativa
    return b - 256 if b > 127 else b


def valor(ent, dec):                  # entero + decimales -> numero
    return ent - dec / 100 if ent < 0 else ent + dec / 100


def descripcion(opcion):              # texto para "Seleccionaste ..."
    partes = []
    for a, b, nombre in (("1", "2", "temperatura"), ("3", "4", "humedad")):
        if a in opcion and b in opcion:
            partes.append(f"{nombre} con decimales")
        elif a in opcion:
            partes.append(f"{nombre} (solo parte entera)")
        elif b in opcion:
            partes.append(f"decimales de {nombre}")
    if "5" in opcion:
        partes.append("distancia al agua")
    return ", ".join(partes)


print(AYUDA)
while True:
    opcion = input("> ").strip()
    if opcion == "q":
        break
    if not opcion or any(c not in "12345" for c in opcion):
        print("Solo se aceptan los digitos 1 a 5.")
        continue

    print(f"Seleccionaste {opcion}: {descripcion(opcion)}")
    try:
        r = {}
        for c in opcion:              # un byte por digito, esperando la respuesta de cada uno
            r[c] = pedir(c)
    except TimeoutError:
        print(f"Sin respuesta al pedir '{c}'. Reinicia la placa y volve a correr el script.")
        break

    if "1" in r:
        ent = con_signo(r["1"])
        if "2" in r:
            print(f"Temperatura: {valor(ent, r['2']):.2f} C   (bytes {ent} y {r['2']})")
        else:
            print(f"Temperatura: {ent} C")
    elif "2" in r:
        print(f"Decimales de temperatura: {r['2']}")

    if "3" in r:
        if "4" in r:
            print(f"Humedad: {valor(r['3'], r['4']):.2f} %   (bytes {r['3']} y {r['4']})")
        else:
            print(f"Humedad: {r['3']} %")
    elif "4" in r:
        print(f"Decimales de humedad: {r['4']}")

    if "5" in r:
        print(f"Agua: {r['5']} cm")

s.close()