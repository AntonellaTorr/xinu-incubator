#ifndef ESTADO_H
#define ESTADO_H

typedef struct {
    float temperatura;
    float humedad;
    float distancia_agua;
    int cant_dias;
} estado_t;

extern estado_t estado;

#endif