#include <xinu.h>
#include <stdint.h>
#include <math.h>
#include "twi.h"
#include "sht31.h"
#include "tarea_controltyh.h"
#include "serial.h"

extern float sht31_temperature;
extern float sht31_humidity;

//Se conecta scl a A5 y sda a A4

int control_tyh(void){
    int loopCnt = 0;
    int enableHeater = 0;
    float t, h;

    serial_put_string("Inicia control_tyh\r\n");
    twi_init();
    sleepms(5000);


    if (!sht31_begin()) {
        serial_put_string("SHT31 begin FALLO\r\n");
        while(1) {
            // Loop infinito de error
            sleepms(1000);
        }
    }
    serial_put_string("SHT31 begin OK\r\n");

    while (1) {
        if (sht31_read_both(&t, &h)) {
            sync_wait(SEM_TYH);
                serial_put_string("En control_tyh: T=");
                serial_print_float(t);
                serial_put_string(" H=");
                serial_print_float(h);  
                temperatura = t;
                humedad = h;
            sync_signal(SEM_TYH);
        } else {
            // No se pudo leer el sensor, manejar error
            serial_put_string("No se pudo leer el sensor SHT31\r\n");
        }

        sleepms(1000);

        if (loopCnt >= 30) {
            enableHeater = !enableHeater;
            sht31_heater(enableHeater);
            loopCnt = 0;
        }

        loopCnt++;
    }
}